include(FindPackageHandleStandardArgs)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#	message(STATUS "CMake thinks this is 64-bit environment")
	set(POSSIBLE_LIB_SUFFIXES Win64 x64 x86_64 lib/Win64 lib/x86_64 lib/x64)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
#	message(STATUS "CMake thinks this is 32-bit environment")
	set(POSSIBLE_LIB_SUFFIXES Win32 x86 lib/Win32 lib/x86)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

find_path(INPAINTING_DEPENDENCIES_ROOT
  NAMES "dependencties_package.txt"
  HINTS "inpainting_dependencies" "../inpainting_dependencies" "../../inpainting_dependencies"
  DOC "inpainting dependencies root directory")
  message(STATUS "inpainting dependencies root: ${INPAINTING_DEPENDENCIES_ROOT}")

################
# find MKL 
################

find_path(MKL_ROOT_DIR
  NAMES lib/mkl_core_dll.lib
  HINTS ${INPAINTING_DEPENDENCIES_ROOT}/mkl
  DOC "MKL root directory")
  
find_library(MKL_CORE_LIB
  NAMES mkl_core_dll mkl_intel_lp64_dll mkl_sequential_dll
  HINTS ${MKL_ROOT_DIR}/lib
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

find_library(MKL_LP64_LIB
  NAMES mkl_intel_lp64_dll
  HINTS ${MKL_ROOT_DIR}/lib
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

find_library(MKL_SEQUENTIAL_LIB
  NAMES mkl_sequential_dll
  HINTS ${MKL_ROOT_DIR}/lib
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

set(MKL_LIBRARIES ${MKL_CORE_LIB} ${MKL_LP64_LIB} ${MKL_SEQUENTIAL_LIB} CACHE INTERNAL "Intel MKL Libraries" )
set(MKL_INCLUDE_DIR ${MKL_ROOT_DIR}/include CACHE INTERNAL "MKL Include Directory" )
  
find_package_handle_standard_args(mkl REQUIRED_VARS MKL_ROOT_DIR )

################
# find DAAL 
################

find_path(DAAL_ROOT_DIR
  NAMES lib/daal_core_dll.lib
  HINTS ${INPAINTING_DEPENDENCIES_ROOT}/daal
  DOC "DAAL root directory")
    
find_library(DBB_LIB
  NAMES tbb.lib
  HINTS ${DAAL_ROOT_DIR}/lib
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

find_library(DBB_MALLOC_LIB
  NAMES tbbmalloc.lib
  HINTS ${DAAL_ROOT_DIR}/lib
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})
  
find_library(DAAL_CORE_LIB
  NAMES daal_core_dll.lib
  HINTS ${DAAL_ROOT_DIR}/lib
  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

set(DAAL_LIBRARIES ${DBB_LIB} ${DBB_MALLOC_LIB} ${DAAL_CORE_LIB} CACHE INTERNAL "Intel DAAL Libraries" ) 
set(DAAL_INCLUDE_DIR ${DAAL_ROOT_DIR}/include CACHE INTERNAL "DAAL Include Directory" )
  
find_package_handle_standard_args(daal REQUIRED_VARS MKL_ROOT_DIR )
