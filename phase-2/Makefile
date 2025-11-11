CXX := arduino-cli
CXXFLAGS := --fqbn arduino:avr:nano

BOARD ?= /dev/cu.usbserial-BG0006TA

all: build/src.ino.hex
	$(CXX) upload --input-dir build -p $(BOARD) $(CXXFLAGS) src
	$(CXX) monitor -p $(BOARD)

build/src.ino.hex: src/src.ino src/buzzer.ino | build
	$(CXX) compile $(CXXFLAGS) --build-path build src

build:
	mkdir -p $@

clean:
	rm -r build/*

.PHONY: all clean
