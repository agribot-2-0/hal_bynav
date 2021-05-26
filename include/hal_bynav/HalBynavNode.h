/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

#include <string>

#include "agribot_common/CommonFunctions/ros_common_functions.h"
#include "agribot_msgs/BynavExtendedData.h"
#include "hal_bynav/Bynav.h"
#include "hal_bynav/BynavSocket.h"
#include "hal_bynav/BynavFrameParser.h"

class HalBynavNode
{
private:
  BynavSocket bynav_socket_;
  Bynav bynav_;

  ros::NodeHandle* nh_;
  ros::Publisher nav_sat_fix_publisher_;
  ros::Publisher heading_publisher_;
  ros::Publisher extended_data_publisher_;

  bool publish_extended_data_;
  std::string frame_id_;
  std::uint16_t port_;
  std::string ip_address_;

  int error_counter_ = 0;
  static constexpr int MAX_ERRORS_NUMBER = 10;

public:
  explicit HalBynavNode(ros::NodeHandle* nh);
  ~HalBynavNode();
  void run();
  void publishNavSatFix();
  void publishImu();
  void publishExtendedData();
};
