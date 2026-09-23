cmake_minimum_required(VERSION 3.50)

project(osproxy VERSION 1.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(VersionInformation)

find_package(fmt CONFIG REQUIRED)
find_package(safetyhook CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED
    src/osrs.cpp
    src/memory.cpp
    src/modules.cpp
    src/entrypoint.cpp
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    safetyhook::safetyhook
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "META_PROJECT_VERSION=\"${PROJECT_VERSION}\""
    "META_GIT_BRANCH=\"${META_GIT_BRANCH}\""
    "META_GIT_COMMIT_SHORT=\"${META_GIT_COMMIT_SHORT}\""
    "META_COMPILER_ID=\"${CMAKE_CXX_COMPILER_ID}\""
    "META_COMPILER_VERSION=\"${CMAKE_CXX_COMPILER_VERSION}\""
)
