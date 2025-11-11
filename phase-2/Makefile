CXX := arduino-cli
CXXFLAGS := --fqbn arduino:avr:nano

BUILD_DIR := build

SKETCH := src
BOARD := /dev/cu.usbserial-BG0006TA

all: $(BUILD_DIR)/$(SKETCH).ino.hex
	$(CXX) upload --input-dir $(BUILD_DIR) -p $(BOARD) $(CXXFLAGS) $(SKETCH)

build/$(SKETCH).ino.hex: $(SKETCH)/$(SKETCH).ino
	$(CXX) compile $(CXXFLAGS) --build-path $(BUILD_DIR) $(SKETCH)

clean:
	rm -r build/*

.PHONY: all clean
