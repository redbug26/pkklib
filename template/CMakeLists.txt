cmake_minimum_required(VERSION 3.13)

set(DEST "template")

# ---- BEGIN PKKLIB CONFIG ----

include(${CMAKE_CURRENT_LIST_DIR}/pkklib/res.cmake)

set(PICO_SDK_PATH "/home/dev/git/pico-sdk")

add_executable(${DEST}
	src/main.c
)

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

target_include_directories(${DEST} PRIVATE pkklib res)

project(${DEST} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_LIST_DIR}/pkklib/pkklib.cmake)

# ---- END PKKLIB CONFIG ----


