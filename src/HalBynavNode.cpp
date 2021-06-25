/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

HalBynavNode::HalBynavNode(ros::NodeHandle* nh, std::unique_ptr<IParser> parser, std::unique_ptr<ISocket> socket,
                           std::unique_ptr<Buffer> buffer)
  : nh_{ nh }, parser_{ std::move(parser) }, socket_{ std::move(socket) }, buffer_{ std::move(buffer) }
{
  frame_id_ = ros_common_functions::getRosParam<std::string>("~frame_id");
  publish_extended_data_ = ros_common_functions::getRosParam<bool>("~publish_extended_data");
  print_debug_info_ = ros_common_functions::getRosParam<bool>("~print_debug_info_");

  nav_sat_fix_publisher_ = nh_->advertise<sensor_msgs::NavSatFix>("gnss", 1);
  heading_publisher_ = nh_->advertise<sensor_msgs::Imu>("heading", 1);

  parser_->registerCallback(BestGnssPosFrame::ID,
                            std::bind(&HalBynavNode::publishNavSatFix, this, std::placeholders::_1));
  parser_->registerCallback(HeadingFrame::ID, std::bind(&HalBynavNode::publishImu, this, std::placeholders::_1));

  if (publish_extended_data_)
  {
    gnss_extended_publisher_ = nh_->advertise<agribot_msgs::BynavGnssExtended>("gnss/extended", 1);
    heading_extended_publisher_ = nh_->advertise<agribot_msgs::BynavHeadingExtended>("heading/extended", 1);

    parser_->registerCallback(BestGnssPosFrame::ID,
                              std::bind(&HalBynavNode::publishGnssExtended, this, std::placeholders::_1));
    parser_->registerCallback(HeadingFrame::ID,
                              std::bind(&HalBynavNode::publishHeadingExtended, this, std::placeholders::_1));
  }

  if (print_debug_info_)
  {
    parser_->registerCallback(BestGnssPosFrame::ID,
                              std::bind(&HalBynavNode::printBestGnssPosFrame, this, std::placeholders::_1));
    parser_->registerCallback(HeadingFrame::ID,
                              std::bind(&HalBynavNode::printHeadingFrame, this, std::placeholders::_1));
  }
}

void HalBynavNode::publishNavSatFix(const IFrame& iframe)
{
  auto frame = dynamic_cast<const BestGnssPosFrame*>(&iframe);

  auto msg = navSatFixMsgFromBestGnssPosFrame(*frame);
  msg.header.frame_id = frame_id_;
  msg.header.stamp = ros::Time::now();

  nav_sat_fix_publisher_.publish(msg);
}

void HalBynavNode::publishGnssExtended(const IFrame& iframe)
{
  auto frame = dynamic_cast<const BestGnssPosFrame*>(&iframe);
  auto msg = bynavGnssExtendedMsgFromBestGnssPosFrame(*frame);
  nav_sat_fix_publisher_.publish(msg);
}

void HalBynavNode::publishImu(const IFrame& iframe)
{
  auto frame = dynamic_cast<const HeadingFrame*>(&iframe);

  auto msg = imuMsgFromHeadingFrame(*frame);
  msg.header.frame_id = frame_id_;
  msg.header.stamp = ros::Time::now();

  heading_publisher_.publish(msg);
}

void HalBynavNode::publishHeadingExtended(const IFrame& iframe)
{
  auto frame = dynamic_cast<const HeadingFrame*>(&iframe);
  auto msg = bynavHeadingExtendedMsgFromHeadingFrame(*frame);
  nav_sat_fix_publisher_.publish(msg);
}

void HalBynavNode::printBestGnssPosFrame(const IFrame& iframe)
{
  auto frame = dynamic_cast<const BestGnssPosFrame*>(&iframe);

  // TODO(olek) - print other data
  ROS_WARN_STREAM("[HalBynavNode] Latitude: " << frame->latitude_);
}

void HalBynavNode::printHeadingFrame(const IFrame& iframe)
{
  auto frame = dynamic_cast<const HeadingFrame*>(&iframe);

  // TODO(olek) - print other data
  ROS_WARN_STREAM("[HalBynavNode] Baseline length: " << frame->baseline_length_);
}

sensor_msgs::NavSatFix HalBynavNode::navSatFixMsgFromBestGnssPosFrame(const BestGnssPosFrame& frame)
{
  sensor_msgs::NavSatFix msg;

  // TODO(olek) - fill in other data
  msg.latitude = frame.latitude_;

  return msg;
}

agribot_msgs::BynavGnssExtended HalBynavNode::bynavGnssExtendedMsgFromBestGnssPosFrame(const BestGnssPosFrame& frame)
{
  agribot_msgs::BynavGnssExtended msg;

  // TODO(olek) - fill in other data
  static_cast<void>(frame);

  return msg;
}

sensor_msgs::Imu HalBynavNode::imuMsgFromHeadingFrame(const HeadingFrame& frame)
{
  sensor_msgs::Imu msg;

  // TODO(olek) - fill in other data
  static_cast<void>(frame);

  return msg;
}

agribot_msgs::BynavHeadingExtended HalBynavNode::bynavHeadingExtendedMsgFromHeadingFrame(const HeadingFrame& frame)
{
  agribot_msgs::BynavHeadingExtended msg;

  // TODO(olek) - fill in other data
  static_cast<void>(frame);

  return msg;
}

void HalBynavNode::run()
{
  while (ros::ok())
  {
    // TODO(olek) - add exception handling

    socket_->read(*buffer_);
    parser_->getFrameBuffersFromPacketBuffer(*buffer_);

    while (parser_->haveFrameBufferToParse())
    {
      // TODO(olek) - add exception handling for parse frame
      parser_->parseFrameBuffer();
    }

    ros::spinOnce();
  }
}
