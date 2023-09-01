//==============================================================
// Copyright Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#include <sycl/ext/intel/ac_types/ac_int.hpp>
// oneAPI headers
#include <sycl/ext/intel/fpga_extensions.hpp>
#include <sycl/sycl.hpp>

#include "exception_handler.hpp"

// Forward declare the kernel name in the global scope.
// This FPGA best practice reduces name mangling in the optimization report.
class KernelCompute;

using MyInt27 = ac_int<27, false>;
using MyInt54 = ac_int<54, false>;

// Using host pipes to stream data in and out of kernel
// IDPipeA and IDPipeB will be written to by the host, and then read by the kernel (device)
// IDPipeC will be written to by the kernel (device), and then read by the host
class IDPipeANative;
using InputPipeANative = sycl::ext::intel::experimental::pipe<IDPipeANative, unsigned>;
class IDPipeBNative;
using InputPipeBNative = sycl::ext::intel::experimental::pipe<IDPipeBNative, unsigned>;
class IDPipeCNative;
using OutputPipeCNative = sycl::ext::intel::experimental::pipe<IDPipeCNative, unsigned long>;

// This kernel computes multiplier result by using the C++ '*' operator
template <typename PipeIn1, typename PipeIn2, typename PipeOut>
struct NativeMult27x27 {

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
    MyInt54 res =(MyInt54)a_val * b_val;
    PipeOut::write(res);
  }
};

