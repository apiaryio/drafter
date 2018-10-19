set(CMAKE_SYSTEM_NAME Emscripten)

if(NOT EMSCRIPTEN_PREFIX)
    if(DEFINED ENV{EMSCRIPTEN})
        set(EMSCRIPTEN_PREFIX $ENV{EMSCRIPTEN})
    else()
        set(EMSCRIPTEN_PREFIX "/usr/lib/emscripten")
    endif()
endif()

set(EMSCRIPTEN_GENERATE_BITCODE_STATIC_LIBRARIES TRUE)

include(${EMSCRIPTEN_PREFIX}/cmake/Modules/Platform/Emscripten.cmake)
