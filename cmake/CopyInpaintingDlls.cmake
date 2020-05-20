macro(CopyDlls)
    # copy necessary dependency DLLs to destination folder
    if(MSVC)
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Debug/")
        add_custom_command(
            TARGET ${PROJECT_NAME} 
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${MKL_ROOT_DIR}/lib/mkl_core.dll"
                "${MKL_ROOT_DIR}/lib/mkl_sequential.dll"
                "${MKL_ROOT_DIR}/lib/mkl_avx2.dll"
                "${MKL_ROOT_DIR}/lib/mkl_def.dll"
                "${DAAL_ROOT_DIR}/bin/daal_core.dll"
                "${DAAL_ROOT_DIR}/bin/daal_thread.dll"
                "${DAAL_ROOT_DIR}/bin/tbb.dll"
                "${DAAL_ROOT_DIR}/bin/tbbmalloc.dll"
                "${CMAKE_BINARY_DIR}/bin/Debug/"
        )
        
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Release/")
        add_custom_command(
            TARGET ${PROJECT_NAME} 
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${MKL_ROOT_DIR}/lib/mkl_core.dll"
            "${MKL_ROOT_DIR}/lib/mkl_sequential.dll"
            "${MKL_ROOT_DIR}/lib/mkl_avx2.dll"
            "${MKL_ROOT_DIR}/lib/mkl_def.dll"
            "${DAAL_ROOT_DIR}/bin/daal_core.dll"
            "${DAAL_ROOT_DIR}/bin/daal_thread.dll"
            "${DAAL_ROOT_DIR}/bin/tbb.dll"
            "${DAAL_ROOT_DIR}/bin/tbbmalloc.dll"
            "${CMAKE_BINARY_DIR}/bin/Release/"
        )
        
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/")
        add_custom_command(
            TARGET ${PROJECT_NAME} 
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${MKL_ROOT_DIR}/lib/mkl_core.dll"
            "${MKL_ROOT_DIR}/lib/mkl_sequential.dll"
            "${MKL_ROOT_DIR}/lib/mkl_avx2.dll"
            "${MKL_ROOT_DIR}/lib/mkl_def.dll"
            "${DAAL_ROOT_DIR}/bin/daal_core.dll"
            "${DAAL_ROOT_DIR}/bin/daal_thread.dll"
            "${DAAL_ROOT_DIR}/bin/tbb.dll"
            "${DAAL_ROOT_DIR}/bin/tbbmalloc.dll"
            "${CMAKE_BINARY_DIR}/bin/RelWithDebInfo/"
        )

    endif(MSVC)
endmacro()