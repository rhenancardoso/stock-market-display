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

# 2.4 LVGL Library Usage
## 2.4.1 Converting PNG image to C array
To use PNG images with LVGL library, it is required to convert the images first.
Luckily there is an online converter that makes it simpler.

1. Go to [LVGL Converter](https://lvgl.io/tools/imageconverter):

    a. Upload the image

    b. Color format: `CF_TRUE_COLOR`

    c. Output format: `C array`

    d. Tick `Dither images (can improve quality)` and `Output in big-endian format` checkboxes.

    e. Then press `Converter`


2. Copy the converted files into the repository (eg. `main/display_screen/weather_icons`)

3. Declare the image using `LV_IMG_DECLARE()`.
```    
    Eg. Open the `img.c` file, and at the bottom of the file.
        search for a variable declaration using `const lv_img_dsc_t`.
        In the header file, declar the image using `LV_IMG_DECLARE(<variable-name>)`.
```

# Troubleshooting
* Ensure using `lvgl` library version `/release/v8.0`