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

if (WIN32)
	find_path(MKL_ROOT_DIR
	  NAMES lib/mkl_core_dll.lib
	  HINTS ${INPAINTING_DEPENDENCIES_ROOT}/mkl
	  DOC "MKL root directory")
  
	find_library(MKL_CORE_LIB
	  NAMES mkl_core_dll
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

	set(MKL_LIBRARIES "${MKL_CORE_LIB} ${MKL_LP64_LIB} ${MKL_SEQUENTIAL_LIB}" CACHE INTERNAL "Intel MKL Libraries" )
endif (WIN32)


if (UNIX)
	message( STATUS "ENV MKLROOT $ENV{MKLROOT}" )

	find_path(MKL_ROOT_DIR
	  NAMES lib/intel64/libmkl_core.a
	  HINTS ENV "MKLROOT"
	  DOC "MKL root directory")

	message( STATUS "MKL_ROOT_DIR ${MKL_ROOT_DIR}" )

	find_library(MKL_CORE_LIB
	  NAMES libmkl_core.a
	  HINTS ${MKL_ROOT_DIR}
	  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

	message( STATUS "MKL_CORE_LIB ${MKL_CORE_LIB}" )

	find_library(MKL_LP64_LIB
	  NAMES libmkl_intel_ilp64.a
	  HINTS ${MKL_ROOT_DIR}
	  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

	message( STATUS "MKL_LP64_LIB ${MKL_LP64_LIB}" )	

	find_library(MKL_SEQUENTIAL_LIB
	  NAMES libmkl_sequential.a
	  HINTS ${MKL_ROOT_DIR}
	  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

	message( STATUS "MKL_SEQUENTIAL_LIB ${MKL_SEQUENTIAL_LIB}" )

	set(MKL_LIBRARIES "-lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl" CACHE INTERNAL "Intel MKL Libraries" )
	set(MKL_LINKER_OPTIONS "-Wl,--no-as-needed")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMKL_ILP64 -m64")
	message( STATUS "MKL_LIBRARIES ${MKL_LIBRARIES}" )

endif (UNIX)

set(MKL_INCLUDE_DIR ${MKL_ROOT_DIR}/include/ CACHE INTERNAL "MKL Include Directory" )
  
find_package_handle_standard_args(mkl REQUIRED_VARS MKL_ROOT_DIR )

################
# find DAAL 
################

if (WIN32)
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
endif (WIN32)

if (UNIX)
	message( STATUS "ENV DAALROOT $ENV{MKLROOT}" )

	find_path(DAAL_ROOT_DIR
	  NAMES lib/intel64/libdaal_core.so
	  HINTS ENV "DAALROOT"
	  DOC "DAAL root directory")

	  set(DAAL_LIB_DIR "${DAAL_ROOT_DIR}/lib/intel64/")
	  message( STATUS "DAAL_LIB_DIR ${DAAL_LIB_DIR}" )

	  find_library(DAAL_CORE_LIB
	  NAMES libdaal_core.so
	  HINTS ${DAAL_ROOT_DIR}
	  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

	  message( STATUS "DAAL_CORE_LIB ${DAAL_CORE_LIB}" )

	  find_library(DAAL_CORE_SEQ
	  NAMES libdaal_sequential.so
	  HINTS ${DAAL_ROOT_DIR}
	  PATH_SUFFIXES ${POSSIBLE_LIB_SUFFIXES})

	  message( STATUS "DAAL_CORE_SEQ ${DAAL_CORE_SEQ}" )

	  set(DAAL_LIBRARIES -ldaal_core -ldaal_sequential CACHE INTERNAL "Intel DAAL Libraries" )   
endif (UNIX)

set(DAAL_INCLUDE_DIR ${DAAL_ROOT_DIR}/include/ CACHE INTERNAL "DAAL Include Directory" )
  
find_package_handle_standard_args(daal REQUIRED_VARS MKL_ROOT_DIR )
