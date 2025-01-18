## How to Use OpenOCD for OpenSDA

OpenOCD (Open On-Chip Debugger) and OpenSDA (Open Serial Debug Adapter) are tools related to debugging and programming microcontrollers.

**OpenOCD** is an open source tool used for debugging, programming and controlling embedded devices. It supports standard debug interfaces like JTAG and SWD and is widely used on ARM Cortex-based systems. It acts as an intermediary between the PC and the MCU, connecting to the device via a hardware interface (e.g. CMSIS-DAP, ST-Linx, J-Link), offering an interface to the user and to oher tools (like GDB) to communicate with the MCU.

**CMSIS-DAP** is the interface firmware for a Debug Unit that connects the Debug Port to USB. Debuggers, which execute on a host computer, connect via USB to the Debug Unit and to the Device that runs the application software. The Debug Unit connects via JTAG or SW to the target Device.

**OpenSDA** is a hardware-based programming and debugging interface developed by NXP for its development boards. It acts as a bridge between the computer and the microcontroller on the board. It's based in the CMSIS-DAP, and it allows drap-and-drop flashing. When connected to your computer, OpenSDA usually presents itself as a USB storage device. You can drag-and-drop the firmware binary to the drive to write it to the microcontroller. In the board, the chip with the OpenSDA firmware is located next to the USB port (the OpenSDA one). It also converts USB to TTL levels.

We can program the MCU just by copying the binary to it's folder, but apparently the K32's OpenSDA firmware don't automatically reset the MCU after the flashing (it need a manual push). So, I decided to automate the process.

### Setup

To install OpenOCD in Linux:

```
sudo apt-get install openocd
```

Connect the board (FRDM-K32L2B3 in my case) to the computer via USB cable (use the OpenSDA port), and check if it's listed as an USB device:

```
lsusb
```

The output should be something like this:

```
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 001 Device 002: ID 320f:5078 Evision RGB Keyboard
Bus 001 Device 003: ID 1c4f:0048 SiGma Micro USB Mouse
Bus 001 Device 004: ID 04ca:3015 Lite-On Technology Corp. Qualcomm Atheros QCA9377 Bluetooth
Bus 001 Device 014: ID 0d28:0204 NXP ARM mbed
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
```

Note the **NXP ARM mbed** line. It denotes our board with OpenSDA. Now we are good to go.

OpenOCD uses configuration files to define the debugging interfaces and board parameters. We will use the CMSIS-DAP interface, which is compatible with OpenSDA. You can also use the JTAG interface, given that you have the necessary hardware.     

We will create a configuration file for the interface, ```interface/cmsis-dap.cfg```:

```
interface cmsis-dap
transport select swd
```

CMSIS-DAP is the interface between the PC and the MCU, and SWD (Serial Wire Debug) is the debugging interface between the OpenSDA and the MCU. Actually, SWD is a serial debugging protocol used by ARM Cortex microcontrollers. It is simpler and uses fewer pins than the traditional JTAG protocol, making it ideal for embedded devices.

We also need configuration files for the board, namely ```target/k32.cfg```:

```
set CHIPNAME k32
source [find target/kx.cfg]
```

This configuration file is based in another one, provided by NXP. The OpenOCD provides various files with board (in tcl/target and tcl/board) a interface (in tcl/interfaces) configurations to use:

```
git clone git@github.com:openocd-org/openocd.git
```

We want the following file:

```
tcl/target/klx.cfg
```

It has the configurations for the K32 board. Put it in the ```target``` folder of the project.

We also have to setup the permissions to acess the USB port via OpenOCD, or else we can have the error:

```
Error: unable to open CMSIS-DAP device 0xd28:0x204
Error: unable to find a matching CMSIS-DAP device
```

So, we have to setup access rules. We can create a ```udev``` rule to allow access:

```
sudo gedit /etc/udev/rules.d/99-openocd.rules
```

And add the following line:

```
SUBSYSTEM=="usb", ATTR{idVendor}=="0d28", ATTR{idProduct}=="0204", MODE="0666"
```

Replace the values for the attributes based in the device ID viewed in ```lsusb```, in the format ```0d28:0204```. 

Then you update the udev rules and, at last, reconect the board:

```
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### Using

In a terminal initialize the OpenOCD server:

```
openocd -f interface/cmsis-dap.cfg -f target/k32.cfg
```

In another terminal, connect to the OpenOCD server via ```telnet``` client:

```
telnet localhost 4444
```

Now we can issue commands to upload the binary file: 

```
reset halt
flash write_image erase build/firmware.bin 0x00000000
reset run
```

Basically:

- reset halt: Resets and pauses the execution
- flash write_image erase firmware.bin 0x00000000: Flashes the binary in the flash, starting in the address 0x00000000.
- reset run: Resets and executes firmware.

Another way to upload to the microcontroller (in a single line) is:

```
openocd -f interface/cmsis-dap.cfg -f target/k32.cfg -c "program build/firmware.elf verify reset exit"
```

It will program the firmware, verify it, reset the microcontroller (what we would have to do manually using just the drag-and-drop) and exit the OpenOCD.

We can also integrate OpenOCD with GDB to debug the code:

```
arm-none-eabi-gdb build/firmware.elf
```

Make sure you compile with ```-g``` flag and debug the ELF file, because it has the debug symbols necessary. Then in GDB connect to OpenOCD server in port 3333: 

```
(gdb) target remote localhost:3333
```

### 
