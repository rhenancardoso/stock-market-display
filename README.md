# 1. T-DISPLAY-S3
Repository created for setting LilyGo T-Dipslay-S3 with the LVGL library

## 1.1 Device used
1. LilyGo T-Display-S3 [https://www.lilygo.cc/products/t-display-s3]

# 2. CONTRIBUTING
## 2.1 Environment setup
Tool development in OS system.

### 2.1.1 Requirements
1. ESP-IDF tool (version 5.2): https://docs.espressif.com/projects/esp-idf/en/release-v5.0/esp32s3/get-started/index.html
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

# Troubleshooting
* Ensure using `lvgl` library version `/release/v8.0`