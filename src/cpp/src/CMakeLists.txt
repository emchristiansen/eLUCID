add_definitions("-msse4.2")

# deal with the LUCID library
find_package(OpenCV REQUIRED)
add_library(lucid SHARED descriptor_extractor.cpp
                         elucid_binary_descriptor_extractor.cpp
                         elucid_descriptor_extractor.cpp
                         features2d.cpp
                         lucid_functions.cpp
                         util.cpp
)
target_link_libraries(lucid ${OpenCV_LIBRARIES})

# deal with the mex file
