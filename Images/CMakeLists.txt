#Minimum CMake version and policy settings
cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

#Project Name
project(Images)

#Add executable
add_executable(Images)

#Add include dirs
if(WIN32)
    target_include_directories(
        Images
        PUBLIC
        ../deps/windows/i386/SDL2/include
        ../deps/windows/i386/SDL2/include/SDL2
        ../deps/windows/i386/SDL2_image/include
    )
endif(WIN32)

if(UNIX)
    target_include_directories(
        Images
        PUBLIC
        ../deps/linux/amd64/SDL2/include
        ../deps/linux/amd64/SDL2/include/SDL2
        ../deps/linux/amd64/SDL2_image/include
    )
endif(UNIX)

#Add link dirs
if(WIN32)
    target_link_directories(
        Images
        PUBLIC
        ../deps/windows/i386/SDL2/lib
        ../deps/windows/i386/SDL2_image/bin
        ../deps/windows/i386/SDL2_image/lib
    )
endif(WIN32)

if(UNIX)
    target_link_directories(
        Images
        PUBLIC
        ../deps/linux/amd64/SDL2/lib
        ../deps/linux/amd64/SDL2_image/bin
        ../deps/linux/amd64/SDL2_image/lib
    )
endif(UNIX)

#Add source code
target_sources(
    Images
    PUBLIC
    src/main.c
)

#Libraries to link against
set(LIBS
    SDL2main
    SDL2
    SDL2_image
)

if(WIN32)
    if(MINGW)
        set(LIBS
            mingw32
            ${LIBS}
        )
    endif(MINGW)
    
    target_link_libraries(
        Images
        ${LIBS}
    )
endif(WIN32)

if(UNIX)
    target_link_libraries(
        Images
        ${LIBS}
    )
endif(UNIX)

#Add compile flags
if(MSVC10)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /TP")
endif(MSVC10)

if(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=.")
endif(UNIX)

#Copy deps
if(WIN32)
    file(
        GLOB DEPS
        ../deps/windows/i386/SDL2/bin/*
        ../deps/windows/i386/SDL2_image/bin/*
    )
    file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif(WIN32)

if(UNIX)
    file(
        GLOB DEPS
        ../deps/linux/amd64/SDL2/bin/*.txt
        ../deps/linux/amd64/SDL2/lib/*.so*
        ../deps/linux/amd64/SDL2_image/bin/*
        ../deps/linux/amd64/SDL2_image/lib/*.so*
    )
    file(COPY ${DEPS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif(UNIX)

#Copy data files
file(COPY ../data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#Install
install(TARGETS Images DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${DEPS} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ../data DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)