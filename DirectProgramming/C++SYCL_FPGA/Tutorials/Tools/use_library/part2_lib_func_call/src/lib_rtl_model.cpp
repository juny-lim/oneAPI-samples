//==============================================================
// Copyright Intel Corporation
//
// SPDX-License-Identifier: MIT
// =============================================================
#include "lib_rtl.hpp"

// This emulation model is only used during emulation, so it should functionally
// match the RTL in lib_rtl.v.
SYCL_EXTERNAL extern "C" uint64_t RtlDSPm27x27u(uint32_t x, uint32_t y) {
  return ((uint64_t)x * (uint64_t)y);
}

