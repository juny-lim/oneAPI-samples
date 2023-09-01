#include "use_library.hpp"
#include "use_native.hpp"

bool testNative(sycl::queue q) {
  std::cout << "Test kernel that uses only SYCL code..." << std::endl;
  unsigned long result_native = 0;
  unsigned kA =
      134217727;  // 0x7FFFFFF is the largest possible ac_int<27, false>.
  unsigned kB = 100;

  // write data to host-to-device pipes
  InputPipeANative::write(q, kA);
  InputPipeBNative::write(q, kB);
  // launch kernel that infers a multiplier automatically
  q.single_task<KernelCompute>(
       NativeMult27x27<InputPipeANative, InputPipeBNative, OutputPipeCNative>{})
      .wait();
  // read data from device-to-host pipe
  result_native = OutputPipeCNative::read(q);

  // Check the results
  unsigned long expected_result = (unsigned long)kA * kB;
  if (result_native != expected_result) {
    std::cout << "FAILED: result (" << result_native
              << ") is incorrect! Expected " << expected_result << "\n";
    return false;
  }
  std::cout << "PASSED: result is correct!\n";
  return true;
}

bool testRtl(sycl::queue q) {
  std::cout << "Test kernel that uses RTL library..." << std::endl;
  unsigned long result_rtl = 0;
  unsigned kA =
      134217727;  // 0x7FFFFFF is the largest possible ac_int<27, false>.
  unsigned kB = 100;

  // write data to host-to-device pipes
  InputPipeARTL::write(q, kA);
  InputPipeBRTL::write(q, kB);
  // launch a kernel to that uses a multiplier defined in RTL
  q.single_task<KernelComputeRTL>(
       RtlMult27x27<InputPipeARTL, InputPipeBRTL, OutputPipeCRTL>{})
      .wait();
  // read data from device-to-host pipe
  result_rtl = OutputPipeCRTL::read(q);

  // Check the results
  unsigned long expected_result = (unsigned long)kA * kB;
  if (result_rtl != expected_result) {
    std::cout << "FAILED: result (" << result_rtl << ") is incorrect! Expected "
              << expected_result << "\n";
    return false;
  }
  std::cout << "PASSED: result is correct!\n";
  return true;
}

int main() {
  // Select the FPGA emulator (CPU), FPGA simulator, or FPGA device
#if FPGA_SIMULATOR
  auto selector = sycl::ext::intel::fpga_simulator_selector_v;
#elif FPGA_HARDWARE
  auto selector = sycl::ext::intel::fpga_selector_v;
#else  // #if FPGA_EMULATOR
  auto selector = sycl::ext::intel::fpga_emulator_selector_v;
#endif

  bool passed = true;

  try {
    sycl::queue q(selector, fpga_tools::exception_handler);

    auto device = q.get_device();

    std::cout << "Running on device: "
              << device.get_info<sycl::info::device::name>().c_str()
              << std::endl;

    passed &= testNative(q);
    passed &= testRtl(q);

  } catch (sycl::exception const &e) {
    // Catches exceptions in the host code
    std::cerr << "Caught a SYCL host exception:\n" << e.what() << "\n";

    // Most likely the runtime couldn't find FPGA hardware!
    if (e.code().value() == CL_DEVICE_NOT_FOUND) {
      std::cerr << "If you are targeting an FPGA, please ensure that your "
                   "system has a correctly configured FPGA board.\n";
      std::cerr << "Run sys_check in the oneAPI root directory to verify.\n";
      std::cerr << "If you are targeting the FPGA emulator, compile with "
                   "-DFPGA_EMULATOR.\n";
    }
    std::terminate();
  }

  // Check the results
  if (!passed) {
    std::cout << "FAILED\n";
    return -1;
  } else {
    std::cout << "PASSED\n";
    return 0;
  }
}