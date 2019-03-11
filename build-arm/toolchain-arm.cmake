
#set( CMAKE_VERBOSE_MAKEFILE on )
set( HCB_PLATFORM "arm")

# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_C_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-g++)
SET(CMAKE_ASM_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-gcc)

#Gcc 5.5 Asan settings
#SET(CMAKE_C_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-linaro-5.5.0-2017.10-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
#SET(CMAKE_CXX_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-linaro-5.5.0-2017.10-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)
#SET(CMAKE_ASM_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/gcc-linaro-5.5.0-2017.10-i686_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)

#This is needed for compatibility with gcc 4.9
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address" )

SET(CMAKE_SYSTEM_PROCESSOR arm)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /home/qbus/hcb/libraries/sysroot-arm)

# search for programs in the build host directories
#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
# for libraries and headers in the target directories
#SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)

#SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
#this fixes the problem with boost (find_package(boost)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
