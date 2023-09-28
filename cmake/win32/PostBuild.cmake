if (NOT WIN32)
    message(FATAL_ERROR "This script can be used only on Windows")
endif()

if (NOT DEFINED CURRENT_BINARY_DIR)
    message(FATAL_ERROR "CURRENT_BINARY_DIR is not defined")
endif()

# create symlink to the PointCloudCrust library
file(GLOB PCC_DLL "${CURRENT_BINARY_DIR}/contrib/PointCloudCrust/pcc*.dll")
get_filename_component(PCC_DLL_NAME ${PCC_DLL} NAME)

set(PCC_DLL_LINK ${CURRENT_BINARY_DIR}/${PCC_DLL_NAME})

if (NOT EXISTS "${PCC_DLL_LINK}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${PCC_DLL}"
            "${PCC_DLL_LINK}")
endif()
