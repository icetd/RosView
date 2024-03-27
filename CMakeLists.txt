cmake_minimum_required(VERSION 3.10)
project(RosView)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

add_definitions(-w)

set (CMKAE_CXX_STANDARD 14)
set (CMAKE_BUILD_TYPE Release)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/src") 
set (INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

set (ROS_LIB            "${CMAKE_CURRENT_LIST_DIR}/thirdparty/ros_lib")
set (GLFW_INC			"${CMAKE_CURRENT_LIST_DIR}/thirdparty/glfw/include")
set (GLAD_SRC			"${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad/src")
set (GLAD_INC			"${CMAKE_CURRENT_LIST_DIR}/thirdparty/glad/include")
set (STB_IMAGE_INC		"${CMAKE_CURRENT_LIST_DIR}/thirdparty/stb_image")
set (GLM_INC			"${CMAKE_CURRENT_LIST_DIR}/thirdparty/glm")
set (IMGUI_SRC			"${CMAKE_CURRENT_LIST_DIR}/thirdparty/imgui")
set (IMPLOT_SRC			"${CMAKE_CURRENT_LIST_DIR}/thirdparty/implot")
set (FFMPEG_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/thirdparty/FFmpeg/include")
set (FFMPEG_LIB_DIR     "${CMAKE_CURRENT_LIST_DIR}/thirdparty/FFmpeg/lib")
set (SQLITE3_SRC		"${CMAKE_CURRENT_LIST_DIR}/thirdparty/sqlite3")
set (SQLITE3_INC		"${CMAKE_CURRENT_LIST_DIR}/thirdparty/sqlite3")
set (VLD_INCLUDE_DIR    "${CMAKE_SOURCE_DIR}/thirdparty/VLD/include")
set (VLD_LIB_DIR        "${CMAKE_SOURCE_DIR}/thirdparty/VLD/lib")

link_directories(
		${FFMPEG_LIB_DIR}
		${VLD_LIB_DIR}
)

include_directories(
	${INCLUDE_DIR}
	${INCLUDE_DIR}/Commuication
	${INCLUDE_DIR}/Core
	${INCLUDE_DIR}/Video
	${INCLUDE_DIR}/Graphics
	${INCLUDE_DIR}/Plan
	${INCLUDE_DIR}/Ui
	${INCLUDE_DIR}/Utils
	${INCLUDE_DIR}/Ros
	${GLFW_INC}
	${GLAD_INC}
	${STB_IMAGE_INC}
	${GLM_INC}
	${IMGUI_SRC}
	${IMPLOT_SRC}
	${FFMPEG_INCLUDE_DIR}
    ${ROS_LIB}
	${SQLITE3_INC}
	${VLD_INCLUDE_DIR}
) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE SRC_FILES
    "${SOURCE_DIR}/*.c*"
    "${SOURCE_DIR}/Commuication/*.c*"
    "${SOURCE_DIR}/Core/*.c*"
    "${INCLUDE_DIR}/Graphics/*.c*"
    "${INCLUDE_DIR}/Plan/*.c*"
    "${SOURCE_DIR}/Ui/*.c*"
    "${SOURCE_DIR}/Utils/*.c*"
    "${SOURCE_DIR}/Video/*.c*"
    "${SOURCE_DIR}/Ros/*.c*"
	"${GLAD_SRC}/*c*"
	"${IMGUI_SRC}/*.c*"
	"${IMPLOT_SRC}/*.c*"
    "${ROS_LIB}/*.c*"
	"${SQLITE3_SRC}/*c*"
)

file (GLOB_RECURSE INC_FILES
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/Commuication/*.h"
    "${INCLUDE_DIR}/Core/*.h"
    "${INCLUDE_DIR}/Graphics/*.h"
    "${INCLUDE_DIR}/Plan/*.h"
    "${INCLUDE_DIR}/Ui/*.h"
    "${INCLUDE_DIR}/Utils/*.h"
    "${INCLUDE_DIR}/Video/*.h"
    "${INCLUDE_DIR}/Ros/*.h"
	"${IMGUI_SRC}/*.h"
	"${IMPLOT_SRC}/*.h"
	"${GLAD_INC}/*h"
    "${ROS_LIB}/*.h"
	"{SQLITE3_INC}/*.h"
)

file (GLOB_RECURSE RES_FILES
    "${CMAKE_SOURCE_DIR}/res/shaders/*.shader"
    "${CMAKE_SOURCE_DIR}/res/*.rc"
)


source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${SRC_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${INC_FILES})
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${RES_FILES})

list(APPEND EXTRA_LIBS
	ws2_32 opengl32 glfw User32 Gdi32 Shell32 
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES} ${RES_FILES})

# Add the target includes for MY_PROJECT 
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})


#===================== LINKING LIBRARIES =======================#
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS} swscale avformat avdevice  avcodec avutil)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/output)

file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
file(COPY ${CMAKE_SOURCE_DIR}/configs DESTINATION ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
file (GLOB FFMPEG_COPY_DLL "${CMAKE_SOURCE_DIR}/thirdparty/FFmpeg/dll/*")
file(COPY ${FFMPEG_COPY_DLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})