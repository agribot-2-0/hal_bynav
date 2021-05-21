/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

HalBynavNode::HalBynavNode(ros::NodeHandle* nh) : bynav_{}, nh_{ nh }
{
  frame_id_ = ros_common_functions::getRosParam<std::string>("~frame_id");
  auto ip_address = ros_common_functions::getRosParam<std::string>("~ip_address");
  std::uint16_t port = ros_common_functions::getRosParam<int>("~port");

  bynav_socket_.connectSocket(ip_address, port);

  nav_sat_fix_publisher_ = nh_->advertise<sensor_msgs::NavSatFix>("gnss", 1);
  heading_publisher_ = nh_->advertise<sensor_msgs::Imu>("heading", 1);
}

HalBynavNode::~HalBynavNode()
{
}

void HalBynavNode::publishNavSatFix()
{
  // TODO(olek) - fill in status fields

  auto msg = sensor_msgs::NavSatFix{};
  msg.header.frame_id = frame_id_;
  msg.header.stamp = ros::Time::now();  // TODO(olek) - maybe use timestamp from GNSS receiver?

  if (bynav_.best_gnss_pos_solution_status_ == SolutionStatus::SolComputed)
  {
    // TODO(olek) - select proper PositionStatus for RTK
    switch (bynav_.best_gnss_pos_position_status_)
    {
      case PositionStatus::WideInt:
      case PositionStatus::NarrowInt:
        msg.status.status = msg.status.STATUS_GBAS_FIX;
        break;

      case PositionStatus::Single:
        msg.status.status = msg.status.STATUS_FIX;
        break;

      default:
        ROS_WARN_STREAM("best_gnss_pos_position_status_ = "
                        << "is not recognized. Defaulting to STATUS_NO_FIX");
        msg.status.status = msg.status.STATUS_NO_FIX;
        break;
    }
  }
  else
  {
    msg.status.status = msg.status.STATUS_NO_FIX;
  }

  msg.latitude = bynav_.latitude_;
  msg.longitude = bynav_.longitude_;
  msg.altitude = bynav_.height_;

  msg.position_covariance[0] = bynav_.latitude_std_ * bynav_.latitude_std_;
  msg.position_covariance[4] = bynav_.longitude_std_ * bynav_.longitude_std_;
  msg.position_covariance[8] = bynav_.height_std_ * bynav_.height_std_;
  msg.position_covariance_type = sensor_msgs::NavSatFix::COVARIANCE_TYPE_DIAGONAL_KNOWN;

  nav_sat_fix_publisher_.publish(msg);
}

void HalBynavNode::publishImu()
{
  // TODO(olek) - publish only if proper solution status and/or position status
  if (bynav_.heading_solution_status_ == SolutionStatus::SolComputed)
  {
    auto msg = sensor_msgs::Imu{};
    msg.header.frame_id = frame_id_;
    msg.header.stamp = ros::Time::now();  // TODO(olek) - maybe use timestamp from GNSS receiver?

    tf2::Quaternion quat;
    quat.setRPY(0.0, bynav_.pitch_, bynav_.yaw_);
    msg.orientation = tf2::toMsg(quat);

    msg.orientation_covariance[0] = 0.0;
    msg.orientation_covariance[4] = bynav_.pitch_std_ * bynav_.pitch_std_;
    msg.orientation_covariance[8] = bynav_.yaw_std_ * bynav_.yaw_std_;

    heading_publisher_.publish(msg);
  }
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
      publishNavSatFix();
      publishImu();
    }
    catch (const std::exception& e)
    {
      ROS_ERROR_STREAM("[HalBynav]" << e.what());
    }

    ros::spinOnce();
  }
}
