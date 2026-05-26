cmake_minimum_required(VERSION 3.10)
project(HuffmanCoding LANGUAGES CXX)

if(NOT CMAKE_CXX_COMPILER)
	set(CMAKE_CXX_COMPILER g++)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(huffmanCoding huffmanCoding.cpp huffman.cpp)
add_executable(huffmanDecoding huffmanDecoding.cpp huffman.cpp)

enable_testing()

set(HUFFMAN_TEST_SCRIPT ${CMAKE_SOURCE_DIR}/tests/run_tests.sh)
add_test(
	NAME huffman_roundtrip
	COMMAND bash ${HUFFMAN_TEST_SCRIPT} $<TARGET_FILE:huffmanCoding> $<TARGET_FILE:huffmanDecoding>
)
