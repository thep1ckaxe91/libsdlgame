find_package(GTest REQUIRED)

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

foreach(module IN LISTS TEST_MODULES)
    add_executable(test_${module} test_${module}.cpp)
    target_link_libraries(test_${module} PRIVATE GameEngine GTest::gtest_main)
    gtest_discover_tests(test_${module})
endforeach()
