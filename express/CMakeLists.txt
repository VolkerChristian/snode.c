cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(nlohmann_json 3.7.0 REQUIRED)

add_library(
    express SHARED
    Response.cpp
    Request.cpp
    Router.cpp
    WebApp.cpp
    middleware/JsonMiddleware.cpp
    middleware/StaticMiddleware.cpp
    middleware/VHost.cpp
)

target_link_libraries(express LINK_PUBLIC utils logger net web)
target_link_libraries(express PRIVATE nlohmann_json::nlohmann_json)

target_compile_options(express PRIVATE --warn-all)

set_target_properties(
    express
    PROPERTIES CXX_STANDARD 20
               CXX_STANDARD_REQUIRED ON
               CXX_EXTENSIONS OFF
               SOVERSION 1
)

target_include_directories(express PUBLIC ${PROJECT_SOURCE_DIR})

install(TARGETS express LIBRARY DESTINATION lib
                                NAMELINK_COMPONENT libexpress.so.1
)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION include/snode.c/express
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "cmake" EXCLUDE
)

if(CHECK_INCLUDES)
    set_property(
        TARGET express PROPERTY CXX_INCLUDE_WHAT_YOU_USE
                                ${iwyu_path_and_options}
    )
endif(CHECK_INCLUDES)
