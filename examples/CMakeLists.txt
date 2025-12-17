# CMake configuration for examples

# Add an executable for the example
add_executable(example_main main.cpp)

# Link the dec64 library to the example
# This ensures the example can use the dec64 functionality
target_link_libraries(example_main PRIVATE dec64)

# Set the C++ standard for the example
target_compile_features(example_main PRIVATE cxx_std_17)

# Add include directories for the example
target_include_directories(example_main PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

# Set the output directory for the example executable
set_target_properties(example_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)