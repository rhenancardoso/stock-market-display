PROJECT_NAME := Stock-Display

USB_PORT=/dev/cu.usbmodem2101

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

.PHONY: debug-monitor
debug-monitor: deploy
	echo "Deploying project to connected ESP32-S3"
	idf.py -p $(USB_PORT) monitor --print-filter="*:D"

.PHONY: clean
clean:
	echo "Cleaning ESP-IDF build"
	rm -rf ./build/

.PHONY: config
config:
	echo "ESP-IDF menuconfig for ESP32 configuration"
	idf.py menuconfig

.PHONY: part-table-csv2bin
part-table:
	echo "ESP-IDF export input_partitions.csv to binary"
	python ./tools/gen_esp32part.py ./tools/input_partitions.csv ./build/partition_table/partition-table.bin