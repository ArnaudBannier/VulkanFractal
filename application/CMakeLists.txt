
set(NAME application)
add_executable(${NAME})

file(GLOB_RECURSE
    PROJECT_SOURCE_FILES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.c"
)
file(GLOB_RECURSE
    PROJECT_HEADER_FILES CONFIGURE_DEPENDS
    "src/*.hpp" "src/*.h"
)
file(GLOB_RECURSE
    PROJECT_SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_SOURCE_DIR}/shaders/*.comp"
    "${CMAKE_SOURCE_DIR}/shaders/*.geom"
    "${CMAKE_SOURCE_DIR}/shaders/*.tesc"
    "${CMAKE_SOURCE_DIR}/shaders/*.tese"
)

target_sources(${NAME} PRIVATE
    ${PROJECT_SOURCE_FILES}
    ${PROJECT_HEADER_FILES}
    ${PROJECT_SHADER_FILES}
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
source_group("shaders" FILES ${PROJECT_SHADER_FILES})

#-------------------------------------------------------------------------------
# Third party libraries

if(TARGET SDL2::SDL2main)
    target_link_libraries(${NAME} PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(${NAME} PRIVATE
    SDL2::SDL2
    #SDL2_image::SDL2_image
    #SDL2_ttf::SDL2_ttf
    #SDL2_mixer::SDL2_mixer
    ${Vulkan_LIBRARIES}
    engine
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
# Copy DLL and files

function(copy_dll PATH_TO_DLL)
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        message(STATUS "[INFO] Copy: " ${PATH_TO_DLL} " to " ${CONFIG})
        file(
            COPY ${PATH_TO_DLL}
            DESTINATION "${CMAKE_SOURCE_DIR}/_bin/${CONFIG}"
        )
    endforeach(CONFIG)
endfunction()

if (MSVC)
    copy_dll("${THIRD_PARTY_SDL2_DIR}/lib/x64/SDL2.dll")
    #copy_dll("${THIRD_PARTY_SDL2_IMAGE_DIR}/lib/x64/SDL2_image.dll")
    #copy_dll("${THIRD_PARTY_SDL2_TTF_DIR}/lib/x64/SDL2_ttf.dll")
    #copy_dll("${THIRD_PARTY_SDL2_MIXER_DIR}/lib/x64/SDL2_mixer.dll")
endif()

if(VS_DEPLOY_CONFIG)
    file(
        COPY "${CMAKE_SOURCE_DIR}/assets"
        DESTINATION "${CMAKE_SOURCE_DIR}/_bin"
    )
endif()

# # Copy SDL2.dll in the bin folder as post-build event
# add_custom_command(
#     TARGET ${CMAKE_PROJECT_NAME} POST_BUILD 
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "${CMAKE_CURRENT_SOURCE_DIR}/${THIRD_PARTY_SDL2_DIR}/lib/x64/SDL2.dll"
#         $<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>
# )

#-------------------------------------------------------------------------------
# Build shader target

find_program(GLSL_VALIDATOR glslangValidator HINTS 
    ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
    $ENV{VULKAN_SDK}/Bin/
    $ENV{VULKAN_SDK}/Bin32/
)

message(STATUS "[INFO] GLSL Validator: " ${GLSL_VALIDATOR})

foreach(GLSL ${PROJECT_SHADER_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    shaders_target 
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(${NAME} shaders_target)

set_target_properties(shaders_target PROPERTIES FOLDER cmake_targets)

add_custom_command(TARGET ${NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${NAME}>/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "$<TARGET_FILE_DIR:${NAME}>/shaders"
)