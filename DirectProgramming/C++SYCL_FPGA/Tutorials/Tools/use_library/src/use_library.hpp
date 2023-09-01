//==============================================================
// Copyright Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#include <sycl/ext/intel/ac_types/ac_int.hpp>
// oneAPI headers
#include <sycl/ext/intel/fpga_extensions.hpp>
#include <sycl/sycl.hpp>

#include "lib_rtl.hpp"

// RTL Library will use the Verilog model during hardware generation, and the
// c++ model during emulation.
#include "exception_handler.hpp"
#include <stdint.h>
// Forward declare the kernel name in the global scope.
// This FPGA best practice reduces name mangling in the optimization report.
class KernelComputeRTL;



// Using host pipes to stream data in and out of kernal
// IDPipeA and IDPipeB will be written to by the host, and then read by the kernel (device)
// IDPipeC will be written to by the kernel (device), and then read by the host
class IDPipeARTL;
using InputPipeARTL = sycl::ext::intel::experimental::pipe<IDPipeARTL, uint32_t>;
class IDPipeBRTL;
using InputPipeBRTL = sycl::ext::intel::experimental::pipe<IDPipeBRTL, uint32_t>;
class IDPipeCRTL;
using OutputPipeCRTL = sycl::ext::intel::experimental::pipe<IDPipeCRTL, uint64_t>;

// This kernel computes multiplier result by calling RTL function RtlDSPm27x27u
template <typename PipeIn1, typename PipeIn2, typename PipeOut>
struct RtlMult27x27 {

  // use a streaming pipelined invocation interface to minimize hardware
  // overhead
  auto get(sycl::ext::oneapi::experimental::properties_tag) {
    return sycl::ext::oneapi::experimental::properties{
        sycl::ext::intel::experimental::streaming_interface_accept_downstream_stall, 
        sycl::ext::intel::experimental::pipelined<1>};
  }
  
  void operator()() const {
    MyInt27 a_val = PipeIn1::read();
    MyInt27 b_val = PipeIn2::read();
    MyInt54 res = RtlDSPm27x27u(a_val, b_val);
    PipeOut::write(res);
  }
};

