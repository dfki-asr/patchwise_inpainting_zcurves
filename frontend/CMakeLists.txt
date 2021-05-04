cmake_minimum_required(VERSION 3.10)

project(Inpainting)

# find all directories containing source files
set(FILE_TYPES *.c *.cc *.cpp *.h *.hpp *.tcc)
find_directories(DIRS src "${FILE_TYPES}")
generate_source_tree(HOST_SOURCES "${DIRS}" "${FILE_TYPES}")

# set include directories
include_directories("${INPAINTING_DEPENDENCIES_ROOT}"
                    "${FREEIMAGE_INCLUDE_DIR}"
					"${MKL_INCLUDE_DIR}"
					"${DAAL_INCLUDE_DIR}"
                    "${RAPID_XML_INCLUDE_DIR}"
					"${MEMORYMANAGER_INCLUDE_DIR}"
                    "${LIBTIMER_INCLUDE_DIR}"
					"${LIBMMV_INCLUDE_DIR}"
                    "${LIBCFG_INCLUDE_DIR}"
					"${LIBINPAINTING_INCLUDE_DIR}"
					)

add_executable(${PROJECT_NAME} ${HOST_SOURCES})

target_link_libraries(${PROJECT_NAME} 
				${MKL_LIBRARIES}
				${DAAL_LIBRARIES}
				libTimer
				libMMV
				libCFG
				libThreadPool
				libInpainting
				)

if(CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(${PROJECT_NAME} pthread)
endif(CMAKE_COMPILER_IS_GNUCC)

add_precompiled_header(${PROJECT_NAME} stdafx.h)

CopyDlls()
CopyInpaintingDlls()