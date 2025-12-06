CXX := arduino-cli
CXXFLAGS := --fqbn arduino:avr:nano

PORT ?= /dev/cu.usbserial-BG0006TA

all: build/src.ino.hex
	$(CXX) upload --input-dir build -p $(PORT) $(CXXFLAGS) src
	$(CXX) monitor -p $(PORT)

build/src.ino.hex: src/src.ino | build
	$(CXX) compile $(CXXFLAGS) --build-path build src

build:
	mkdir -p $@

clean:
	rm -r build/*

.PHONY: all build clean
