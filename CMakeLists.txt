cmake_minimum_required(VERSION 3.26)

add_library(TonyCommon INTERFACE)
target_compile_features(TonyCommon INTERFACE cxx_std_17)

add_library(TonyWad STATIC
	"Source/Wad.cpp"
	"Source/Wad.h"
)
target_link_libraries(TonyWad PUBLIC TonyCommon)

add_library(TonyADPCM STATIC
	"Source/ADPCM.cpp"
	"Source/ADPCM.h"
)
target_link_libraries(TonyADPCM PUBLIC TonyCommon)

add_library(TonyPCM STATIC
	"Source/PCM.cpp"
	"Source/PCM.h"
)
target_link_libraries(TonyPCM PUBLIC TonyCommon)

add_executable(TonyStream
	"Source/Stream.cpp"
)
target_link_libraries(TonyStream PRIVATE TonyCommon TonyWad TonyADPCM TonyPCM)

add_executable(TonyMusic
	"Source/Music.cpp"
)
target_link_libraries(TonyMusic PRIVATE TonyCommon TonyWad TonyADPCM TonyPCM)
