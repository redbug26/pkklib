add_library(pkklib INTERFACE)

target_sources(pkklib INTERFACE
    pkklib_sound.c
    pkklib_system.c
    pkklib_display.c
    pkklib_keyboard.c
    pkklib_gfx.c

    i2ckbd.c
    lcdspi.c
    gif.c
    hxcmod.c
    ymplay.c

    libstsound/Ymload.cpp
    libstsound/digidrum.cpp
    libstsound/YmFilters.cpp
    libstsound/LzhLib.cpp
    libstsound/YmMusic.cpp
    libstsound/Ym2149Ex.cpp
    libstsound/YmUserInterface.cpp
    )