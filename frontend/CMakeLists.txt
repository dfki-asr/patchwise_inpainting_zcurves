cmake_minimum_required(VERSION 3.19)

project(Inpainting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# set include directories
include_directories("${INPAINTING_DEPENDENCIES_ROOT}"
                    "${FREEIMAGEPLUS_INCLUDE_PATH}"
                    "${MKL_INCLUDE_DIR}"
                    "${DAAL_INCLUDE_DIR}"
                    "${RAPID_XML_INCLUDE_DIR}"
                    "${LIBMEMORYMANAGER_INCLUDE_DIR}"
                    "${LIBTIMER_INCLUDE_DIR}"
                    "${LIBMMV_INCLUDE_DIR}"
                    "${LIBCFG_INCLUDE_DIR}"
                    "${LIBINPAINTING_INCLUDE_DIR}")

add_executable(${PROJECT_NAME} ${HOST_SOURCES})

target_link_libraries(${PROJECT_NAME} 
                intel_onedal_core
                intel_mkl_lp64
                intel_mkl_core
                intel_mkl_seq
                libTimer
                libmmv
                libCFG
                libThreadPool
                libInpainting
                )

if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(${PROJECT_NAME} pthread intel_onedal_seq)
    # if gcc version <9 we also need to link the stdc++fs library to support c++17's filesystem extension
    IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(${PROJECT_NAME} stdc++fs)
    ENDIF()
endif(CMAKE_COMPILER_IS_GNUCC)

target_precompile_headers(${PROJECT_NAME} PRIVATE "src/stdafx.h")

CopyInpaintingDlls()