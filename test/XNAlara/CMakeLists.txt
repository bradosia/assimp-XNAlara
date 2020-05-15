cmake_minimum_required(VERSION 3.0)

add_executable(example_xnalara
    main.cpp
)
target_include_directories(example_xnalara PUBLIC
    ../../
)
target_link_libraries(example_xnalara PUBLIC
    assimp-xnalara
)
