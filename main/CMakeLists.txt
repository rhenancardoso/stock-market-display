file(GLOB_RECURSE SOURCES utils/*.c)

idf_component_register(SRCS main.c ${SOURCES} display_screens/display_screen.c
                    INCLUDE_DIRS ../components/lvgl utils)
