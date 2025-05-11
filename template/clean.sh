#!/bin/bash

# get dest name from CMakeLists.txt
DEST_NAME=$(grep 'set(DEST' CMakeLists.txt | sed -E 's/.*set\(DEST[[:space:]]+"?([^" )]+)"?.*/\1/')
if [ -z "$DEST_NAME" ]; then
    echo -e "\e[31mError:\e[0m \e[1mCould not find project name in CMakeLists.txt\e[0m"
    exit 1
fi

echo -e "\e[34mCleaning up the project directory for \e[1m$DEST_NAME\e[0m\e[34m...\e[0m"

rm -rf _deps build CMakeFiles generated pico-sdk picotool pioasm pioasm-install CMakeCache.txt cmake_install.cmake pico_flash_region.ld Makefile out/* .DS_Store res/*.h

# Display a message with formatting and color
echo -e "\e[32mYou can now run the build script again:\e[0m"
echo -e "\e[33mcmake . ; make\e[0m"
