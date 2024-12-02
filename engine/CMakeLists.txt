
set(NAME engine)
add_library(${NAME} STATIC)

file(GLOB_RECURSE
    PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.c"
)
file(GLOB_RECURSE
    PROJECT_HEADER_FILES CONFIGURE_DEPENDS
    "src/*.hpp" "src/*.h"
)

target_sources(${NAME} PRIVATE
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
)

target_compile_features(${NAME} PUBLIC cxx_std_17)
target_compile_definitions(${NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_definitions(${NAME} PUBLIC _SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING)

set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake_targets")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "sources"
    FILES ${PROJECT_SOURCE_FILES} ${PROJECT_HEADER_FILES}
)

target_link_libraries(${NAME} PRIVATE
    SDL2::SDL2
    ${Vulkan_LIBRARIES}
)

#-------------------------------------------------------------------------------
# Other include directories

target_include_directories(
    ${NAME} PUBLIC
    "src"
    ${Vulkan_INCLUDE_DIRS}
    "${THIRD_PARTY_SDL2_DIR}/include"
    #"${THIRD_PARTY_SDL2_IMAGE_DIR}/include"
    #"${THIRD_PARTY_SDL2_TTF_DIR}/include"
    #"${THIRD_PARTY_SDL2_MIXER_DIR}/include"
    "${THIRD_PARTY_IM_GUI_DIR}"
    "${THIRD_PARTY_IM_GUI_DIR}/backends"
    ${THIRD_PARTY_STB_IMAGE_DIR}
    ${THIRD_PARTY_GLM_DIR}
)

#-------------------------------------------------------------------------------
# ImGui files

file(GLOB
    IM_GUI_SOURCE_FILES CONFIGURE_DEPENDS
    "${THIRD_PARTY_IM_GUI_DIR}/*.h"
    "${THIRD_PARTY_IM_GUI_DIR}/*.cpp"
)


set(IM_GUI_BACKENDS_FILES
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_sdl2.h"
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_sdl2.cpp"
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_vulkan.h"
    "${THIRD_PARTY_IM_GUI_DIR}/backends/imgui_impl_vulkan.cpp"
    # "${THIRD_PARTY_IM_GUI_DIR}/examples/example_sdl2_vulkan/main.cpp"
)

target_sources(${NAME} PRIVATE
    ${IM_GUI_SOURCE_FILES} ${IM_GUI_BACKENDS_FILES}
)

source_group("third_party/im_gui" FILES ${IM_GUI_SOURCE_FILES})
source_group("third_party/im_gui/backends" FILES ${IM_GUI_BACKENDS_FILES})
