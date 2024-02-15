
add_library(neural SHARED neural.cpp)
#add_executable(neural neural.cpp)

target_link_libraries(neural
    lua54
)


