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
                ${MKL_LIBRARIES}
                ${DAAL_LIBRARIES}
                libTimer
                libmmv
                libCFG
                libThreadPool
                libInpainting
                )

if(CMAKE_COMPILER_IS_GNUCC)
    target_link_options(${PROJECT_NAME} PRIVATE ${MKL_LINKER_OPTIONS})
    target_link_directories(${PROJECT_NAME} PRIVATE ${MKL_ROOT_DIR}/lib/intel64/ ${TBB_LIB_DIR} ${DAAL_LIB_DIR})
endif(CMAKE_COMPILER_IS_GNUCC)

target_precompile_headers(${PROJECT_NAME} PRIVATE "src/stdafx.h")

CopyDlls()
CopyInpaintingDlls()