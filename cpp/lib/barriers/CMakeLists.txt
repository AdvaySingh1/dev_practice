add_library(barrier barrier.cpp)
target_include_directories(barrier
    PUBLIC include)
target_link_libraries(barrier 
    PUBLIC types
    PUBLIC functional
    )