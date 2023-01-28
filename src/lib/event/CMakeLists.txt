add_library(event event.c)

target_include_directories(event PUBLIC include)

add_subdirectory(test)
