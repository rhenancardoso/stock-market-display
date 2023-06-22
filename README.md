# 1. T-DISPLAY-S3
Repository created for setting LilyGo T-Dipslay-S3 with the LVGL library

## 1.1 Device used
1. [LilyGo T-Display-S3](https://www.lilygo.cc/products/t-display-s3)


# 2. CONTRIBUTING
## 2.1 Environment setup
Tool development in OS system.

### 2.1.1 Requirements
1. ESP-IDF tool (version 5.2): [ESP-IDF website](https://docs.espressif.com/projects/esp-idf/en/release-v5.0/esp32s3/get-started/index.html)
2. Git

## 2.2 Inital development setup

1. After cloning the project to a local directory, add EDP-IDF tool to in the project folder
```
. $HOME/esp/esp-idf/export.sh
```

2. Make project files using MAKE
```
make all
```

## 2.3 ESP Partition update
During development increasing the partition app size was required due to size overflow.
See more in [ESP-IDF website](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)

1. Configure the `Flash Size` using `menuconfig`. ESP32-S3 has 8 MB of flash.

a. Open the IDF menuconfig
```
make config
```

b. Select `Serial flasher config` -> `Flash Size` -> `(X) 8MB`.

2. Update the partition-table in `/tools/input_partitions.csv`. To update the binary file with the updates run
```
make part-table
```


# Troubleshooting
* Ensure using `lvgl` library version `/release/v8.0`