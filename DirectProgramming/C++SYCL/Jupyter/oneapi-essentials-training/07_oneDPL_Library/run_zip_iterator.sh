#!/bin/bash
source /opt/intel/oneapi/setvars.sh > /dev/null 2>&1
/bin/echo "##" $(whoami) is compiling SYCL_Essentials Module7 -- oneDPL Extension APIs - 7 of 12 zip_iterator.cpp
icpx -fsycl lab/zip_iterator.cpp -w -o bin/zip_iterator
bin/zip_iterator
