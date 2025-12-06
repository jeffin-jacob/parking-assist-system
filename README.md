# Steps to Build and Upload from Source
## Step 0: Dependencies
### Documentation
Please refer to the official
[Arduino CLI Documentation](https://docs.arduino.cc/arduino-cli/) on how to
install `arduino-cli`, install the `arduino:avr:nano` platform core, and
identify which port your board is connected to.
### Software
- `make`
- `arduino-cli`
- `arduino:avr:nano` platform core
### Hardware
- Arduino Nano
- HC-SR04 Ultrasonic Sensor
- 5V Buzzer
- Jumper Wires
- Breadboard *(optional yet recommended)*
## Step 1: Configure the Circuit
Using jumper wires, configure the circuit in accordance to the following tables:

| HC-SR04 Pin | Arduino Nano Pin |
| :---: | :---: |
| Vcc | 5V |
| Trig | D11 |
| Echo | D12 |
| Gnd | GND |

| 5V Buzzer Pin | Arduino Nano Pin |
| :---: | :---: |
| + | 5V |
| - | GND |

## Step 2: Build and Upload
First, clone this repository. Next, in a terminal emulator, navigate to the
repository's root. Then, run `cd codebase` to navigate to the codebases' root.
Lastly, once in `codebase/`, run `make PORT=<port>`, where `<port>` is the port
to which the board is connected to.
## Step 3: Have Fun!
Place an obstacle at various distances from the ultrasonic sensor. If the
obstacle is 10-15 cm from the sensor, the buzzer will beep once per second. If
the obstacle is 5-10 cm from the sensor, the buzzer will beep twice per second.
If the obstacle is 0-5 cm from the sensor, the buzzer will continuously buzz.
## Step 4: Have Even More Fun! *(optional yet recommended)*
Power the Arduino Nano using a small battery pack and mount the components onto
a toy car. Simulate real-world scenarios, and see the parking assist system
prevent parking accidents in *real time* (no pun intended).
