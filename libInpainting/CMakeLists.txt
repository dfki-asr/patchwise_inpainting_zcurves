cmake_minimum_required(VERSION 3.19)
project(libInpainting)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

INCLUDE(CommonFlags)

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# this is for other Inpainting projects to have access to libMissingWedgeInpainting headers
set(LIBINPAINTING_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIBINPAINTING_INCLUDE_DIR ${LIBINPAINTING_INCLUDE_DIR} PARENT_SCOPE)

message("libInpainting DAAL_INCLUDE_DIR ${DAAL_INCLUDE_DIR}")
message("libInpainting MKL_INCLUDE_DIR  ${MKL_INCLUDE_DIR}")

include_directories("${INPAINTING_DEPENDENCIES_ROOT}"
                    "${FREEIMAGEPLUS_INCLUDE_PATH}"
                    "${MKL_INCLUDE_DIR}"
                    "${DAAL_INCLUDE_DIR}"
                    "${LIBMEMORYMANAGER_INCLUDE_DIR}"
                    "${LIBMMV_INCLUDE_DIR}"
                    "${LIBCFG_INCLUDE_DIR}"
                    "${LIBTP_INCLUDE_DIR}"
                    "${LIBINPAINTING_INCLUDE_DIR}"
                    )

add_library(${PROJECT_NAME} ${HOST_SOURCES})

target_link_libraries(${PROJECT_NAME}
                      ${MKL_LIBRARIES}
                      MemoryManager
                      intel_onedal_core
                      intel_mkl_lp64
                      intel_mkl_core
                      intel_mkl_seq
                      libmmv
                      libThreadPool
                      )


if(CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(${PROJECT_NAME} pthread intel_onedal_seq)
    # if gcc version <9 we also need to link the stdc++fs library to support c++17's filesystem extension
    IF (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(${PROJECT_NAME} stdc++fs)
    ENDIF()
endif(CMAKE_COMPILER_IS_GNUCC)

target_precompile_headers(${PROJECT_NAME} PRIVATE "${LIBINPAINTING_INCLUDE_DIR}/stdafx.h")
