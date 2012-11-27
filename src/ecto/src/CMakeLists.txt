ectomodule(ecto_lucid DESTINATION ./
                      INSTALL
                      interfaces.cpp
                      LUCID.cpp
                      matcher.cpp
                      module.cpp
                      ransac_est.cpp
)

find_package(OpenCV REQUIRED)
link_ecto(ecto_lucid ${catkin_LIBRARIES}
                ${OpenCV_LIBRARIES}
                lucid
)

#For Hamming distance
set_target_properties(ecto_lucid_ectomodule PROPERTIES COMPILE_FLAGS "-msse4.2")
