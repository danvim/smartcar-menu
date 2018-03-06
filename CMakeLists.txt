cmake_minimum_required(VERSION 3.8)
project(inno14-d-2017-inno C CXX)

set(SCCC_BUILD DEBUG)
set(SCCC_CONFIG 2017_INNO)
set(LIBSCCC_INC_PATH "D:/projects/cpp/smartcar/libsccc/inc")
set(LIBSCCC_BIN_PATH "D:/projects/cpp/smartcar/libsccc/bin")


set(INC_PATH inc)
set(SRC_PATH src)
set(BUILD_PATH build)
if(NOT LIBSCCC_INC_PATH)
    set(LIBSCCC_INC_PATH lib/sccc/inc)
endif()
if(NOT LIBSCCC_BIN_PATH)
    set(LIBSCCC_BIN_PATH lib/sccc/lib)
endif()
if(NOT LIBSCCC_BIN)
    set(LIBSCCC_BIN sccc)
endif()
set(OUT_EXE inno14)
set(OUT_EXE_SUFFIX .elf)
set(OUT_EXE_PATH bin)
set(OUT_OBJ_PATH obj)

# Additional include dirs and libs. libsccc will be added automatically
set(ALL_INC_PATHS ${INC_PATH} ${SRC_PATH})
set(ALL_LIB_PATHS)
set(ALL_LIBS)






message("CMake version = ${CMAKE_VERSION}")
message("User include paths = ${ALL_INC_PATHS}")
message("User lib paths = ${ALL_LIB_PATHS}")
message("User libs = ${ALL_LIBS}")

if("${SCCC_BUILD}" STREQUAL "DEBUG")
    set(BIN_SUFFIX ${BIN_SUFFIX}-d)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDBUG=1")
    message("Build = DEBUG")
elseif("${SCCC_BUILD}" STREQUAL "RELEASE")
    set(BIN_SUFFIX ${BIN_SUFFIX}-d)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRELEASE=1 -DNDEBUG")
    message("Build = RELEASE")
else()
    message(WARNING "Unknown build type, defaulting to DEBUG (set SCCC_BUILD)")
    set(BIN_SUFFIX ${BIN_SUFFIX}-d)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDBUG=1")
    message("Build = DEBUG")
endif()

include(CMakeConfig.cmake)

if("${SCCC_MCU}" STREQUAL "MK60DZ10")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMK60DZ10=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -msoft-float -mfloat-abi=soft")
    set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/${BUILD_PATH}/d10.ld")
    message("MCU sub-family = MK60DZ10")
elseif("${SCCC_MCU}" STREQUAL "MK60D10")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMK60D10=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -msoft-float -mfloat-abi=soft")
    set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/${BUILD_PATH}/d10.ld")
    message("MCU sub-family = MK60D10")
elseif("${SCCC_MCU}" STREQUAL "MK60F15")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMK60F15=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
    set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/${BUILD_PATH}/f15.ld")
    message("MCU sub-family = MK60F15")
else()
    message(FATAL_ERROR "Missing/Unknown MCU identifier '${SCCC_MCU}' (set SCCC_MCU)")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_LINK_DEF_FILE_FLAG} -T${LINKER_SCRIPT}")

include_directories(${ALL_INC_PATHS} ${LIBSCCC_INC_PATH})
include_directories(${ALL_LIB_PATHS} ${LIBSCCC_BIN_PATH})

file(GLOB_RECURSE USER_SOURCES "src/*.cpp")
file(GLOB_RECURSE USER_INCLUDES "inc/*.h")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -specs=nano.specs -u _printf_float")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/bin/${PROJECT_NAME}.map")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${LIBSCCC_BIN_PATH}")

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${USER_INCLUDES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf)
set_target_properties(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE CXX)

message("CMAKE_EXE_LINKER_FLAGS = ${CMAKE_EXE_LINKER_FLAGS}")
message("CMAKE_C_FLAGS = ${CMAKE_C_FLAGS}")
message("CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")



# add_custom_command(TARGET ${PROJECT_NAME}.elf)