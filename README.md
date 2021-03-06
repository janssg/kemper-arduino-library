# Kemper Profiler Arduino Library and DIY Kemper Remote 

**IMPORTANT:**
Current state of documentation may contains errors, and code will be refactored in a few days. 

This project consists of 3 sub projects:
- **C++ library** for Kemper and DIY Kemper Remote (`/libraries/kemper`)
- DIY Kemper Remote **Arduino** project (`/KemperRemoteDIYArduino`)
- DIY Kemper Remote **Windows** project (`/KemperRemoteDIYWin` and `/Simulator`)

C++ Library, is designed to run on both Arduino and Windows so that the later two projects can use the same C++ Kemper Library. Windows project has some mock classes, which mimics their Arduino counterparts. This allows you to code and debug using familiar tools. 

`KemperRemoteDIYWin` can talk with Kemper (directly using your sound card without Arduino). It will send and receive commands (like switch down, led states, display etc). It also have a simulated `KemperMock` class to be able to run even without Kemper Profiler. 

The `Simulator` node.js project, is the user interface layer. Using your browser you will be able to use your mouse and keyboard to interact with Kemper. Arduino project can also connect to `Simulator`. 

All of these properties allow several different configurations to code:

1. **You have Arduino and you have setup the complete circuit (details will be added later):**    
Use `KemperRemoteDIYArduino` project. It is a complete implementation of DIY Kemper Remote project. 
2. **You have Arduino and you have setup only MIDI interface (input and output) part of your circuit.**     
Use `KemperRemoteDIYArduino` and `Simulator` projects together. `Simulator` will communicate with Arduino through serial interface. You can use `Simulator` also in the previous setup.
3. **You don't setup MIDI circuit but you have a sound card with MIDI inputs and outputs**     
Connect your Kemper Profiler to your sound card using two MIDI cables. Use `KemperRemoteDIYWin` and `Simulator` projects together. 
4. **You don't have a sound card or you don't even have a Kemper Profiler.**     
This is also OK :) There is a mock class for Kemper inside `KemperRemoteDIYWin` project. It simulates most of the Kemper (apart from sound obviously) that are needed for a foot controller. With this option, you can install just a bunch of software and see how the foot controller behaves. 

## Installation
You can use Arduino IDE for development on Arduino. However I strongly suggest to use a modern IDE like Visual Studio. 

