# Technical Guide: Setting Up VS Code with SEGGER and J-Link

This guide describes the setup on 64-bit Windows step by step,
so you can build the firmware from VS Code and flash it to the STM32 using J-Link.
It will be expanded as the setup progresses.

## Goal and Prerequisites

- **Editor:** Visual Studio Code
- **Build toolchain:** SEGGER Embedded Studio for ARM, as used by the existing project
- **Debug probe:** SEGGER J-Link via USB
- **Target microcontroller:** STM32F446RE
- **Debug interface to the microcontroller:** SWD
- **Project file:** [Robotarm.emProject](../Robotarm.emProject)

The existing compiler, startup code, and linker configuration are preserved.
Switching to GCC or CMake is not part of this setup.

> **Safety:** Keep the servo/motor power supply switched off during setup.
> The USB connection to the controller or J-Link may remain connected,
> provided it does not also power the servos. Flashing, resetting, or starting
> the firmware can trigger movement. Step 1 does not perform any flashing.

## How the Tools Work Together

VS Code is the editor and task launcher in this setup. The graphical SEGGER
Embedded Studio IDE does not need to be open: we use its **build toolchain**,
not its graphical interface, to compile and link the existing project.

| Component | Role in this setup |
| --- | --- |
| VS Code and the task configuration | Launch the build and flash commands in the required order. |
| Embedded Studio's `emBuild.exe` | Read the SEGGER project and invoke its configured compiler and linker to generate the ELF firmware file. |
| J-Link Commander (`JLink.exe`) from the J-Link Software and Documentation Pack | Load that ELF onto the target and issue reset/start commands. |
| J-Link USB driver | Allow Windows and the J-Link software to communicate with the connected debug probe. The driver itself does not build or flash firmware. |
| Nucleo onboard debugger running J-Link firmware | Bridge the PC's USB connection to the target STM32's SWD interface. |

The flash task explicitly uses `JLink.exe` from the separately installed
**J-Link Software and Documentation Pack**, not the flash button or GUI of
Embedded Studio. Installing the USB driver makes the probe accessible; it
does not replace the build toolchain or the flash utility.

```text
VS Code build task
  -> Embedded Studio emBuild + compiler + linker
  -> Output\Debug\Exe\Robotarm.elf

VS Code flash task (after a successful build)
  -> J-Link Commander reads the ELF and flash command file
  -> J-Link USB driver -> onboard J-Link probe -> SWD -> target STM32
  -> program firmware, reset, and start
```

The virtual COM port is separate from this flashing connection. Firmware
logging over UART or SEGGER RTT would require a suitable terminal or viewer;
the current tasks do not configure a log viewer or an interactive debugger.

## Step 1: Install the J-Link Software and Documentation Pack

### 1.1 Download

Open the official SEGGER download page:

