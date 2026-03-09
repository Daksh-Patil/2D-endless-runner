cmake_minimum_required(VERSION 3.10)
project(EndlessRunner VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add MSYS2 UCRT64 path to prefix path
set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64" ${CMAKE_PREFIX_PATH})

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add Executable
add_executable(EndlessRunner 
    src/main.cpp 
)

# Link SFML
target_link_libraries(EndlessRunner PRIVATE sfml-graphics sfml-window sfml-system)

# Copy Assets to Build Directory
add_custom_command(TARGET EndlessRunner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/BlueWizard"
    "$<TARGET_FILE_DIR:EndlessRunner>/BlueWizard"
)
add_custom_command(TARGET EndlessRunner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/SlimeGreen"
    "$<TARGET_FILE_DIR:EndlessRunner>/SlimeGreen"
)
add_custom_command(TARGET EndlessRunner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Parallax Cave"
    "$<TARGET_FILE_DIR:EndlessRunner>/Parallax Cave"
)
add_custom_command(TARGET EndlessRunner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/Mossy - FloatingPlatforms.png"
    "$<TARGET_FILE_DIR:EndlessRunner>/Mossy - FloatingPlatforms.png"
)
