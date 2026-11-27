cmake_minimum_required(VERSION 3.14)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g")

project(libyomu)

add_subdirectory(./vendor)
add_subdirectory(./app)

add_library(yomu)

set(SRC
    ./src/util.cpp
    ./src/Term.cpp
    ./src/Dictionary.cpp
    ./src/Glossary.cpp
    ./src/Frequency.cpp
    ./src/TermMeta.cpp
    ./src/Kanji.cpp
    ./src/Tag.cpp
    ./src/Image.cpp
    ./src/StructuredContent.cpp
)

set(INC
    ./include/libyomu/Term.hpp
    ./include/libyomu/Glossary.hpp
    ./include/libyomu/Dictionary.hpp
    ./include/libyomu/Error.hpp
    ./include/libyomu/Frequency.hpp
    ./include/libyomu/TermMeta.hpp
    ./include/libyomu/Kanji.hpp
    ./include/libyomu/Tag.hpp
    ./include/libyomu/Image.hpp
    ./include/libyomu/StructuredContent.hpp
)

target_sources(yomu
  PRIVATE
    ${SRC}
  PUBLIC
    FILE_SET HEADERS
    FILES
    ${INC}
)

target_include_directories(yomu PUBLIC ./include)

target_link_libraries(yomu
    PUBLIC
      nlohmann_json::nlohmann_json
      libzippp::libzippp
)

target_compile_features(yomu PRIVATE cxx_std_20)

