cmake_minimum_required(VERSION 3.10)
project(libInpainting)

INCLUDE(CommonFlags)

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
# generate source tree
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# this is for other Inpainting projects to have access to libMissingWedgeInpainting headers
set(LIBINPAINTING_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIBINPAINTING_INCLUDE_DIR ${LIBINPAINTING_INCLUDE_DIR} PARENT_SCOPE)

# set include directories
include_directories("${INPAINTING_DEPENDENCIES_ROOT}"
                    "${FREEIMAGE_INCLUDE_DIR}"
					"${MKL_INCLUDE_DIR}"
					"${DAAL_INCLUDE_DIR}"
                    "${MEMORYMANAGER_INCLUDE_DIR}"
                    "${LIBMMV_INCLUDE_DIR}"
                    "${LIBCFG_INCLUDE_DIR}"
                    "${LIBTP_INCLUDE_DIR}"
					"${LIBINPAINTING_INCLUDE_DIR}"					
					)

add_library(${PROJECT_NAME} ${HOST_SOURCES})

target_link_libraries(${PROJECT_NAME} 
				libMMV
				)

if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} pthread)
endif(CMAKE_COMPILER_IS_GNUCC)

add_precompiled_header(${PROJECT_NAME} stdafx.h)