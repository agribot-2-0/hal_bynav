cmake_minimum_required(VERSION 3.0.2)
project(hal_bynav)

add_compile_options(-std=c++17 -Wall -Wextra -pedantic -Werror)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  agribot_common
  agribot_msgs
  sensor_msgs
  tf2_geometry_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES hal_bynav
#  CATKIN_DEPENDS roscpp
#  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${agribot_common_SOURCE_DIR}/include
)

add_executable(hal_bynav_node 
  src/hal_bynav_node.cpp
  src/HalBynavNode.cpp
  src/BynavSocket.cpp
  src/BynavFrameParser.cpp
  src/Bynav.cpp

  src/BynavParser.cpp
  src/BynavFrameFactory.cpp
  src/HeadingFrame.cpp
  src/BestGnssPosFrame.cpp
)
target_link_libraries(hal_bynav_node ${catkin_LIBRARIES})
add_dependencies(hal_bynav_node ${catkin_EXPORTED_TARGETS})

install(TARGETS
  hal_bynav_node
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

#############
## Testing ##
#############

if (CATKIN_ENABLE_TESTING)
  catkin_add_gtest(bynav-test
    test/bynav_test.cpp
    src/BynavFrameFactory.cpp
    src/HeadingFrame.cpp
    src/BestGnssPosFrame.cpp
  )
  target_link_libraries(bynav-test ${catkin_LIBRARIES} gtest)
endif()
