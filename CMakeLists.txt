cmake_minimum_required(VERSION 3.10)
project(RosView)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)

set (CMKAE_CXX_STANDARD 14)
set (CMAKE_BUILD_TYPE Relesae)
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
set (FILEDIALOGS_SRC	"${CMAKE_CURRENT_LIST_DIR}/thirdparty/tinyfiledialogs")
set (FFMPEG_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/thirdparty/FFmpeg/include")
set (FFMPEG_LIB_DIR     "${CMAKE_CURRENT_LIST_DIR}/thirdparty/FFmpeg/lib")
set (SQLITE3_SRC		"${CMAKE_CURRENT_LIST_DIR}/thirdparty/sqlite3")
set (SQLITE3_INC		"${CMAKE_CURRENT_LIST_DIR}/thirdparty/sqlite3")

link_directories(
		${FFMPEG_LIB_DIR}
)

include_directories(
	${INCLUDE_DIR}
	${INCLUDE_DIR}/test
	${INCLUDE_DIR}/Core
	${INCLUDE_DIR}/Video
	${INCLUDE_DIR}/Graphics
	${INCLUDE_DIR}/Plan
	${INCLUDE_DIR}/Ui
	${INCLUDE_DIR}/Node
	${GLFW_INC}
	${GLAD_INC}
	${STB_IMAGE_INC}
	${GLM_INC}
	${IMGUI_SRC}
	${FILEDIALOGS_SRC}
	${FILEDIALOGS_SRC}/backends
	${FFMPEG_INCLUDE_DIR}
    ${ROS_LIB}
	${SQLITE3_INC}
) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE SRC_FILES
    "${SOURCE_DIR}/*.c*"
    "${SOURCE_DIR}/Core/*.c*"
    "${INCLUDE_DIR}/Graphics/*.c*"
    "${INCLUDE_DIR}/Plan/*.c*"
    "${SOURCE_DIR}/Ui/*.c*"
    "${SOURCE_DIR}/Video/*.c*"
    "${SOURCE_DIR}/Node/*.c*"
	"${GLAD_SRC}/*c*"
	"${IMGUI_SRC}/*.c*"
	"${FILEDIALOGS_SRC}/*c*"
	"${FILEDIALOGS_SRC}/backends/*c*"
    "${ROS_LIB}/*.c*"
	"${SQLITE3_SRC}/*c*"
)

file (GLOB_RECURSE INC_FILES
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/test/*.h"
    "${INCLUDE_DIR}/Core/*.h"
    "${INCLUDE_DIR}/Graphics/*.h"
    "${INCLUDE_DIR}/Plan/*.h"
    "${INCLUDE_DIR}/Ui/*.h"
    "${INCLUDE_DIR}/Video/*.h"
    "${INCLUDE_DIR}/Node/*.h"
	"${IMGUI_SRC}/*.h"
	"${GLAD_INC}/*h"
	"${FILEDIALOGS_SRC}/*h"
	"${FILEDIALOGS_SRC}/backends/*h"
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
	opengl32 glfw User32 Gdi32 Shell32
)

add_executable(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES} ${RES_FILES})

# Add the target includes for MY_PROJECT 
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR})


#===================== LINKING LIBRARIES =======================#
target_link_libraries(${PROJECT_NAME} ${EXTRA_LIBS} swscale avformat avdevice  avcodec avutil )

file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})