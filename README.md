# Subaru-SSMII-DAQ
A Teensy 4.1 project, interfacing with the SSM Protocol in K-Line based Subarus through the OBD2 port to acquire engine sensor data, display on a TFT LCD, and log to an SD Card

Code is built and uploaded using VSCode and PlatformIO. It currently uses the Arduino framework.

CURRENTLY UNFINISHED

# Overview
In 2008, CAN bus based ISO 15765-4 became stadardized to be used for on-board diagnostics (OBD) in all new vehicles. Prior to this, manufacturers used varying communication protocols and layers for OBD. 

The vehicle this project was designed around is a 2006 Subaru Legacy 2.5i Limited with an EJ253 engine and 4EAT automatic transmission. Although it can vary based of model and region, Subaru's made between 2000 - 2007 used the Subaru Select Monitor II protocol over K-Line (ISO 9141) over the OBD port to communicate with and diagnose the vehicles ECUs. 

Following one of the famous head gasket failures that Subaru's are known for, a method to monitor and log engine sensor data was desired. Thus, this project began.

# Major Components Used
- Teensy 4.1 as the MCU
- Onboard SD slot with SDIO
- Onboard RTC
- [SN65HVDA195QDRQ1 LIN Transceiver](https://www.digikey.ca/en/products/detail/texas-instruments/SN65HVDA195QDRQ1/2094636?s=N4IgTCBcDaIMoDkBsBWAEgNQCIEECMAnCiALoC%2BQA)
- [Adafruit 2.8" Capacitive Touchscreen TFT Display](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/2090/9919535)
- [CR2032 Battery Holder](https://www.digikey.ca/en/products/detail/adafruit-industries-llc/1870/6827145) for the RTC
- [Sparkfun OBD2 to DB9 cable](https://www.digikey.ca/en/products/detail/sparkfun-electronics/CAB-10087/5721422)

# Development Notes
Originally, this project was planned to use a Raspberry Pi RP2040 Pico as the MCU, yet after initial testing, it was found that the processor was too slow to achieve a desireable refresh rate on the TFT display. Addionally, there were compatibility issues encountered when testing existing libraries for the TFT display, SD card reader over SPI, and others. With the significantly faster processor, onboard SD and RTC, and broader IO capabilities of the Teensy 4.1, it was chosen as the replacement, with much improved results. 

One issue faced with the Teensy 4.1 was regarding its voltage input. A criteria of the design was to be able to safely plug in / unplug a laptop from the Teensy while it was being powered from the vehicle. There were concerns with shorts when having two devices powering the 5V pin of the Teensy, as it has no on-board circuitry to isolate VUSB from the 5V VIN pin. To solve this, the VUSB jumper on the bottom of the Teensy was cut, which disconnects VUSB from the VIN pin. A female header was soldered to the inboard VUSB through-hole, and a diode was placed between the VUSB pin and VIN. This way, both the laptop and vehicle power coud be connected to the circuit safely. If just the laptop was connected, there would be a small voltage drop across the diode, making the voltage at the VIN pin around 4.3V. When the device was plugged into the vehicle, the LDO would provide 5V to the VIN pin, and no backfeeding would occur between the sources, thanks to the diode. 

## Bugs/To-Dos
- [x] Improve Data receive rate: When requesting ECU data in the single response mode, data would only be received at ~1Hz, which is far less than desireable for fast changing data like engine RPM, timing, MAP, etc. Solved by using the 'respond until interrupted' mode (byte 5 as 0x01 instead of 0x00), and improving efficiency of functions.
- [ ] 12V Supply: The 12V pin on the OBD2 port is always on (KL30), but an ignition power source (KL15) would be more desireable to eliminate the battery draw if the device is left plugged in when the vehicle is off. Need to find a source of switched power to use (while keeping the wiring clean), simply add a power switch that would need to be turned on/off manually with the vehicle ignition, or implement a software sleep state.
- [ ] 5V Regulator: A 7805 LDO (linear dropout) voltage regulator was used to get 5V power for the device, but the large voltage drop across the LDO is undesireable for efficiency and heat. Using a switching regulator circuit in a future iteration would be more desireable, but not mandatory.
- [ ] Log file name with date/time: Incorporate the date/time that the Teensy became powered into the log file name to ensure logs aren't overwritten. This was initially attempted by concatenating Strings into the file name, but caused the Teensy to crash. Also, add date/time back into outputHeaders / outputValues functions
- [ ] Add multiple screens: Use the touchscreen to toggle through multiple screens/gauge layouts. The display is too small to show all of the received data at once, thus multiple display layouts would be nice.
- [ ] Add TCU communication: It would be nice to get information from the transmission controller (TCU), but this is not described in ROMRaider. Research what could be requested from the TCU, and figure out if it is worth sacrificing ECU data rate for.
- [ ] Write script to convert csv data to MoTeC i2 log files: MoTeC i2 is an industry used data viewing software, typically used with MoTeC Motorsport ECUs. Is it possible to convert data into the MoTeC file types to view in i2? Would durastically improve the viewing experience of the logs.
- [ ] Uses classes more. TFT is a good example where a class should be used
- [ ] Move to an RTOS. Both for the learning, and for proper scheduling and timing of the program, using an RTOS over the Arduino Framework would be a welcomed upgrade to the project.

# References
- Original inspiration for the project: [Obesiance's Clock-pod mod with SSM](https://www.clubwrx.net/threads/clock-pod-mod-with-subarb-select-monitor-ecu-polling-and-arduino.134423369/)
- [RomRaider wiki](https://www.romraider.com/RomRaider/SsmProtocol)
- [rpkish's Subduino-SSM](https://github.com/rpkish/Subduino-SSM)
- [matprophet's subduino](https://github.com/matprophet/subduino)
- [Magister54's opengauge](https://github.com/magister54/opengauge)
- [KrisKasprzak's GraphingFunction](https://github.com/KrisKasprzak/GraphingFunction)
