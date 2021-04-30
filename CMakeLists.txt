cmake_minimum_required(VERSION 3.19)

project(Inpainting)

# set cmake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/c3di_libs/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# redirect output directories
set(CMAKE_BINARY_DIR       ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH    ${EXECUTABLE_OUTPUT_PATH})

include(CommonFlags)
include(GetSourceFiles)
include(PrecompiledHeaders)
include(CopyDlls)
include(CopyInpaintingDlls)
include(Cpp11)

SwitchOnCpp11()

find_package(Dependencies)
find_package(InpaintingDependencies)

# utilities
add_subdirectory(c3di_libs/memoryManager)
add_subdirectory(c3di_libs/libMMV)
add_subdirectory(c3di_libs/libThreadPool)
add_subdirectory(c3di_libs/libTimer)
add_subdirectory(c3di_libs/libCFG)

# utilities
add_subdirectory(libInpainting)
add_dependencies(libInpainting MemoryManager)
add_dependencies(libInpainting libMMV)
add_dependencies(libInpainting libCFG)
add_dependencies(libInpainting libThreadPool)

add_subdirectory(frontend)
add_dependencies(Inpainting libInpainting)
add_dependencies(Inpainting libTimer)

add_subdirectory(test)
add_dependencies(libInpaintingIntegrationTest libInpainting)
add_dependencies(libInpaintingUnitTest libInpainting)