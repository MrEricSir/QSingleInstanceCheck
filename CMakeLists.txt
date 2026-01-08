cmake_minimum_required(VERSION 3.16)

project(QSingleInstanceCheck VERSION 1.0.0 LANGUAGES CXX)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Network)

# Create library as a header-only library without binaries
add_library(QSingleInstanceCheck INTERFACE)
add_library(QSingleInstanceCheck::QSingleInstanceCheck ALIAS QSingleInstanceCheck)

# Add our .h file here
target_sources(QSingleInstanceCheck INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include/QSingleInstanceCheck/QSingleInstanceCheck.h"
)

# Include directory
target_include_directories(QSingleInstanceCheck INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Unit tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "🛠️ Building tests")

    enable_testing()

    # Add Qt Test
    find_package(Qt6 REQUIRED COMPONENTS Test)

    # Generate Qt .moc file
    set(CMAKE_AUTOMOC ON)

    # TestQSingleInstanceCheck test executable
    qt_add_executable(tst_testqsingleinstancecheck
        tests/tst_testqsingleinstancecheck.cpp
        # Source files under test
        ${CMAKE_SOURCE_DIR}/include/QSingleInstanceCheck/QSingleInstanceCheck.h
    )

    # Apply coverage flags to the test binary
    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(tst_testqsingleinstancecheck PRIVATE --coverage -O0 -g)
        target_link_options(tst_testqsingleinstancecheck PRIVATE --coverage)
    endif()

    # QSingleInstanceCheck and Qt libraries
    target_link_libraries(tst_testqsingleinstancecheck PRIVATE
        QSingleInstanceCheck
        Qt6::Test
        Qt6::Core
        Qt6::Network
    )

    # Add to test
    add_test(NAME QSingleInstanceCheck COMMAND tst_testqsingleinstancecheck)
endif()

# Coverage
if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Code coverage enabled")

        # Add coverage flags to main target
        target_compile_options(QSingleInstanceCheck INTERFACE --coverage -O0 -g)
        target_link_options(QSingleInstanceCheck INTERFACE --coverage)

        # Add custom target for coverage report generation
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)

        if(LCOV_PATH AND GENHTML_PATH)
            add_custom_target(coverage
                # Reset coverage counters
                COMMAND ${LCOV_PATH} --directory . --zerocounters

                # Run tests
                COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure

                # Capture coverage data
                COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info --ignore-errors inconsistent,format,gcov,unsupported,unused

                # Remove system and test files from coverage
                COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/Qt/*' '*/test/*' '*_autogen/*' --output-file coverage.info.cleaned --ignore-errors inconsistent,format,gcov,unsupported,unused

                # Generate HTML report
                COMMAND ${GENHTML_PATH} coverage.info.cleaned --output-directory coverage_html --ignore-errors inconsistent,format,unused

                # Print summary
                COMMAND ${LCOV_PATH} --list coverage.info.cleaned --ignore-errors inconsistent,format,gcov,unsupported,unused

                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating coverage report"
            )

            # Add a lightweight target to generate HTML from existing coverage data
            add_custom_target(coverage-html
                # Capture coverage data
                COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
                    --ignore-errors inconsistent,format,gcov,unsupported,unused

                # Remove system and test files
                COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/Qt/*' '*/test/*' '*_autogen/*'
                    --output-file coverage.info.cleaned
                    --ignore-errors inconsistent,format,gcov,unsupported,unused

                # Generate HTML report
                COMMAND ${GENHTML_PATH} coverage.info.cleaned --output-directory coverage_html
                    --ignore-errors inconsistent,format,unused

                # Print location and try to open in browser
                COMMAND ${CMAKE_COMMAND} -E echo ""
                COMMAND ${CMAKE_COMMAND} -E echo "======================================================================"
                COMMAND ${CMAKE_COMMAND} -E echo "Coverage report: ${CMAKE_BINARY_DIR}/coverage_html/index.html"
                COMMAND ${CMAKE_COMMAND} -E echo "======================================================================"
                COMMAND ${CMAKE_COMMAND} -E echo ""

                # Try to open in default browser (best effort, may not work from all contexts)
                COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Opening in browser..."
                COMMAND open ${CMAKE_BINARY_DIR}/coverage_html/index.html || xdg-open ${CMAKE_BINARY_DIR}/coverage_html/index.html || ${CMAKE_COMMAND} -E echo "Note: Could not open browser automatically. Use the link above."

                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating HTML coverage report and opening in browser"
            )

            message(STATUS "Coverage target 'coverage' available - run: make coverage (runs tests + generates HTML)")
            message(STATUS "Coverage target 'coverage-html' available - run: make coverage-html (generates HTML from existing test data)")
        else()
            message(WARNING "lcov or genhtml not found - coverage target not available")
        endif()
    else()
        message(WARNING "Coverage only supported with GCC or Clang")
    endif()
endif()

# Documentation
find_program(QDOC_EXE qdoc)
if(QDOC_EXE)
    add_custom_target(docs
        COMMAND ${QDOC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/config.qdocconf
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running QDoc to generate documentation"
    )
endif()
