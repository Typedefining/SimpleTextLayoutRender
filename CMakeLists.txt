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
	TextParagraph.cpp
	TextParagraph.h

	FreeTypeFontMetrics.cpp
	FreeTypeFontMetrics.h

	main.cpp
)

# include路径前缀
include_directories(${PROJECT_SOURCE_DIR})

# 收集依赖的qt库
set(QT_LIB
	Core
	Widgets
	Gui
)

find_package(Qt5 COMPONENTS REQUIRED
${QT_LIB}
)

find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

# 组合连接时的名字
list(TRANSFORM QT_LIB PREPEND "Qt5::" OUTPUT_VARIABLE QT_LIB_LINK)

foreach(item ${QT_LIB_LINK})
	get_target_property(LibLocation ${item} LOCATION)
	get_filename_component(LibDir ${LibLocation} DIRECTORY)
	list(APPEND LIB_LOCATION ${LibLocation})
endforeach()

message(${LibDir})
message(${LIB_LOCATION})

message(FREETYPE_LIBRARIES ${FREETYPE_LIBRARIES})
message(FREETYPE_INCLUDE_DIRS ${FREETYPE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIB_LINK} ${FREETYPE_LIBRARIES})

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
	WIN32_EXECUTABLE ON
)