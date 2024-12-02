cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(NAME vulkan_fractal)
project(${NAME} VERSION 1.0)

option(VS_DEBUG_RELEASE "Generate only DEBUG and RELEASE configuration on VS" ON)
option(VS_DEPLOY_CONFIG "Generate deploy configuration on VS and copy assets" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/_bin/")

message(STATUS "[INFO] Current directory: " ${CMAKE_SOURCE_DIR})

#add_executable(${NAME})


if(MSVC)
    include(ConfigureVS.cmake)
    # set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
endif()


#-------------------------------------------------------------------------------
# Third party libraries

set(THIRD_PARTY_SDL2_DIR       "${CMAKE_SOURCE_DIR}/third_party/SDL2-2.30.6")
#set(THIRD_PARTY_SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL2_image-2.8.2")
#set(THIRD_PARTY_SDL2_TTF_DIR   "${CMAKE_SOURCE_DIR}/third_party/SDL2_ttf-2.22.0")
#set(THIRD_PARTY_SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/third_party/SDL2_mixer-2.8.0")
set(THIRD_PARTY_IM_GUI_DIR     "${CMAKE_SOURCE_DIR}/third_party/imgui-1.91.0")
set(THIRD_PARTY_GLM_DIR        "${CMAKE_SOURCE_DIR}/third_party/glm-1.0.1")
set(THIRD_PARTY_STB_IMAGE_DIR  "${CMAKE_SOURCE_DIR}/third_party/stb_image-2.30")

list(APPEND CMAKE_PREFIX_PATH
    "${THIRD_PARTY_SDL2_DIR}/cmake"
    #"${THIRD_PARTY_SDL2_IMAGE_DIR}/cmake"
    #"${THIRD_PARTY_SDL2_TTF_DIR}/cmake"
    #"${THIRD_PARTY_SDL2_MIXER_DIR}/cmake"
)

find_package(Vulkan REQUIRED)
IF (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
ENDIF()

find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)
#find_package(SDL2_ttf REQUIRED)
#find_package(SDL2_mixer REQUIRED)

add_subdirectory(engine)
add_subdirectory(application)

