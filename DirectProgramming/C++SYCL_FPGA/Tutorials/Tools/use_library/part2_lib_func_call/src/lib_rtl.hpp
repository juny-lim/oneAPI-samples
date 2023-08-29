// =============================================================
// Copyright Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#include <sycl/sycl.hpp>
#include <sycl/ext/intel/ac_types/ac_int.hpp>
#include <stdint.h>

// pad out to native types to avoid warnings
SYCL_EXTERNAL extern "C" uint64_t RtlDSPm27x27u(uint32_t x, uint32_t y);

