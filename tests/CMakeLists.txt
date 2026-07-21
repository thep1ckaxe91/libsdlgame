find_package(GTest REQUIRED)
include(GoogleTest)

set(TEST_MODULES
    color
    display
    draw
    engine
    event
    font
    image
    key
    math
    mixer
    mouse
    music
    random
    rect
    sprite
    surface
    time
    transform
)

add_custom_target(CopyTestAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMENT "Copying assets to test directory"
)

foreach(module IN LISTS TEST_MODULES)
    add_executable(test_${module} test_${module}.cpp)
    target_link_libraries(test_${module} PRIVATE GameEngine GTest::gtest_main)
    add_dependencies(test_${module} CopyTestAssets)
    gtest_discover_tests(test_${module})
endforeach()
