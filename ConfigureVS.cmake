if(VS_DEBUG_RELEASE)
    message(STATUS "[INFO] Configurations: DEBUG; RELEASE")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

if(VS_DEPLOY_CONFIG)
    message(STATUS "[INFO] Add DEPLOY configuration")
    list(APPEND CMAKE_CONFIGURATION_TYPES "Deploy")

    set(CMAKE_C_FLAGS_DEPLOY "/MD /O2 /Ob2 /DNDEBUG"
        CACHE STRING "Flags used by the C compiler during DEPLOY builds." FORCE
    )
    set(CMAKE_CXX_FLAGS_DEPLOY "/MD /O2 /Ob2 /DNDEBUG"
        CACHE STRING "Flags used by the CXX compiler during DEPLOY builds." FORCE
    )
    set(CMAKE_EXE_LINKER_FLAGS_DEPLOY "/SUBSYSTEM:WINDOWS"
        CACHE STRING "Flags used by the linker during DEPLOY builds." FORCE
    )
endif()
