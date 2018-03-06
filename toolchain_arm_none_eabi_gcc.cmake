include(CMakeForceCompiler)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
set(CMAKE_SHARED_LINKER_FLAGS "-mthumb -mthumb-interwork -mcpu=cortex-m4")
set(SHARED_C_CXX_FLAGS "-fmessage-length=0 -flto -ffunction-sections -fdata-sections -fno-strict-aliasing -Wall -Wextra")



find_program(ARM_CC arm-none-eabi-gcc
        ${TOOLCHAIN_DIR}/bin
        )
find_program(ARM_CXX arm-none-eabi-g++
        ${TOOLCHAIN_DIR}/bin
        )
find_program(ARM_OBJCOPY arm-none-eabi-objcopy
        ${TOOLCHAIN_DIR}/bin
        )
find_program(ARM_SIZE_TOOL arm-none-eabi-size
        ${TOOLCHAIN_DIR}/bin)

find_program(ARM_AR arm-none-eabi-as
        ${TOOLCHAIN_DIR}/bin)

find_program(ARM_AR arm-none-eabi-ar
        ${TOOLCHAIN_DIR}/bin)

find_program(ARM_RANLIB arm-none-eabi-ranlib
        ${TOOLCHAIN_DIR}/bin)

#set(CMAKE_C_COMPILER ${ARM_CC})
#set(CMAKE_CXX_COMPILER ${ARM_CXX})
CMAKE_FORCE_C_COMPILER(${ARM_CC} GNU)
CMAKE_FORCE_CXX_COMPILER(${ARM_CXX} GNU)
set(CMAKE_ASM          ${ARM_AS})
set(CMAKE_AR           ${ARM_AR})
set(CMAKE_RANLIB       ${ARM_RANLIB})

set(CMAKE_C_FLAGS "${SHARED_C_CXX_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${SHARED_C_CXX_FLAGS} -std=gnu++11 -pedantic -fno-exceptions -fno-rtti")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "cortex-m4")

    set(CMAKE_C_FLAGS
            "${CMAKE_C_FLAGS}"
            "${CMAKE_SHARED_LINKER_FLAGS}"
            )

else ()
    message(WARNING
            "Processor not recognised in toolchain file, "
            "compiler flags not configured."
            )
endif ()

# fix long strings (CMake appends semicolons)
string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "")

set(BUILD_SHARED_LIBS OFF)