# `Use RTL with customized DSP Block` Sample

This FPGA tutorial demonstrates how to build SYCL device libraries from RTL sources and use them in your SYCL design.

| Area                  | Description
|:---                   |:---
| What you will learn   | How to integrate customised DSP block RTL into your oneAPI program and emulate it using a C model, as well as pulling the RTL directly into your full system design.
| Time to complete      | 30 minutes
| Category              | Concepts and Functionality

> **Note**: Even though the Intel DPC++/C++ OneAPI compiler is enough to compile for emulation, generating reports and generating RTL, there are extra software requirements for the simulation flow and FPGA compiles.
>
> For using the simulator flow, Intel® Quartus® Prime Pro Edition and one of the following simulators must be installed and accessible through your PATH:
> - Questa*-Intel® FPGA Edition
> - Questa*-Intel® FPGA Starter Edition
> - ModelSim® SE
>
> When using the hardware compile flow, Intel® Quartus® Prime Pro Edition must be installed and accessible through your PATH.
>
> :warning: Make sure you add the device files associated with the FPGA that you are targeting to your Intel® Quartus® Prime installation.


> :warning: When targeting an IP-only flow, the RTL library feature will not work when compiling to Quartus and will error out in the late-stage compile. RTL libraries will work in the simulation flow. This will be fixed in a future release. This is documented in the [compiler release notes](https://www.intel.com/content/www/us/en/developer/articles/release-notes/intel-oneapi-dpc-c-compiler-release-notes.html).

## Prerequisites

This sample is part of the FPGA code samples.
It is categorized as a Tier 3 sample that demonstrates the usage of a tool.

```mermaid
flowchart LR
   tier1("Tier 1: Get Started")
   tier2("Tier 2: Explore the Fundamentals")
   tier3("Tier 3: Explore the Advanced Techniques")
   tier4("Tier 4: Explore the Reference Designs")

   tier1 --> tier2 --> tier3 --> tier4

   style tier1 fill:#0071c1,stroke:#0071c1,stroke-width:1px,color:#fff
   style tier2 fill:#0071c1,stroke:#0071c1,stroke-width:1px,color:#fff
   style tier3 fill:#f96,stroke:#333,stroke-width:1px,color:#fff
   style tier4 fill:#0071c1,stroke:#0071c1,stroke-width:1px,color:#fff
```

Find more information about how to navigate this part of the code samples in the [FPGA top-level README.md](/DirectProgramming/C++SYCL_FPGA/README.md).
You can also find more information about [troubleshooting build errors](/DirectProgramming/C++SYCL_FPGA/README.md#troubleshooting), [running the sample on the Intel® DevCloud](/DirectProgramming/C++SYCL_FPGA/README.md#build-and-run-the-samples-on-intel-devcloud-optional), [using Visual Studio Code with the code samples](/DirectProgramming/C++SYCL_FPGA/README.md#use-visual-studio-code-vs-code-optional), [links to selected documentation](/DirectProgramming/C++SYCL_FPGA/README.md#documentation), etc.

## Purpose

This FPGA tutorial demonstrates how to allow DSP customisation in RTL sources and use them in your SYCL design. A RTL library can be used to customize DSP block behaviour, leading to embedding high performance FPGA code, handwritten in Verilog into your oneAPI program.

### Use of RTL libraries in SYCL

Prior to this tutorial, tutorial [Using FPGA Cross-Language Libraries](https://github.com/oneapi-src/oneAPI-samples/tree/master/DirectProgramming/C%2B%2BSYCL_FPGA/Tutorials/Tools/use_library) demostrated how to generate static library from an RTL source.

Files need to create SYCL target library from RTL source include:
- Verilog, System Verilog, or VHDL files that define the RTL component
- An Object Manifest File (.xml) which contains properties need to integrate RTL component into SYCL pipeline
- A header file containing valid SYCL kernel language and declares the signatures of functions implemented by the RTL component.
- A SYCL based emulation model file for RTL component

The RTL is used when compiling for the hardware whereas the emulation model is used when the oneAPI program is run on the FPGA emulator.
After having the library file, the function in the library can be called from SYCL kernel, without need to know the hardware design or implementation details on underlying functions in the library.
Refer to the tutorial [Using FPGA Cross-Language Libraries](https://github.com/oneapi-src/oneAPI-samples/tree/master/DirectProgramming/C%2B%2BSYCL_FPGA/Tutorials/Tools/use_library) for more details.

Given an workable RTL module that you has been using or verified, you might need to add a few tweak in order to integrate the RTL into oneAPI program.
1. An RTL module must use a single input Avalon® streaming interface. 
    Besides RTL library's interface, you must include a clock port, a resetn port, and Avalon® streaming interface input and output ports (that is, ivalid, ovalid, iready, oready) into your RTL module. Name the ports as specified here.

    > **Note**: The signal names must match the ones specified in the .xml file. An error occurs during library creation if a signal name is inconsistent

2. RTL library’s characteristics needs to be specified. For example, this tutorial RTL library has specified latency value, that needs to be specified in object manifest file (.xml) under ATTRIBUTES. For other ATTRIBUTES-specific elements, do refer to [Object Manifest File Syntax of an RTL Module](https://www.intel.com/content/www/us/en/docs/oneapi/programming-guide/2023-1/object-manifest-file-syntax-of-an-rtl-library.html) for additional information.

    > **Note**: It is challenging to debug an RTL module that works correctly on its own but works incorrectly as part of a SYCL kernel. Double-check all parameters under the ATTRIBUTES element in the object manifest file (.xml).


## Building the `use_rtl_dsp` Tutorial

> **Note**: When working with the command-line interface (CLI), you should configure the oneAPI toolkits using environment variables.
> Set up your CLI environment by sourcing the `setvars` script located in the root of your oneAPI installation every time you open a new terminal window.
> This practice ensures that your compiler, libraries, and tools are ready for development.
>
> Linux*:
> - For system wide installations: `. /opt/intel/oneapi/setvars.sh`
> - For private installations: ` . ~/intel/oneapi/setvars.sh`
> - For non-POSIX shells, like csh, use the following command: `bash -c 'source <install-dir>/setvars.sh ; exec csh'`
>
> Windows*:
> - `C:\Program Files(x86)\Intel\oneAPI\setvars.bat`
> - Windows PowerShell*, use the following command: `cmd.exe "/K" '"C:\Program Files (x86)\Intel\oneAPI\setvars.bat" && powershell'`
>
> For more information on configuring environment variables, see [Use the setvars Script with Linux* or macOS*](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-development-environment-setup/use-the-setvars-script-with-linux-or-macos.html) or [Use the setvars Script with Windows*](https://www.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-development-environment-setup/use-the-setvars-script-with-windows.html).

### On a Linux* System

1. Generate the `Makefile` by running `cmake`.

    ```bash
    mkdir build
    cd build
    ```

    To compile for the default target (the Agilex® 7 device family), run `cmake` using the command:
    ```
    cmake ..
    ```

    > **Note**: You can change the default target by using the command:
    >  ```
    >  cmake .. -DFPGA_DEVICE=<FPGA device family or FPGA part number>
    >  ```
    >
    > Alternatively, you can target an explicit FPGA board variant and BSP by using the following command:
    >  ```
    >  cmake .. -DFPGA_DEVICE=<board-support-package>:<board-variant>
    >  ```
    >
    > You will only be able to run an executable on the FPGA if you specified a BSP.

2. Compile the design through the generated `Makefile`. The following build targets are provided, matching the recommended development flow:

   * Compile for emulation (fast compile time, targets emulated FPGA device):

      ```bash
      make fpga_emu
      ```

   * Generate the optimization report:

     ```bash
     make report
     ```

   * Compile for FPGA Simulator

     ```bash
     make fpga_sim
     ```

   * Compile for FPGA hardware (longer compile time, targets FPGA device):

     ```bash
     make fpga
     ```

### On a Windows* System

1. Generate the `Makefile` by running `cmake`.

    ```
    mkdir build
    cd build
    ```

    To compile for the default target (the Agilex® 7 device family), run `cmake` using the command:
    ```
    cmake -G "NMake Makefiles" ..
    ```
    > **Note**: You can change the default target by using the command:
    >  ```
    >  cmake -G "NMake Makefiles" .. -DFPGA_DEVICE=<FPGA device family or FPGA part number>
    >  ```
    >
    > Alternatively, you can target an explicit FPGA board variant and BSP by using the following command:
    >  ```
    >  cmake -G "NMake Makefiles" .. -DFPGA_DEVICE=<board-support-package>:<board-variant>
    >  ```
    >
    > You will only be able to run an executable on the FPGA if you specified a BSP.

2. Compile the design through the generated `Makefile`. The following build targets are provided, matching the recommended development flow:

   - Compile for emulation (fast compile time, targets emulated FPGA device):

     ```
     nmake fpga_emu
     ```

   - Generate the optimization report:

     ```
     nmake report
     ```

   - Compile for simulation (fast compile time, targets simulated FPGA device, reduced problem size):

     ```
     nmake fpga_sim
     ```

   - Compile for FPGA hardware (longer compile time, targets FPGA device):

     ```
     nmake fpga
     ```

## Running the Sample

1. Run the sample on the FPGA emulator (the kernel executes on the CPU):

     ```bash
     ./use_rtl_dsp.fpga_emu     (Linux)
     use_rtl_dsp.fpga_emu.exe   (Windows)
     ```

2. Run the sample on the FPGA simulator device

    * On Linux
        ```bash
        CL_CONTEXT_MPSIM_DEVICE_INTELFPGA=1 ./use_rtl_dsp.fpga_sim
        ```
    * On Windows
        ```bash
        set CL_CONTEXT_MPSIM_DEVICE_INTELFPGA=1
        use_rtl_dsp.fpga_sim.exe
        set CL_CONTEXT_MPSIM_DEVICE_INTELFPGA=
        ```

3. Run the sample on the FPGA device (only if you ran `cmake` with `-DFPGA_DEVICE=<board-support-package>:<board-variant>`):

     ```bash
     ./use_rtl_dsp.fpga         (Linux)
     use_rtl_dsp.fpga.exe       (Windows)
     ```

### Example of Output

```bash
PASSED: result (13421772700) is correct!
```

## Examining the Reports
All compiler area estimation tools assume that the RTL module area is 0. The compiler does not currently support specifying an area model for RTL modules.
Optionally, you may specifiy the FPGA resources that the RTL library use (for example, DSPS value="1") in object manifest file under RESOURCES attribute and it will reflect exactly in optimasation report.

## License

Code samples are licensed under the MIT license. See [License.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/License.txt) for details.

Third-party program Licenses can be found here: [third-party-programs.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/third-party-programs.txt).
