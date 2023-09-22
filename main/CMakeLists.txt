file(GLOB_RECURSE SOURCES utils/*.c)
file(GLOB_RECURSE ICONS_SCRS display_screen/weather_icons/*.c)
file(GLOB_RECURSE SCREENS_SCRS display_screen/*.c)
file(GLOB_RECURSE SHARED_SCREENS_SCRS display_screen/shared/*.c)

idf_component_register(SRCS main.c ${SOURCES} ${SCREENS_SCRS} ${ICONS_SCRS} ${SHARED_SCREENS_SCRS}
                    INCLUDE_DIRS ../components/lvgl)
