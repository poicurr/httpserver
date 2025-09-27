function(add_httpserver_example target source_dir)
  add_executable(${target} ${source_dir}/main.cpp)
  target_link_libraries(${target} PRIVATE HttpServer)
endfunction()

add_httpserver_example(httpserver_example_basic basic_server)
target_compile_definitions(httpserver_example_basic PRIVATE
  HTTP_SERVER_EXAMPLE_RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/basic_server/resources"
)

add_httpserver_example(httpserver_example_hello_world hello_world)
add_httpserver_example(httpserver_example_json_api json_api)
add_httpserver_example(httpserver_example_echo echo_server)
add_httpserver_example(httpserver_example_keep_alive keep_alive_demo)

