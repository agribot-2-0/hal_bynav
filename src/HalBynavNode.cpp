/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

HalBynavNode::HalBynavNode()
{
  // Connect socket
}

HalBynavNode::~HalBynavNode()
{
}

void HalBynavNode::run()
{
  ros::Rate loop_rate{ 10 };

  while (ros::ok)
  {
    // TODO - read

    ros::spinOnce();
    loop_rate.sleep();
  }
}