- [J-Link Software and Documentation Pack](https://www.segger.com/downloads/jlink/)
- [Direct download: Windows 64-bit Installer](https://www.segger.com/downloads/jlink/JLink_Windows_x86_64.exe)

On the download page, select **Windows > 64-bit Installer**.
The direct link points to the currently available installer; its version number
may change with later releases.

The package includes J-Link software, USB drivers, manuals, and
firmware updates for the debug probe. It does **not** include the compiler
needed to build this project.

### 1.2 Installation

1. Run the downloaded installer, `JLink_Windows_x86_64.exe`.
2. Approve any Windows administrator permission prompt for the official
   SEGGER installer.
3. Read the license terms and proceed if you agree.
4. Keep the suggested installation path.
5. Include the USB drivers in the installation. If a driver selection option
   appears, leave it enabled. Approve any Windows prompt to install the
   SEGGER device drivers.
6. Complete the installation.
7. Unplug the J-Link from USB and reconnect it.

### 1.3 Verify the Installation

1. Open Windows Device Manager.
2. Check that the J-Link and its USB interfaces are recognized without a yellow
   warning icon. The exact device name may vary depending on the probe.
3. If a warning icon remains, open the device properties
   and check the error code under **Device status**.
   **Code 28** means the driver is not installed.
4. For Code 28, follow the troubleshooting steps below, then reconnect the probe.

A visible COM port alone does not confirm that the J-Link debug interface
is also installed correctly.

**Expected result:** The software package is installed, and Windows recognizes
the J-Link USB interface without driver errors. This does not yet verify
that an SWD connection to the STM32 or a flash operation works.

### 1.4 Troubleshooting: Nucleo Appears as "BULK interface"

On a Nucleo board whose onboard ST-LINK has previously been converted to
SEGGER J-Link firmware, the onboard debugger acts as a **J-Link STLink**.
This conversion affects the debug adapter, not the application firmware on
the target STM32.

Normally, installing the J-Link package with its USB drivers is sufficient for
Windows to associate the correct driver automatically. If Device Manager still
shows **BULK interface** with a yellow warning icon and **Code 28**, the Windows
driver association is missing. This alone does not mean the conversion failed
or that the onboard debugger needs to be reflashed.

To associate the installed driver manually:

1. Open **Device Manager**.
2. Right-click **BULK interface** and select **Update driver**.
3. Select **Browse my computer for drivers**.
4. Browse to the USB driver directory inside your J-Link installation.
   For J-Link V9.82 installed at the default location, use:

   ```text
   C:\Program Files\SEGGER\JLink_V982\USBDriver\x64
   ```

   Adjust the version directory to match your installation.
5. Enable **Include subfolders**, select **Next**, and approve any driver
   installation prompt.
6. Unplug the USB connection and reconnect it.
7. Verify that the debug interface now appears as **J-Link driver** without
   a warning icon and that Device Manager reports no error.

For the board checked during this setup, the debug interface reports
`USB\VID_1366&PID_0105&MI_02`. The official SEGGER driver includes this hardware
ID. A working virtual COM port is a separate interface and does not establish
that the debug driver is installed.

### 1.5 Verify USB Probe Access Without Flashing

Run the following in PowerShell, adjusting the installation path if needed:

```powershell
@('ShowEmuList', 'exit') |
    & 'C:\Program Files\SEGGER\JLink_V982\JLink.exe' -NoGui 1 -ExitOnError 1
if ($LASTEXITCODE -ne 0) {
    throw "J-Link Commander failed with exit code $LASTEXITCODE."
}
```

This opens the probe and lists connected emulators. It does not issue a target
`connect`, reset, or flash command.

**Verified on this setup:** Windows reports **J-Link driver** with problem code
**0**. J-Link Commander V9.82 reports **Connecting to J-Link ...O.K.**, identifies
the probe as **J-Link STLink**, and reads **VTref=3.300V**.
USB probe access is working; the SWD target connection and firmware flashing
have not yet been tested.

## Step 2: Install SEGGER Embedded Studio for ARM

### 2.1 Download and Install

The project uses the SEGGER compiler and linker. Installing the J-Link package
alone is therefore not enough to build the firmware.

1. Open the official [SEGGER Embedded Studio download page](https://www.segger.com/downloads/embedded-studio/).
2. Select **Embedded Studio for ARM** and the **Windows 64-bit installer**,
   not the RISC-V edition. If the original working ARM version is available,
   prefer it to minimize toolchain differences.
3. Run the installer and review the license terms. Proceed only if you agree
   and your intended use is covered by the applicable license.
4. Keep the suggested installation path.
5. Complete the installation. If a restart is requested, restart Windows.

### 2.2 Locate the Build Tools

1. Locate the Embedded Studio installation directory.
2. Check that its `bin` directory contains `emBuild.exe`, the command-line
   project builder that the VS Code build task will use.
3. Keep the installation path available for the next setup step.
4. Restart VS Code if it was open during installation so it can pick up any
   environment changes.

**Expected result:** SEGGER Embedded Studio for ARM and its command-line build
tool are installed. This does not yet confirm a successful firmware build.
The existing project will be built and checked when the VS Code task is configured.
Do not start a debug session or flash the board during this installation step.

**Verified on this setup:** SEGGER Embedded Studio for ARM **8.30a** is installed.
The command-line builder is located at:

```text
C:\Program Files\SEGGER\SEGGER Embedded Studio 8.30a\bin\emBuild.exe
```

## Step 3: Open the Project in VS Code

1. Use **File > Open Folder** to open the repository root containing
   [Robotarm.emProject](../Robotarm.emProject), not just an individual source file.
2. Trust the workspace only if you trust its contents. VS Code tasks execute
   local programs.
3. Optionally, open Extensions with **Ctrl+Shift+X** and install
   **C/C++** by Microsoft (`ms-vscode.cpptools`) for C/C++ editing.

The C/C++ extension is optional for building: the task below invokes SEGGER's
compiler independently. Compiler-specific IntelliSense configuration is not
included in this step.

Both tasks use `"problemMatcher": []` so they do not depend on an
extension-provided problem matcher such as `$gcc`. Compiler diagnostics remain
visible in the terminal but are not automatically added to the Problems panel.
Build failures are still detected through the process exit code and prevent
the dependent flash task from starting. If an older configuration reports
`Invalid problemMatcher reference: $gcc`, replace `"problemMatcher": "$gcc"`
with `"problemMatcher": []` in the build task.

**Cortex-Debug** (`marus25.cortex-debug`) is an option for the later debugging
step, not a prerequisite for this build. Debugger configuration is not yet
included.

## Step 4: Build the Firmware from VS Code

The repository includes a [build task](../.vscode/tasks.json) that uses the
existing SEGGER project without changing its compiler or linker configuration.

1. If your Embedded Studio version or installation directory differs from the
   verified setup, update the task's `command` to your `emBuild.exe` path.
   JSON paths must use escaped backslashes (`\\`).
2. Press **Ctrl+Shift+B** to run the default **SEGGER: Build Debug** task.
   Alternatively, use **Terminal > Run Task > SEGGER: Build Debug**.
3. Wait for the task to finish successfully. On failure, inspect the terminal
   output and resolve the reported error before attempting to flash.
4. The generated firmware is `Output\Debug\Exe\Robotarm.elf`.
   Build outputs are ignored by Git.

The equivalent PowerShell command, run from the repository root, is:

```powershell
& 'C:\Program Files\SEGGER\SEGGER Embedded Studio 8.30a\bin\emBuild.exe' `
    -config Debug -project Robotarm 'Robotarm.emProject'
if ($LASTEXITCODE -ne 0) {
    throw "SEGGER build failed with exit code $LASTEXITCODE."
}
```

**Verified on this setup:** The command above passed with exit code **0** using
Embedded Studio **8.30a** and generated `Output\Debug\Exe\Robotarm.elf`.
This verifies firmware compilation and linking, not runtime behavior.
The build task does not connect to the board, flash, reset, or start the firmware.

## Step 5: Build, Flash, and Start the Firmware

> **Safety:** This task overwrites the target firmware and automatically starts
> the new application. Keep the servo/motor power supply switched off for the
> first test. Ensure that USB does not also power the servos. Starting the
> application with powered actuators can cause immediate movement.

### 5.1 Flash Task Configuration

The [VS Code tasks](../.vscode/tasks.json) include
**SEGGER: Build, Flash & Start Debug** with these settings:

| Setting | Value |
| --- | --- |
| J-Link Commander | `C:\Program Files\SEGGER\JLink_V982\JLink.exe` |
| USB probe serial number | `776625679` (the probe verified during this setup) |
| Target device | `STM32F446RE` |
| Target interface | `SWD` |
| Interface speed | `1000` kHz |
| Build configuration | `Debug` |
| Firmware | `Output\Debug\Exe\Robotarm.elf` |

For another installation, update the flash task's `command` path.
For another probe, replace the value following `-USB` with its serial number.
Use the probe-listing command in step 1.5 to identify it. Selecting a specific
probe prevents accidentally flashing another connected J-Link target.

The flash task depends on **SEGGER: Build Debug**. VS Code must complete that
build successfully before starting J-Link, so a failed build does not trigger
flashing of an older firmware image.

The [J-Link command file](../scripts/flash-debug.jlink) resets and halts the
target, loads the ELF, resets again, and starts execution with `g`.
It uses the repository root as its working directory.
`-ExitOnError 1` makes Commander stop on an error instead of continuing to
the start command after a failed download.

### 5.2 Run the Task

1. Switch off the servo/motor power supply and connect the Nucleo's onboard
   J-Link USB port to the PC.
2. Close other debug sessions or tools that may be using the same probe.
3. In VS Code, select **Terminal > Run Task**.
4. Select **SEGGER: Build, Flash & Start Debug**.
5. Check the build terminal and J-Link terminal for errors. A successful build
   alone does not mean the flash step succeeded.
6. Confirm that Commander reports a successful download and exits with code
   **0**. The application is then expected to be running; actual application
   behavior still requires a separate hardware test.

**Ctrl+Shift+B remains build-only.** Flashing and starting require explicitly
selecting the flash task.

If Commander cannot open the probe, recheck the driver, serial number, USB
connection, and other programs using J-Link. If it opens the probe but cannot
connect to the target, check target power and the onboard debugger's SWD
connections/jumpers. Do not change the MCU type or erase the entire device
as a workaround.

**Validation status:** The task configuration and its build dependency have
been checked without executing the flash task. USB probe access and the Debug
build passed in the earlier steps. SWD connection, flash download, and
application startup still require the first controlled hardware test.

## Next Steps (to Be Added)

The following steps are planned but are not yet documented here as completed
or tested:

5. Complete the controlled hardware test of the flash-and-start task above.
6. Optional: Set up debugging with Cortex-Debug and breakpoints.

Cortex-Debug is not required for flashing alone.
VS Code extensions replace neither the build toolchain nor the J-Link drivers.
