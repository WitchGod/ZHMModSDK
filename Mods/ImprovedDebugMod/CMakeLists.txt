cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE SRC_FILES
	CONFIGURE_DEPENDS
	Src/*.cpp
	Src/*.c
	Src/*.hpp
	Src/*.h
)

add_library(ImprovedDebugMod SHARED
	${SRC_FILES}
)

target_include_directories(ImprovedDebugMod PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Src
)

find_package(lz4 CONFIG REQUIRED)
find_package(7zip CONFIG REQUIRED)

target_link_libraries(ImprovedDebugMod PRIVATE
	ZHMModSDK
	winhttp
	imguizmo
	lz4::lz4
	7zip::7zip
)

install(TARGETS ImprovedDebugMod
	RUNTIME DESTINATION bin/mods
)