include(FindPackageHandleStandardArgs)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#	message(STATUS "CMake thinks this is 64-bit environment")
	if (UNIX)
		set(POSSIBLE_LIB_SUFFIXES x64 x86_64 lib/x86_64 lib/x64 lib/intel64/ )
	endif (UNIX)
	if (WIN32)
		set(POSSIBLE_LIB_SUFFIXES Win64 x64 x86_64 lib/Win64 lib/x86_64 )
	endif (WIN32)
else(CMAKE_SIZEOF_VOID_P EQUAL 8)
#	message(STATUS "CMake thinks this is 32-bit environment")
	set(POSSIBLE_LIB_SUFFIXES Win32 x86 lib/Win32 lib/x86)
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

find_path(INPAINTING_DEPENDENCIES_ROOT
  NAMES "dependencies_package.txt"
  HINTS "inpainting_dependencies" "../inpainting_dependencies" "../../inpainting_dependencies"
  DOC "inpainting dependencies root directory")
  message(STATUS "inpainting dependencies root: ${INPAINTING_DEPENDENCIES_ROOT}")

################
# find MKL 
################

file(TO_CMAKE_PATH "$ENV{MKLROOT}" MKL_ROOT_DIR)
message("-- Linking Intel MKL: Looking in ${MKL_ROOT_DIR}")

add_library(intel_mkl_core SHARED IMPORTED REQUIRED)
add_library(intel_mkl_seq SHARED IMPORTED REQUIRED)
add_library(intel_mkl_lp64 SHARED IMPORTED REQUIRED)

if (WIN32)
	set_target_properties(intel_mkl_core PROPERTIES IMPORTED_LOCATION ${MKL_ROOT_DIR}/redist/intel64/mkl_core.1.dll)
	set_target_properties(intel_mkl_core PROPERTIES IMPORTED_IMPLIB ${MKL_ROOT_DIR}/lib/intel64/mkl_core_dll.lib)
	set_target_properties(intel_mkl_core PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MKL_ROOT_DIR}/include)
	
	set_target_properties(intel_mkl_seq PROPERTIES IMPORTED_LOCATION ${MKL_ROOT_DIR}/redist/intel64/mkl_sequential.1.dll)
	set_target_properties(intel_mkl_seq PROPERTIES IMPORTED_IMPLIB ${MKL_ROOT_DIR}/lib/intel64/mkl_sequential_dll.lib)
	set_target_properties(intel_mkl_seq PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MKL_ROOT_DIR}/include)
	
	#set_target_properties(intel_mkl_lp64 PROPERTIES IMPORTED_LOCATION ${MKL_ROOT_DIR}/redist/intel64/mkl_intel_ilp64_dll.lib)
	set_target_properties(intel_mkl_lp64 PROPERTIES IMPORTED_IMPLIB ${MKL_ROOT_DIR}/lib/intel64/mkl_intel_ilp64_dll.lib)
	set_target_properties(intel_mkl_lp64 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MKL_ROOT_DIR}/include)
endif (WIN32)

if (UNIX)
	set_target_properties(intel_mkl_core PROPERTIES IMPORTED_LOCATION ${MKL_ROOT_DIR}/lib/intel64/libmkl_core.so)
	set_target_properties(intel_mkl_core PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MKL_ROOT_DIR}/include)
	
	set_target_properties(intel_mkl_seq PROPERTIES IMPORTED_LOCATION ${MKL_ROOT_DIR}/lib/intel64/libmkl_sequential.so)
	set_target_properties(intel_mkl_seq PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MKL_ROOT_DIR}/include)
	
	set_target_properties(intel_mkl_lp64 PROPERTIES IMPORTED_LOCATION ${MKL_ROOT_DIR}/lib/intel64/libmkl_intel_ilp64.so)
	set_target_properties(intel_mkl_lp64 PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${MKL_ROOT_DIR}/include)
endif (UNIX)

set(MKL_INCLUDE_DIR ${MKL_ROOT_DIR}/include/ CACHE INTERNAL "MKL Include Directory" )


################
# find DAAL 
################

file(TO_CMAKE_PATH "$ENV{DAALROOT}" DAAL_ROOT_DIR)
message("-- Linking Intel OneDAL: Looking in ${DAAL_ROOT_DIR}")

add_library(intel_onedal_core SHARED IMPORTED REQUIRED)

if (WIN32)
	set_target_properties(intel_onedal_core PROPERTIES IMPORTED_LOCATION ${DAAL_ROOT_DIR}/redist/intel64/onedal_core.dll)
	set_target_properties(intel_onedal_core PROPERTIES IMPORTED_IMPLIB ${DAAL_ROOT_DIR}/lib/intel64/onedal_core_dll.lib)
	set_target_properties(intel_onedal_core PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${DAAL_ROOT_DIR}/include)
endif (WIN32)

if (UNIX)
	set_target_properties(intel_onedal_core PROPERTIES IMPORTED_LOCATION ${DAAL_ROOT_DIR}/lib/intel64/libonedal_core.so)
	set_target_properties(intel_onedal_core PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${DAAL_ROOT_DIR}/include)
	
	add_library(intel_onedal_seq SHARED IMPORTED REQUIRED)
	set_target_properties(intel_onedal_seq PROPERTIES IMPORTED_LOCATION ${DAAL_ROOT_DIR}/lib/intel64/libonedal_sequential.so)
	set_target_properties(intel_onedal_seq PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${DAAL_ROOT_DIR}/include)
endif (UNIX)

set(DAAL_INCLUDE_DIR ${DAAL_ROOT_DIR}/include/ CACHE INTERNAL "DAAL Include Directory" )
