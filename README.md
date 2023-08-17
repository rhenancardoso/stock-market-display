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

## 2.2 Initial development setup

1. After cloning the project to a local directory, add EDP-IDF tool to the project folder
```
. $HOME/esp/esp-idf/export.sh
```

2. Make project files using MAKE
```
make all
```

## 2.3 ESP Partition update
During development increasing the partition app size was required due to size overflow.
See more on [ESP-IDF website](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html)

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
__Note:__ When running `make clean`, it is required to run `make` then `make part-table` again.

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
# 2.5 Google Sheets API - Stock Market spreadsheet
An easy method for getting stock-market data is to use a google spreadsheet using google-finance formulas
to get live stock market data. 

## 2.5.1 Develpoing an WebApp in Google Sheets
for more: https://developers.google.com/apps-script/guides/web

# Troubleshooting
* Ensure using `lvgl` library version `/release/v8.0`

* The board uses USB as the JTAG upload port. When printing serial port information on USB_CDC_ON_BOOT configuration needs to be turned on. If the port cannot be found when uploading the program or the USB has been used for other functions, the port does not appear. Please enter the upload mode manually.

    - Connect the board via the USB cable
    - Press and hold the BOOT button , While still pressing the BOOT button, press RST
    - Release the RST
    - Release the BOOT button
    - Upload sketch