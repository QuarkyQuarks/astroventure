function(SoLoud_collect_sources)
    cmake_parse_arguments(
        PARSED_ARGS # prefix of output variables
        "" # list of names of the boolean arguments (only defined ones will be true)
        "" # list of names of mono-valued arguments
        "BACKENDS;AUDIOSOURCES;" # list of names of multi-valued arguments (output variables are lists)
        ${ARGN} # arguments of the function to parse, here we take the all original ones
    )

    if (NOT PARSED_ARGS_BACKENDS)
        message(FATAL_ERROR "You must provide a list of enabled backends (BACKENDS)")
    endif()

    if (NOT PARSED_ARGS_AUDIOSOURCES)
        message(FATAL_ERROR "You must provide a list of enabled audio sources (AUDIOSOURCES)")
    endif()

    set(soloud_core_dir "contrib/soloud/src/core")
    set(soloud_backends_dir "contrib/soloud/src/backend")
    set(soloud_audiosources_dir "contrib/soloud/src/audiosource")

    macro(collect_sources output_var dir)
        file(GLOB ${output_var}
                ${dir}/*.c ${dir}/*.cpp
                ${dir}/*.h ${dir}/*.hpp)
    endmacro()

    collect_sources(SoLoud_core "${soloud_core_dir}")
    set(SoLoud_core "${SoLoud_core}" PARENT_SCOPE)

    foreach(backend IN LISTS PARSED_ARGS_BACKENDS)
        collect_sources(SoLoud_backend_${backend} "${soloud_backends_dir}/${backend}")
        set(SoLoud_backend_${backend} "${SoLoud_backend_${backend}}" PARENT_SCOPE)
    endforeach()

    foreach(source IN LISTS PARSED_ARGS_AUDIOSOURCES)
        collect_sources(SoLoud_source_${source} "${soloud_audiosources_dir}/${source}")
        set(SoLoud_audiosources "${SoLoud_audiosources};${SoLoud_source_${source}}")
    endforeach()

    set(SoLoud_audiosources "${SoLoud_audiosources}" PARENT_SCOPE)
endfunction()
