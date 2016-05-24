# BTEMS
This is the repository for the BTEMS Sensor Stations. A majority of the documentation is in this file, but some is in the code.

### Uploading Instructions
First, change the `XX` in the filename on line 26 to whatever the BTEMS identifier code is. The current methodology is to use a two digit hexadecimal number. Then, using a 3.3v FTDI programmer such as [this](https://www.adafruit.com/products/284) or [this](https://www.sparkfun.com/products/9873), upload BTEMS.ino to a BTEMS. In the Arduino IDE 'tools' menu, the board should be set to `Arduino Pro or Pro Mini`, the processor should be set to `ATmega328 (3.3V, 8Mhz)`, and the programmer should be set to whatever programmer is used. `AVRISP mkll` often works. The port should be set to whatever port the Arduino is plugged into. The power switch on the BTEMS should be set to off when programming.

### Running Instructions
Place the BTEMS where measurements should take place. Ensure there's an SD card inserted into the SD slot. Turn the switch from 'OFF' to 'ON'. After ~3 seconds, the green LED will turn on and remain lit for ~2 seconds. From then on, the BTEMS is running. The green LED should blink every ~10 seconds, implying a measurement was taken. If a red light comes on after turning on, the likely culprit is either the SD card or the real time clock. Make sure the SD card works and has space, and make sure the real time clock works and has a backup battery plugged in.

To turn off, wait until the green LED blinks, then turn the switch from 'ON' to 'OFF' soon after. It is important to wait until the LED is finished blinking to make sure the file is successfully saved.

### File Descriptions
* BTEMS/
	* `BTEMS.ino` - BTEMS arduino sketch
* BTEMS PCB/
	* `BTEMS.brd` - Eagle board file for BTEMS PCB
	* `BTEMS.sch` - Eagle schematic file for BTEMS PCB
	* `BTEMS.*.ger` - BTEMS gerber files for manufacturing
	* `BTEMS.*.xln` - More BTEMS manufacturing files
	* `eagle.epf` - Eagle preferences file
* Data Sheets/
	* Contain part data sheets for posterity
* Matlab Scripts/
	* `Battery_Analysis.m` - Estimates the battery life given certain assumptions.
	* `BTEMS_Plotter.m` - Requests a BTEMS output .csv file from the user and plots some common plots. Very simple
* Radiation Shield/
	* `Radiation_Shield_RevD.*` - The CAD files for the radiation shield. Edit the .SLDPRT file, submit the .STL file for 3D printing, and view the .pdf file. The .STL file can be easily previewed within Github.
	* `SHT_Plug.*` - The CAD files for the plug that holds the SHT21. Edit the .SLDPRT file, submit the .STL file for 3D printing, and view the .pdf file. The .STL file can be easily previewed within Github
* `Bill of Materials.xlsx` - BTEMS bill of materials. Should be 99.9% complete. Missing any mounting hardware or insulation materials.

### Troubleshooting
* Red Light: SD card problem. Is there an SD card? Is it good? Is it formatted?
* No Lights: Possible sensor problem. Are they plugged in? Are they good? Are they plugged in backwards?

### To-Do
1. Testing
2. Check current measurements with nice multimeter. Get average of several boards

### Assembly Instructions
1. Solder SD card holder to PCB
2. Solder female RTC header to PCB
3. Solder four male header pins to RTC in the PWR, GND, SCL, SDA holes
4. Solder resistors in corect place
5. Solder power switch in place
6. Solder power screw terminal in place. Make sure screw terminal is straight and fits flush on the PCB!
7. Solder Arduino in place. The programming header on the Arduino should be near on the side of the power switch. Use regular male headers, and the long side of the headers should be pointing up from the board. Make sure all the pins on the Arduino, especially the four pins in the middle of the arduino, are passed through to the PCB.
8. Solder male headers for sensor plug ins in place
Detail to be added...