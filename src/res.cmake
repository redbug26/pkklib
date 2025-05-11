# compile the YM

# Dossier source et destination
set(YM_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../res/music)
set(YM_DST_DIR ${CMAKE_CURRENT_LIST_DIR}/../res)

# Cherche tous les fichiers *.ym
file(GLOB YM_FILES RELATIVE ${YM_SRC_DIR} "${YM_SRC_DIR}/*.ym")

# Liste des headers générés
set(GENERATED_HEADERS)

foreach(YM_FILE ${YM_FILES})
    get_filename_component(NAME_WE ${YM_FILE} NAME_WE)
    set(OUT_HEADER "${YM_DST_DIR}/ym_${NAME_WE}.h")
    set(IN_YM "${YM_SRC_DIR}/${YM_FILE}")

    add_custom_command(
        OUTPUT ${OUT_HEADER}
        COMMAND bin2c ${IN_YM} ${OUT_HEADER} ym_${NAME_WE}
        DEPENDS ${IN_YM}
        COMMENT "Génération de ${OUT_HEADER} depuis ${IN_YM}"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${OUT_HEADER})
endforeach()

# Cible fictive pour déclencher la génération
add_custom_target(generate_ym_headers ALL DEPENDS ${GENERATED_HEADERS})

# compile the GIF

# Dossier source et destination
set(GIF_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../res/gfx)
set(GIF_DST_DIR ${CMAKE_CURRENT_LIST_DIR}/../res)

# Cherche tous les fichiers *.gif
file(GLOB GIF_FILES RELATIVE ${GIF_SRC_DIR} "${GIF_SRC_DIR}/*.gif")

# Liste des headers générés
set(GENERATED_HEADERS)

foreach(GIF_FILE ${GIF_FILES})
    get_filename_component(NAME_WE ${GIF_FILE} NAME_WE)
    set(OUT_HEADER "${GIF_DST_DIR}/G_${NAME_WE}_scr.h")
    set(IN_GIF "${GIF_SRC_DIR}/${GIF_FILE}")

    add_custom_command(
        OUTPUT ${OUT_HEADER}
        COMMAND bin2c ${IN_GIF} ${OUT_HEADER} G_${NAME_WE}_scr
        DEPENDS ${IN_GIF}
        COMMENT "Génération de ${OUT_HEADER} depuis ${IN_GIF}"
        VERBATIM
    )

    list(APPEND GENERATED_HEADERS ${OUT_HEADER})
endforeach()

# Cible fictive pour déclencher la génération
add_custom_target(generate_gif_headers ALL DEPENDS ${GENERATED_HEADERS})


### 