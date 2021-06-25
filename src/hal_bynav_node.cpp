/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

#include <hal_bynav/BynavParser.h>
#include <hal_bynav/BynavFrameFactory.h>
#include <agribot_common/connection/Buffer.h>
#include <agribot_common/connection/TcpSocket.h>
#include <agribot_common/connection/LsbFirstParsingStrategy.h>
#include "agribot_common/CommonFunctions/ros_common_functions.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "hal_bynav_node");
  ros::NodeHandle nh("~");

  // TODO(olek) - move timeout and buffer_size to ros parameters
  constexpr int BUFFER_SIZE = 1024;
  auto ip_address = ros_common_functions::getRosParam<std::string>("~ip_address");
  uint16_t port = ros_common_functions::getRosParam<int>("~port");
  uint16_t timeout_ms = 200;

  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_Factory = std::make_unique<BynavFrameFactory>();
  std::unique_ptr<ISocket> socket = std::make_unique<TcpSocket>(ip_address, port, timeout_ms);
  std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>(BUFFER_SIZE);
  std::unique_ptr<IParser> parser =
      std::make_unique<BynavParser>(std::move(parsing_strategy), std::move(frame_Factory));

  try
  {
    HalBynavNode hbn = HalBynavNode{ &nh, std::move(parser), std::move(socket), std::move(buffer) };
    hbn.run();
  }
  catch (const std::exception& e)
  {
    ROS_FATAL_STREAM(e.what());
    exit(-1);
  }
}
