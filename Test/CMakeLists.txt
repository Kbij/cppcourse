cmake_minimum_required(VERSION 2.8)


INCLUDE_DIRECTORIES(${SYSROOT}/include)

# Is a test, .src directory of lib needed
INCLUDE_DIRECTORIES( ../Lib/src )

INCLUDE_DIRECTORIES( ./src )
file(GLOB exe_SRC
    "./src/*.h"
    "./src/*.cpp"
)
link_directories(${SYSROOT}/lib)
add_executable (LibTest ${exe_SRC})

target_link_libraries (LibTest Lib)

target_link_libraries (LibTest libglog.a)
target_link_libraries (LibTest libgtest.a)
target_link_libraries (LibTest libgflags.a)

target_link_libraries (LibTest pthread)
target_link_libraries (LibTest rt)






