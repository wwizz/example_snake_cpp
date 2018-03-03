
file(GLOB_RECURSE source_files "*.cpp" "*.h")
add_executable(huestream_tests ${source_files})
target_link_libraries(huestream_tests
        huestream
        gtest_main
        gtest
        gmock
        )


if (BUILD_TEST_COV)
    set(coverage_project_name huestream_tests_coverage)
    set(coverage_build_dir ${CMAKE_BINARY_DIR}/bin/coverage)
    add_custom_target( ${coverage_project_name}
            DEPENDS huestream_tests
            )

    add_custom_command(TARGET ${coverage_project_name}
            COMMENT "Run tests for coverage"
            POST_BUILD COMMAND $<TARGET_FILE:huestream_tests>
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )

    add_custom_command(TARGET ${coverage_project_name}
            COMMENT "Creating dir: ${coverage_build_dir}"
            POST_BUILD COMMAND ${CMAKE_COMMAND}
            ARGS -E make_directory ${coverage_build_dir})

    add_custom_command(TARGET ${coverage_project_name}
            COMMENT "Generating html report coverage: ${coverage_build_dir}/${coverage_project_name}.html"
            POST_BUILD COMMAND gcovr
            ARGS --html --html-details -f '.*/huestream/.*' -o ${coverage_build_dir}/${coverage_project_name}.html -r .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )

    add_custom_command(TARGET ${coverage_project_name}
            COMMENT "Generating xml report coverage: ${coverage_build_dir}/${coverage_project_name}.xml"
            POST_BUILD COMMAND gcovr
            ARGS --xml -f '.*/huestream/.*' -o ${coverage_build_dir}/${coverage_project_name}.xml -r .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            )

    include(ExternalCpplint)
    add_dependencies(huestream_tests ${EXTERNAL_CPPLINT})
    set(LINT_RUNNER_DIR ${CMAKE_CURRENT_LIST_DIR}/../../tools/cpplint)

    add_custom_command(TARGET ${coverage_project_name}
            COMMENT "Launching cpplint runner: ${LINT_RUNNER_DIR}/lint_edk.sh"
            POST_BUILD COMMAND ./lint_edk.sh ${CPPLINT_EXECUTABLE}
            WORKING_DIRECTORY ${LINT_RUNNER_DIR}
            )
endif()


if (WIN32)
    copy_libs_to_output(huestream_tests "${shared_libs}")
endif ()
