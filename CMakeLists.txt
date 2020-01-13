cmake_minimum_required(VERSION 2.8)

# VitaSDK defines
if( NOT DEFINED CMAKE_TOOLCHAIN_FILE )
  if( DEFINED ENV{VITASDK} )
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

# Project start
set(VITA_APPNAME Lolicopocalypse)
set(VITA_TITLEID "LOLI00001")
set(VITA_VERSION "01.00")

project(${VITA_APPNAME})
include("${VITASDK}/share/vita.cmake" REQUIRED)

# Flags and includes
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVITA -fno-exceptions")

include_directories(
)

link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Builds
add_executable(${VITA_APPNAME}
llcpcls.c
)


target_link_libraries(${VITA_APPNAME}
  SDL_image
  SDL_mixer
  SDL
  mikmod
  vorbisfile
  vorbis
  ogg
  jpeg
  png
  vita2d
  z
  m
  SceAppUtil_stub
  SceAudio_stub
  SceCommonDialog_stub
  SceCtrl_stub
  SceDisplay_stub
  SceGxm_stub
  SceSysmodule_stub
  SceTouch_stub
  SceHID_stub
)

# Create Vita artifacts
vita_create_self(eboot.bin ${VITA_APPNAME} SAFE)
vita_create_vpk(${VITA_APPNAME}.vpk ${VITA_TITLEID} eboot.bin
  VERSION ${VITA_VERSION}
  NAME ${VITA_APPNAME}
  FILE 0.ogg 0.ogg
  FILE 1.ogg 1.ogg
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml

)