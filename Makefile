PROJECT_NAME := Stock-Display

USB_PORT=/dev/cu.usbmodem101

.PHONY: all
all: build 

.PHONY: target_config
target_config:
	echo "Setting ESP-IDF target to ESP32-S3"
	idf.py set-target esp32s3

.PHONY: dependencies
dependencies:
	echo "Build dependencies library"
	git submodule update --recursive
	idf.py reconfigure

.PHONY: build
build:
	echo "Building LCD testing project"
	idf.py build

.PHONY: deploy
deploy: build
	echo "Deploying project to connected ESP32-S3"
	idf.py -p $(USB_PORT) flash

.PHONY: dep-mon
dep-mon: build deploy
	echo "Deploying project to connected ESP32-S3"
	idf.py -p $(USB_PORT) monitor

.PHONY: clean
clean:
	echo "Cleaning ESP-IDF build"
	rm -rf ./build/

.PHONY: config
config:
	echo "ESP-IDF menuconfig for ESP32 configuration"
	idf.py menuconfig