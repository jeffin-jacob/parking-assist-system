CXX := arduino-cli
CXXFLAGS := --fqbn arduino:avr:nano

BOARD ?= /dev/cu.usbserial-BG0006TA

all: build/src.ino.hex
	$(CXX) upload --input-dir build -p $(BOARD) $(CXXFLAGS) src
	$(CXX) monitor -p $(BOARD)

test: build/test.ino.hex
	$(CXX) upload --input-dir build -p $(BOARD) $(CXXFLAGS) test
	$(CXX) monitor -p $(BOARD)

build/src.ino.hex: src/src.ino | build
	$(CXX) compile $(CXXFLAGS) --build-path build src

build/test.ino.hex: test/test.ino | build
	$(CXX) compile $(CXXFLAGS) --build-path build test

build:
	mkdir -p $@

clean:
	rm -r build/*

.PHONY: all test build clean
