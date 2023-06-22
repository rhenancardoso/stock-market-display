file(GLOB_RECURSE SOURCES utils/*.c)
file(GLOB_RECURSE DISPLAY_SCRS display_screen/weather_icons/*.c)

idf_component_register(SRCS main.c ${SOURCES} display_screen/display_screen.c ${DISPLAY_SCRS}
                    INCLUDE_DIRS ../components/lvgl)
