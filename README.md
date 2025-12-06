# Steps to Build and Upload from Source
## Step 0: Dependencies
### Documentation
Please refer to the official
[Arduino CLI Documentation](https://docs.arduino.cc/arduino-cli/) on how to
install `arduino-cli`, install platform cores, and identify which port your
board is connected to.
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
