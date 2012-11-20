ectomodule(lucid DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python2.7/dist-packages/
                      INSTALL
                      interfaces.cpp
                      LUCID.cpp
                      matcher.cpp
                      module.cpp
                      ransac_est.cpp
)

find_package(OpenCV REQUIRED)
link_ecto(lucid ${catkin_LIBRARIES}
                ${OpenCV_LIBRARIES}
                lucid
)

#For Hamming distance
set_target_properties(lucid_ectomodule PROPERTIES COMPILE_FLAGS "-msse4.2")
