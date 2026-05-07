# Renix/Bendix ECU Diagnostic Analyzer (Renault XR25, OBD1)

This software is written for my personal use, and is currently only supporting this one engine in my car and may also support others by displaying diag data as generic byte charts.  Its written in a way, that new models can be easily added in the future. The web-app is already usable if you have data as a file.  The optional ESP32 firmware currently unfinished. Its mostly about tying things together now, which is still time consuming. 

## This project contains:
  * A fast reactive web-app to run on your web-browser
     *  hosted here on GitHub: [https://zwiebert.github.io/r19ana/](https://zwiebert.github.io/r19ana/)
     *  to parse your local diagnostic data and show it as charts
         * if you want to try it without having data, you can enable it to fetch sample data from this repository
     *  to show live-data as charts (planned)
     *  to get live-data via USB or Bluetooth (planned)
  * An (optional) ESP32 firmware
     *  to record diagnostic data to sd-card.
     *  to stream live textual data to a Bluetooth terminal
     *  to host the web-app on its http server
         *  to do the same things the stand-alone web-app does
         *  to configure and control the ESP32 firmware (unfinished)
         *  to show-live data over web-socket (planned)
         *  to let the user download data from the sd-card via http (planned)
  * Some command-line tools to convert data to text format and back to binary:
     * To view the data as text for editing or feeding it to gnuplot
     * To convert edited data back to (byte-stuffed) binary format
  * Some gnuplot scripts, which are currently not maintained, since we now use the web-app with uplot.
  * Some sample data in original binary format: [main/data](https://github.com/zwiebert/r19ana/tree/main/main/data)

## Supported Car-Engines

* Generic: plots each single bytes of the diagnostic data packet (length 30..64) as byte-value 
* F3N-740: Engine in Renault 19 X53B  (54kW, gasoline, Renix-TBI, manual transmission), data-length:30-byte, 65000bps.
* more to come, if I get some data... 



## Building the Software
* the ESP32 firmware is an ESP-IDF project and should be build with "idf.py" the normal way from root folder.
* the tools for converting from bin or hex to human/gnuplot-friendly output are build with cmake from main folder
* gnuplot graphs are generated from the output of the tools or the firmware using some .gp files. these files handle the pre-filtering using standard tools like grep and awk.

## Using the Software
* The ESP32 has to bei Bluetooth-paired with the smart-phone or other devices which run the Bluetooth terminal. Bluetooth Classic SPP is currently used.  BLE may be added later. You can switch models and filter output-lines interactively in the Bluetooth-terminal app.
* the tools have some commandline options. call them with --help option to learn more.
* the gnuplot files can take the data from standard input. there are several options depending on the .gp file, for data-thinning (skip), range-selection (start, span, end). The .gp file for the "exp" model can take two line numbers (line_a, line_b) which will produce a diagram with 2 graphs.  the line number for the exp model are 1:1 the byte index in the frame (byte 0 is the program_version byte, the start bytes 0xff,0x00 are not counted). >t starts with line number 2 and byte 2. the line number 1 is, like always the hex-string of the complete data frame.
  
## Building the Hardware
* This is optional. The ESP32 is there to capture data from the diagnose port, format it into live view data which is then passed to a Bluetooth terminal and/or save it onto SD-card in the original binary format.
*  If you already have a means to capture the binary diagnose data sent from your ECU, then you can pipe this data to the gnuplot files by using the tool xr25-bin2human on your PC.
 
```
build/xr25-bin2human --infile "YourData.bin" --model "x53b-740" | gnuplot -e "skip=100" -p gp/x53b-1.gp
```
* The ESP32 hardware is just a normal development board with a standard ESP32 (the first version). I use an Olimex Gateway with SD-card slot.  Then build the well known interface between the car and the ESP32 UART2-RX pin. this is usual one transistor, one LED, a diode and some resistors.
* buy an adaptor cable "Renault-12pin to obd2-16 pin" and let the obd2 connector hang out at the passenger foot well, if the 12pin connector is located there.  also buy an adaptor obd2 to open wires which you can solder to the universal circuit board which contains your esp32 board and the logic-level-adapter transistor circuit. You can get the  supply voltage for the ESP32 from an USB cable to cigarette-lighter adaptor or you take the 12 Volt from the diagnose port and use a buck-stepdown convertor to generate the 5V from it.
