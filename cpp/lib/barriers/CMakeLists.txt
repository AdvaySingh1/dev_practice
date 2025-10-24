add_library(barrier INTERFACE)
target_include_directories(barrier
    INTERFACE include)
target_link_libraries(barrier 
    INTERFACE types
    INTERFACE functional
    )