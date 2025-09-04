# Plugin system CMake configuration

# Create a shared library for the plugin interface
add_library(jarvis_plugin_interface SHARED plugin_interface.cpp)

# Include directories for plugins
target_include_directories(jarvis_plugin_interface 
    PUBLIC ${CMAKE_SOURCE_DIR}/include
)

# Link libraries for plugins
target_link_libraries(jarvis_plugin_interface
    nlohmann_json::nlohmann_json
    Boost::system
    Boost::filesystem
)

# Set output directory for plugins
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

# Create plugins directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)

# Sample plugin
add_library(sample_plugin SHARED
    sample/sample_plugin.cpp
)

# Set properties for sample plugin
set_target_properties(sample_plugin PROPERTIES
    PREFIX ""  # Remove lib prefix on Linux
    OUTPUT_NAME "sample"
)

# Link plugin with core
find_package(Threads REQUIRED)
target_link_libraries(sample_plugin
    jarvis_plugin_interface
    ${CMAKE_DL_LIBS}
)

# Set C++ standard
target_compile_features(sample_plugin PRIVATE cxx_std_17)

# Copy plugin to build directory
add_custom_command(TARGET sample_plugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:sample_plugin>
    ${CMAKE_BINARY_DIR}/plugins/
)

# Plugin configuration
configure_file(
    ${CMAKE_SOURCE_DIR}/config/plugin_config.json
    ${CMAKE_BINARY_DIR}/config/plugin_config.json
    COPYONLY
)

# Function to create a plugin
function(create_jarvis_plugin name)
    add_library(${name} SHARED ${ARGN})
    
    # Set plugin properties
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        SUFFIX ".jarvis_plugin"
    )
    
    # Link with plugin interface
    target_link_libraries(${name} jarvis_plugin_interface)
    
    # Include directories
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    
    # Install plugin
    install(TARGETS ${name} DESTINATION lib/jarvis/plugins)
endfunction()

# Example plugins (will be created later)
# create_jarvis_plugin(file_manager file_manager.cpp)
# create_jarvis_plugin(web_search web_search.cpp)
# create_jarvis_plugin(calendar calendar.cpp)
# create_jarvis_plugin(code_snippets code_snippets.cpp)

message(STATUS "Plugin system configured - sample plugin ready")