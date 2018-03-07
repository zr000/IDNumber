########################################

set(REBUILD 1)

if (EXISTS ${fn1})
    file(READ ${fn1} f1)
    if (EXISTS ${fn2})
        file(READ ${fn2} f2)
        if (f1 STREQUAL f2)
            set(REBUILD 0)
        endif()
    else()
        file(WRITE ${fn2} "${f1}")
    endif()
endif()

if (NOT REBUILD AND EXISTS ${TARGET_FILE})
    return()
endif()

set(lock ${BUILD_DIR}/cppan_build.lock)

file(
    LOCK ${lock}
    # cannot make GUARD FILE here: https://gitlab.kitware.com/cmake/cmake/issues/16480
    #GUARD FILE # CMake bug workaround https://gitlab.kitware.com/cmake/cmake/issues/16295
    RESULT_VARIABLE lock_result
)
if (NOT ${lock_result} EQUAL 0)
    message(FATAL_ERROR "Lock error: ${lock_result}")
endif()

# double check
if (NOT REBUILD AND EXISTS ${TARGET_FILE})
    # release before exit
    file(LOCK ${lock} RELEASE)

    return()
endif()

# save file
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${fn1} ${fn2})

########################################
# preparation is over, build now
########################################

# TODO: add good message, close enough to generate message
#message(STATUS "Building ${target} (${config_unhashed} - ${config_dir} - ${generator})")

# make could be found on win32 os from cygwin for example
# we deny it on msvc and other build systems except for cygwin
if (NOT WIN32 OR CYGWIN)
    find_program(make make)
endif()

# set vars
set(OUTPUT_QUIET)
set(ERROR_QUIET)
if (DEFINED CPPAN_BUILD_VERBOSE AND NOT CPPAN_BUILD_VERBOSE)
    set(OUTPUT_QUIET OUTPUT_QUIET)
    set(ERROR_QUIET ERROR_QUIET)
endif()

# TODO: maybe provide better way of parallel build
# from cppan's build() call
# Q: how to pass -j options to cmake --build?
# A: cmake --build .  *more args* -- -jN
# maybe add similar options to cppan? '-- pass all args after two dashes to cmake build'
set(parallel)
if (MULTICORE)
    #set(parallel "-j ${N_CORES}") # temporary
endif()

if (CONFIG)
    if (NOT DEFINED make OR
        "${make}" STREQUAL "" OR
        "${make}" STREQUAL "make-NOTFOUND" OR
        XCODE)
        if (EXECUTABLE)
                if (CPPAN_BUILD_EXECUTABLES_WITH_SAME_CONFIGURATION)
                    cppan_debug_message("COMMAND ${CMAKE_COMMAND}
                            --build ${BUILD_DIR}
                            --config ${CONFIG}")
                    execute_process(
                        COMMAND ${CMAKE_COMMAND}
                            --build ${BUILD_DIR}
                            --config ${CONFIG}
                        ${OUTPUT_QUIET}
                        ${ERROR_QUIET}
                        RESULT_VARIABLE ret
                    )
                else()
                    cppan_debug_message("COMMAND ${CMAKE_COMMAND}
                            --build ${BUILD_DIR}
                            --config Release")
                    execute_process(
                        COMMAND ${CMAKE_COMMAND}
                            --build ${BUILD_DIR}
                            --config Release
                        ${OUTPUT_QUIET}
                        ${ERROR_QUIET}
                        RESULT_VARIABLE ret
                    )
                endif()
        else()
                cppan_debug_message("COMMAND ${CMAKE_COMMAND}
                        --build ${BUILD_DIR}
                        --config ${CONFIG}")
                execute_process(
                    COMMAND ${CMAKE_COMMAND}
                        --build ${BUILD_DIR}
                        --config ${CONFIG}
                    ${OUTPUT_QUIET}
                    ${ERROR_QUIET}
                    RESULT_VARIABLE ret
                )
        endif()
    else()
        cppan_debug_message("COMMAND make ${parallel} -C ${BUILD_DIR}")
        execute_process(
            COMMAND make ${parallel} -C ${BUILD_DIR}
            ${OUTPUT_QUIET}
            ${ERROR_QUIET}
            RESULT_VARIABLE ret
        )
    endif()
else(CONFIG)
    if ("${make}" STREQUAL "make-NOTFOUND")
        cppan_debug_message("COMMAND ${CMAKE_COMMAND}
                --build ${BUILD_DIR}")
        execute_process(
            COMMAND ${CMAKE_COMMAND}
                --build ${BUILD_DIR}
            ${OUTPUT_QUIET}
            ${ERROR_QUIET}
            RESULT_VARIABLE ret
        )
    else()
        cppan_debug_message("COMMAND make ${parallel} -C ${BUILD_DIR}")
        execute_process(
            COMMAND make ${parallel} -C ${BUILD_DIR}
            ${OUTPUT_QUIET}
            ${ERROR_QUIET}
            RESULT_VARIABLE ret
        )
    endif()
endif(CONFIG)

check_result_variable(${ret})

file(LOCK ${lock} RELEASE)

########################################