- Download and install [Visual Studio 2015 Community Edition](https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx)
- Download and install [Arduino Extension For Visual Studio](http://www.visualmicro.com/page/Arduino-Visual-Studio-Downloads.aspx)
- Download the repository as [zip file] (https://github.com/basarane/kemper-arduino-library/archive/master.zip), or [fork the repository] (https://help.github.com/articles/fork-a-repo/)
- Optionally if you want to fork the repository, I suggest you using [GitHub Extensions for Visual Studio](https://visualstudio.github.com/)

###Optional
If you want to use Simulator:

- Download and install [node.js](https://nodejs.org/)
- Open a command prompt. Goto `simulator` folder and run the following command:

```
cd Simulator
npm install
```

##Hardware
If you use Arduino (currently you need Arduino Mega), you at least need to prepare a circuit to send and receive MIDI messages to and from Kemper Profiler. You can follow [these instructions](http://www.instructables.com/id/Send-and-Receive-MIDI-with-Arduino/). 

Then connect Serial1 (TX1) and Serial2 (RX2) to MIDI input and output of Kemper Profiler, respectively. If you don't have Arduino Mega but Arduino UNO, you may use Software serial (as you need at least 3 serial interface; one for debug output and two for MIDI input and output). The serial ports can be changed in `libraries/Kemper/Kemper.cpp`.

DIY Kemper Remote project uses a TFT display, 14 buttons and 26 leds (8 of which are RGB). It uses 74HC595 shift registers for buttons, TLC5940 led drivers for leds and 4.3'' TFT display (ER-TFTM043-3). You can use other TFT displays also. All you have to do is extend the class `AbstractDisplay` and implement the functions it contains. `Display_ER_RA8875` class is such a class for ER-TFTM043-3 and it is located under `KemperRemoteDIYArduino` project.

*The schematics and details of hardware will be added later.*

##Usage

### Kemper Profiler Library

#### On Arduino

Library is located under Libraries/Kemper folder. Copy this folder and Libraries/MIDI folder to your [Arduino library folder](https://www.arduino.cc/en/Guide/Libraries#toc5). Restart Arduino IDE or Visual Studio.

The following code prints the name of the current rig or the performance to the serial port on each second. Run the program and open serial monitor. Change the current rig from Kemper and observe the serial monitor. 

```C++
#include <Kemper.h>
#include <MIDI.h>
#include <EEPROM.h>

USING_NAMESPACE_KEMPER

Kemper kemper;

unsigned long lastDebugTime = 0;

void setup()
{
  Serial.begin(9600);
  kemper.begin();
}

void loop()
{
  kemper.read();
  if (millis() - lastDebugTime > 1000) {
    if (kemper.state.mode == MODE_BROWSE)
      Serial.println(kemper.state.rigName);
    else if (kemper.state.mode == MODE_PERFORM)
      Serial.println(kemper.state.performanceNames[0]);
    lastDebugTime = millis();
  }
}
```

#### On Windows

Create a new empty C++ project on the same directory with `KemperRemoteDIYWin` in Visual Studio 2015. 

Then create a new .cpp file under "Source files" and add the following code (which is very similar to Arduino version). This code will print the name of the current rig each second. This code will connect the first MIDI input port and second MIDI output port on your system. Connect Kemper Profiler to corresponding MIDI ports. 

```C++
#include <stdio.h>

#include "Kemper.h"

USING_NAMESPACE_KEMPER

unsigned long lastDebugTime = 0;

int main(int argc, char**argv) {

	Kemper kemper;
	kemper.setMidiPorts(0, 1);  // input and output midi device ids
	kemper.begin();

	while (true) {
		kemper.read();
		if (millis() - lastDebugTime > 1000) {
			if (kemper.state.mode == MODE_BROWSE)
				printf("%s\n", kemper.state.rigName);
			else if (kemper.state.mode == MODE_PERFORM)
				printf("%s\n", kemper.state.performanceNames[0]);
			lastDebugTime = millis();
		}
	}
}
```

Right click on the project in the solution explorer, and click on properties. Add following values to the corresponding sections:

- Configuration Properties > C++ > General > Additional Include Directories    
    ..\KemperRemoteDIYWin\mocks    
    ..\KemperRemoteDIYWin    
    ..\libraries\Kemper    
    ..\libraries\MIDI    
- Configuration Properties > C++ > General > Preprocessor > Preprocessor Definitions    
    _MBCS;_CRT_SECURE_NO_WARNINGS
- Configuration Properties > Linker > Input > Additional Dependencies   
    winmm.lib

Right click on "Source files" and click on "Add > Existing Item...". Add all cpp files inside **KemperRemoteDIYWin** (except *KemperRemoteDIYWin.cpp*), **KemperRemoteDIYWin/Mocks** and **libraries/Kemper** folders. 

Compile and run the project. If you receive any errors during compilation or run, please unload all other projects first (esspecially KemperRemoteDIYArduino project). 


### DIY Kemper Profiler Remote
If you have the same Hardware setup described in this documentation, you may directly use `KemperRemoteDIYArduino` project. If you use different circuits for buttons update `MultiButton` project inside libraries folder. If you use different circuit for leds, update the relevant code inside `KemperRemoteDIYArduino` project. All other classes like `Kemper`, `KemperRemote` and `KemperRemoteDisplay` are hardware independent. They do not contain a single line of code about the hardware used except `KemperRemoteDisplay`. It assumes an TFT LCD screen is present. It won't work with character displays. Although you can completely discard `KemperRemoteDisplay` class and use your own  (since both `Kemper` and `KemperRemote` classes does not depend on `KemperRemoteDisplay`).

### Using Simulator
Simulator is a virtual foot controller. It is designed to work with both projects (Arduino and Windows). In order to run the simulator, select one of the options below then open "http://localhost" in your browser. *(You may need admin privilages to run the following commands)*

#### Using with Arduino (configurations 1 & 2)
If Simulator will be used together with KemperRemoteDIYArduino project, the following command should be run under **Simulator** folder:

```
node app.js serial COM7
```

where *COM7* is the serial port number of Arduino. It should be changed to the correct port number on your system. 

#### Using with MIDI interfaces of a sound card  (configuration 3)
If Kemper MIDI ports are connected to corresponding ports on a soundcard, the following command should be run:
```
node app.js midi 0 1
```

Here, 0 and 1 are the midi port numbers of the midi input and output, respectively. 

#### Using with Kemper Mock class (configuration 4)
If you don't have a soundcard or two midi cables or a Kemper, you can run Simulator by the following command line:
```
node app.js mock
```

This will start a simulated Kemper instance. 


The MIT License (MIT)

Copyright (c) 2015 Ersin Basaran

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.