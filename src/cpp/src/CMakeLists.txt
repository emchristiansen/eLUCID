add_definitions("-msse4.2")

# deal with the LUCID library
find_package(OpenCV REQUIRED)
add_library(${PROJECT_NAME} SHARED features2d.cpp
                                   lucid_functions.cpp
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})

# deal with the mex file
