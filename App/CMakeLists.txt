cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${SYSROOT}/include)
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/Utils)
INCLUDE_DIRECTORIES( ./export )
INCLUDE_DIRECTORIES( ./src )

file(GLOB exe_SRC
    "./src/*.h"
    "./src/*.cpp"
)

link_directories(${SYSROOT}/lib)
add_executable (App ${exe_SRC})

target_link_libraries (App	Lib)
target_link_libraries (App libgflags.a)
target_link_libraries (App libglog.a)
target_link_libraries (App pthread)
target_link_libraries (App rt)


