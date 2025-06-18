cmake_minimum_required(VERSION 3.15)

project(LayoutEnginer)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_PREFIX_PATH "C:/Users/Typedef/vcpkg/installed/x64-windows")

# 整理IDE中的文件分类
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMakeTargets")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE HEADERS "*.h")

source_group("src" FILES ${SOURCES})
source_group("header" FILES ${HEADERS})

if(MSVC)
    add_compile_options("/utf-8")
    add_definitions(-DUNICODE -D_UNICODE)
    add_compile_options(/MP)
endif ()

set(SRC
	FreeTypeFontMetrics.h
	FreeTypeFontMetrics.cpp

	TextRun.h
	TextParagraph.h

	Font.h
	Font.cpp

	GlyphInfo.h

	LayoutEngine.h
	LayoutEngine.cpp

	Color.h
	PaintDevice.h
	PaintEngine.h
	stb_image_write.h

	main.cpp
)

# include路径前缀
include_directories(${PROJECT_SOURCE_DIR})

find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

message(FREETYPE_LIBRARIES ${FREETYPE_LIBRARIES})
message(FREETYPE_INCLUDE_DIRS ${FREETYPE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE ${FREETYPE_LIBRARIES})