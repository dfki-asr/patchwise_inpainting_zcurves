cmake_minimum_required(VERSION 2.8)

set(TEST_INCLUDE_DIR "src")

project(libInpaintingUnitTest)
set(TEST_SOURCE_DIRS "src/unit;src/common")
INCLUDE(CMakeTests.txt)

project(libInpaintingIntegrationTest)
set(TEST_SOURCE_DIRS "src/integration;src/common")
INCLUDE(CMakeTests.txt)
