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

    # Qt libraries
    target_link_libraries(tst_testqsingleinstancecheck PRIVATE
        Qt6::Test
        Qt6::Core
        Qt6::Network
    )

    # Add to test
    add_test(NAME QSingleInstanceCheck COMMAND tst_testqsingleinstancecheck)
endif()
