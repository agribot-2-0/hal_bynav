/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

HalBynavNode::HalBynavNode() : bynav_{}
{
  // TODO(olek) - get ip address and port from ros parameters
  std::string ip_address = "192.168.0.164";
  std::uint16_t port = 1111;

  // Connect socket
  bynav_socket_.connectSocket(ip_address, port);
}

HalBynavNode::~HalBynavNode()
{
}

void HalBynavNode::run()
{
  while (ros::ok())
  {
    try
    {
      auto frame_length = bynav_socket_.read();
      BynavFrameParser::parsePacket(bynav_, bynav_socket_.getBuffer(), frame_length);
      ROS_WARN_STREAM(bynav_);
      // publishNavSatFix();
      // publishImu();
    }
    catch (const std::exception& e)
    {
      ROS_ERROR_STREAM("[HalBynav]" << e.what());
    }

    ros::spinOnce();
  }
}
