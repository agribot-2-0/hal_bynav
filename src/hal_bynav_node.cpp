/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "hal_bynav_node");
  ros::NodeHandle nh;

  try
  {
    HalBynavNode hbn = HalBynavNode{};
    hbn.run();  // run shouldn't throw here anyway (exception is handled internally)
  }
  catch (const std::exception& e)
  {
    ROS_FATAL_STREAM(e.what());
    exit(-1);
  }
}
