/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <ros/ros.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/Imu.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

#include <memory>
#include <string>

#include <agribot_common/connection/Buffer.h>
#include <agribot_common/connection/IParser.h>
#include <agribot_common/connection/ISocket.h>

#include <hal_bynav/BestGnssPosFrame.h>
#include <hal_bynav/HeadingFrame.h>

#include "agribot_common/CommonFunctions/ros_common_functions.h"
#include "agribot_msgs/BynavGnssExtended.h"
#include "agribot_msgs/BynavHeadingExtended.h"

class HalBynavNode
{
private:
  ros::NodeHandle* nh_;
  ros::Publisher nav_sat_fix_publisher_;
  ros::Publisher heading_publisher_;
  ros::Publisher gnss_extended_publisher_;
  ros::Publisher heading_extended_publisher_;

  std::unique_ptr<IParser> parser_;
  std::unique_ptr<ISocket> socket_;
  std::unique_ptr<Buffer> buffer_;

  bool publish_extended_data_;
  bool print_debug_info_;
  std::string frame_id_;

  sensor_msgs::NavSatFix navSatFixMsgFromBestGnssPosFrame(const BestGnssPosFrame& frame);
  agribot_msgs::BynavGnssExtended bynavGnssExtendedMsgFromBestGnssPosFrame(const BestGnssPosFrame& frame);
  sensor_msgs::Imu imuMsgFromHeadingFrame(const HeadingFrame& frame);
  agribot_msgs::BynavHeadingExtended bynavHeadingExtendedMsgFromHeadingFrame(const HeadingFrame& frame);

public:
  explicit HalBynavNode(ros::NodeHandle* nh, std::unique_ptr<IParser> parser, std::unique_ptr<ISocket> socket,
                        std::unique_ptr<Buffer> buffer);
  void run();

  void publishNavSatFix(const IFrame& i_frame);
  void publishGnssExtended(const IFrame& i_frame);
  void publishImu(const IFrame& i_frame);
  void publishHeadingExtended(const IFrame& i_frame);

  void printBestGnssPosFrame(const IFrame& i_frame);
  void printHeadingFrame(const IFrame& i_frame);
};
