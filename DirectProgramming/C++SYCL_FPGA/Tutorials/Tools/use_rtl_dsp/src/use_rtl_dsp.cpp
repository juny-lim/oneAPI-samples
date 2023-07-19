//==============================================================
// Copyright Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#include <sycl/sycl.hpp>
#include <sycl/ext/intel/fpga_extensions.hpp>
#include "lib.hpp"
#include "exception_handler.hpp"

#include <sycl/ext/intel/ac_types/ac_int.hpp>
#include <sycl/ext/intel/experimental/pipe_properties.hpp>
#include <sycl/ext/intel/experimental/pipes.hpp>
#include <sycl/ext/intel/prototype/interfaces.hpp>

// Forward declare the kernel name in the global scope.
// This FPGA best practice reduces name mangling in the optimization report.
class KernelCompute;
class KernelCompute_RTL;

using MyInt27 = ac_int<27, false>;
using MyInt54 = ac_int<54, false>;

class ID_PipeA;
using InputPipeA = sycl::ext::intel::experimental::pipe<ID_PipeA, unsigned long>;
class ID_PipeB;
using OutputPipeB = sycl::ext::intel::experimental::pipe<ID_PipeB, unsigned long>;
class ID_PipeC;
using InputPipeC = sycl::ext::intel::experimental::pipe<ID_PipeC, unsigned long>;
class ID_PipeD;
using OutputPipeD = sycl::ext::intel::experimental::pipe<ID_PipeD, unsigned long>;

template <typename pipeIN, typename pipeOUT>
struct mult27x27_soft {

  streaming_interface void operator()() const {
    MyInt27 a_val = pipeIN::read();
    MyInt27 b_val = pipeIN::read();
    MyInt54 res =(MyInt54)a_val * b_val;
    pipeOUT::write(res);
  }
};

template <typename pipeIN, typename pipeOUT>
struct mult27x27_rtl {

  streaming_interface void operator()() const {
    unsigned a_val = pipeIN::read();
    unsigned b_val = pipeIN::read();
    MyInt27 a = a_val;
    MyInt27 b = b_val;
    MyInt54 res = RtlDSPm27x27u(a_val, b_val);
    pipeOUT::write(res);
  }
};

int main() {
  //MyInt54 result = 0;
  unsigned long result = 0;
  unsigned long result_soft = 0;
  unsigned kA = 134217727;
  unsigned kB = 100;

  // Select the FPGA emulator (CPU), FPGA simulator, or FPGA device
#if FPGA_SIMULATOR
  auto selector = sycl::ext::intel::fpga_simulator_selector_v;
#elif FPGA_HARDWARE
  auto selector = sycl::ext::intel::fpga_selector_v;
#else  // #if FPGA_EMULATOR
  auto selector = sycl::ext::intel::fpga_emulator_selector_v;
#endif

  try {
    sycl::queue q(selector, fpga_tools::exception_handler);

    auto device = q.get_device();

    std::cout << "Running on device: "
              << device.get_info<sycl::info::device::name>().c_str()
              << std::endl;
    {
      
      InputPipeA::write(q, kA);
      InputPipeA::write(q, kB);
      //calling kernal that would compute multipier with soft logic
      q.single_task<KernelCompute>(mult27x27_soft<InputPipeA,OutputPipeB>{}).wait();

      result_soft = OutputPipeB::read(q);
    }
    {
      
      InputPipeC::write(q, kA);
      InputPipeC::write(q, kB);
      // mult27x27_rtl is an RTL library.
      q.single_task<KernelCompute_RTL>(mult27x27_rtl<InputPipeC,OutputPipeD>{}).wait();

      result = OutputPipeD::read(q);
    }

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

  // Compute the expected "golden" result
  unsigned long gold = (unsigned long) kA * kB;
  
  // Check the results
  if (result != gold || result_soft != gold) {
    std::cout << "FAILED: result (RTL: " << result << "; basic: " << result_soft << ") is incorrect! Expected " << gold << "\n";
    return -1;
  }
  std::cout << "PASSED: result (" << result << ") is correct!\n";
  return 0;
}

