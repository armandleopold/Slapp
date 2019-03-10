cmake_minimum_required(VERSION 3.7)

project(slappclion)

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer>=2.0.0)

file(GLOB all_SRCS "${PROJECT_SOURCE_DIR}/sources/*.c")
file(GLOB ARTWORKS "${PROJECT_SOURCE_DIR}/artwork/*")
file(GLOB MUSICS "${PROJECT_SOURCE_DIR}/musique/*")
file(GLOB FONTS "${PROJECT_SOURCE_DIR}/fonts/*")
file(GLOB TILES "${PROJECT_SOURCE_DIR}/tiles/*")
file(GLOB SAVE "${PROJECT_SOURCE_DIR}/save/*")
file(GLOB MAP "${PROJECT_SOURCE_DIR}/map/*")

file(COPY ${ARTWORKS} DESTINATION artwork)
file(COPY ${MUSICS} DESTINATION musique)
file(COPY ${FONTS} DESTINATION fonts)
file(COPY ${TILES} DESTINATION tiles)
file(COPY ${SAVE} DESTINATION save)
file(COPY ${MAP} DESTINATION map)

add_executable(slappclion main.c ${all_SRCS} ${artworks})
target_include_directories(slappclion PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/sources)
target_link_libraries(slappclion ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2MIXER_LIBRARIES})
