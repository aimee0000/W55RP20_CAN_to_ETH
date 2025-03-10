# How to Test W55RP20_CAN_to_ETHERNET

Reference CAN driver code is based on [**CAN2040**][link-can_driver].

Please install the tools required for testing through the following link [**CAN EXAMPLE README**][link-can_example_readme].


## Step 1: Prepare software

The following serial terminal programs are required for CAN TO ETHERNET example test, download and install from below links.

- [**Tera Term**][link-tera_term]
- [**Hercules**][link-hercules]
- [**Klipper**][link-klipper]
- [**CANViewer**][link-canviewer]


## Step 2: Prepare hardware

1. Connect ethernet cable to W55RP20-EVB-Pico ethernet port.

2. Connect W55RP20-EVB-Pico to desktop or laptop using USB Type-C cable. 

3. Connect Raspberry Pi Pico to desktop or laptop using 5 pin micro USB cable.

4. Connect as shown in the diagram below. 

In this example, the CAN transceiver SN65HVD230 is used. The example code runs on the W55RP20-EVB-Pico (target board) and uses a Raspberry Pi Pico for testing.

![][link-hardware_wiring]



## Step 3: Test CAN TO ETHERNET Example

1. Run Klipper

Download Klipper code

```cpp
# Linux
$ git clone https://github.com/Klipper3d/klipper
$ sudo apt-get update && sudo apt-get install build-essential libncurses-dev libusb-dev libnewlib-arm-none-eabi gcc-arm-none-eabi binutils-arm-none-eabi libusb-1.0 pkg-config
```

Build USB to CAN bus code.

```cpp
# Linux
$ make menuconfig
```

After setting it up as shown in the picture below, set the baudrate through CAN bus speed.

![][link-menuconfig_for_test_tools]


Flashing to Raspberry Pi Pico board. 

```cpp
# Linux
$ make
$ make flash FLASH_DEVICE=2e8a:0003
```

If the tools for the Raspberry Pi Pico(test board) are installed correctly, the result will appear as shown in the diagram below.

![][link-raspberry_pi_pico_is_running]

2. Build & Run

Click 'build' in the status bar at the bottom of Visual Studio Code or press the 'F7' button on the keyboard to build.

When the build is completed, 'main.uf2' is generated in 'W55RP20_CAN_to_ETH/build/examples/main' directory.

While pressing the BOOTSEL button of W55RP20-EVB-Pico, the USB mass storage 'RPI-RP2' is automatically mounted.

![][link-raspberry_pi_pico_usb_mass_storage]

Drag and drop 'main.uf2' onto the USB mass storage device 'RPI-RP2'.

Connect to the serial COM port of W55RP20-EVB-Pico with Tera Term.

![][link-connect_to_serial_com_port]

![][link-see_information_of_w55rp20_evb_pico]


3. Setting 

You can access the webpage as follows to read or reconfigure the Network information, Connection information, and CAN information settings.

![][link-config_w55rp20_evb_pico]

4. Run CANViewer

Run CANViewer program in 'W55RP20_CAN_to_ETH/examples/test/' folder.

```cpp
 cd ./examples/test
 python3 ./canview.py
```

![][link-can_viewer_window]

If you have set operation mode to TCP Server on 3. Setting, you must connect as a client in the CANViewer program.

Enter the IP Address and Port as specified Local IP & Local Port in the 3. Setting, then click the Connect button.

If the connection is successful, the message 'Connected to server' will appear at the bottom of the window.

![][link-connect_as_client_in_canviewer]

If you have set operation mode to TCP Client on 3. Setting, you must connect as a server in the CANViewer program.

Enter the Port as specified Remote Port in the 3. Setting, then click the Connect button.

If the connection is successful, the message 'Client connected ' will appear at the bottom of the window.

![][link-connect_as_server_in_canviewer]


5. Test send data Ethernet -> CAN

To check if data is being transmitted from Ethernet to the CAN device, execute the following steps.

```cpp
# Linux
$ candump can0
```

In the CAN Viewer program, enter Format, ID, Data Length, and Data as shown in the image below, then click the Send button.

![][link-send_message_in_can_viewer]


If the data has been successfully transmitted, you can verify it as follows.

![][link-see_message_eth_to_can_in_wireshark]

![][link-recv_message_in_raspberry_pi_pico]


6. Test send data CAN -> Ethernet

To check if data is being transmitted from CAN device to the Ethernet, execute the following steps.

```cpp
# Linux
$ cansend can0 123#8877665544332211
```

![][link-send_message_in_raspberry_pi_pico]

If the data has been successfully transmitted, you can verify it as follows.

![][link-see_message_can_to_eth_in_wireshark]


![][link-recv_message_in_can_viewer]


7. Filtering 

If you want to filter incoming CAN messages, you can using Filter ID and Filter Mask.

For example, if you set it to Spec 2.0 A, with a filter ID of 0x200 and a filter mask of 0x7F0, only data with an ID between 0x200 and 0x20F will be received.

![][link-config_can_filter_1_w55rp20-evb-pico]


For example, if you set it to Spec 2.0 B, with a filter ID of 0x1F821990  and a filter mask of 0x1FFFFFF8, only data with an ID between 0x1F821990 and 0x1F821997 will be received.

![][link-config_can_filter_2_w55rp20-evb-pico]



<!--
Link
-->

[link-can_driver]: https://github.com/KevinOConnor/can2040
[link-can_example_readme]: https://github.com/WIZnet-ioNIC/WIZnet-PICO-C/blob/main/examples/can/README.md
[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
[link-hercules]: https://www.hw-group.com/software/hercules-setup-utility
[link-klipper]: https://www.klipper3d.org/
[link-canviewer]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/examples/test/canview.py
[link-hardware_wiring]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/hardware_wiring.png
[link-menuconfig_for_test_tools]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/menuconfig_for_test_tools.png
[link-raspberry_pi_pico_is_running]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/raspberry_pi_pico_is_running.png
[link-raspberry_pi_pico_usb_mass_storage]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/raspberry_pi_pico_usb_mass_storage.png
[link-connect_to_serial_com_port]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/connect_to_serial_com_port.png
[link-see_information_of_w55rp20_evb_pico]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/see_information_of_w55rp20_evb_pico.png
[link-config_w55rp20_evb_pico]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/config_w55rp20_evb_pico.png
[link-can_viewer_window]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/can_viewer_window.png
[link-connect_as_client_in_canviewer]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/connect_as_client_in_canviewer.png
[link-connect_as_server_in_canviewer]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/connect_as_server_in_canviewer.png
[link-send_message_in_can_viewer]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/send_message_in_can_viewer.png
[link-see_message_eth_to_can_in_wireshark]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/see_message_eth_to_can_in_wireshark.png
[link-recv_message_in_raspberry_pi_pico]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/recv_message_in_raspberry_pi_pico.png
[link-send_message_in_raspberry_pi_pico]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/send_message_in_raspberry_pi_pico.png
[link-see_message_can_to_eth_in_wireshark]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/see_message_can_to_eth_in_wireshark.png
[link-recv_message_in_can_viewer]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/recv_message_in_can_viewer.png
[link-config_can_filter_1_w55rp20-evb-pico]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/config_can_filter_1_w55rp20-evb-pico.png
[link-config_can_filter_2_w55rp20-evb-pico]: https://github.com/aimee0000/W55RP20_CAN_to_ETH/blob/main/static/images/getting_started/config_can_filter_2_w55rp20-evb-pico.png