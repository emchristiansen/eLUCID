add_definitions("-msse4.2")

find_package(OpenCV REQUIRED)
add_library(${PROJECT_NAME} features2d.cpp
                            lucid_functions.cpp
)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES})
