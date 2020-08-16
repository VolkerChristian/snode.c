cmake_minimum_required(VERSION 3.0)

add_executable(snode.c main.cpp)
target_compile_options(snode.c PRIVATE --warn-all)
target_link_libraries(snode.c LINK_PUBLIC http)
install(TARGETS snode.c RUNTIME DESTINATION bin)
set_target_properties(snode.c PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            SOVERSION 1
)

add_executable(posttest posttest.cpp)
target_compile_options(posttest PRIVATE --warn-all)
target_link_libraries(posttest LINK_PUBLIC http)
install(TARGETS posttest RUNTIME DESTINATION bin)
set_target_properties(posttest PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            SOVERSION 1
)

add_executable(simpleserver simpleserver.cpp)
target_compile_options(simpleserver PRIVATE --warn-all)
target_link_libraries(simpleserver LINK_PUBLIC http)
install(TARGETS simpleserver RUNTIME DESTINATION bin)
set_target_properties(simpleserver PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            SOVERSION 1
)

add_executable(experiments experiments.cpp)
target_compile_options(experiments PRIVATE --warn-all)
target_link_libraries(experiments LINK_PUBLIC http utils)
install(TARGETS experiments RUNTIME DESTINATION bin)
set_target_properties(experiments PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            SOVERSION 1
)

add_executable(client client.cpp)
target_compile_options(client PRIVATE --warn-all)
target_link_libraries(client LINK_PUBLIC http utils)
install(TARGETS client RUNTIME DESTINATION bin)
set_target_properties(client PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            SOVERSION 1
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/logger.conf"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/logger.conf" "${CMAKE_CURRENT_BINARY_DIR}"
            DEPENDS "${CMAKE_SOURCE_DIR}/logger.conf"
)

add_custom_target(copy_logger.conf ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/logger.conf")