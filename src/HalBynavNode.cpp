/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/HalBynavNode.h"

HalBynavNode::HalBynavNode(ros::NodeHandle* nh) : bynav_{}, nh_{ nh }
{
  frame_id_ = ros_common_functions::getRosParam<std::string>("~frame_id");
  ip_address_ = ros_common_functions::getRosParam<std::string>("~ip_address");
  port_ = ros_common_functions::getRosParam<int>("~port");
  publish_extended_data_ = ros_common_functions::getRosParam<bool>("~publish_extended_data");

  bynav_socket_.createSocket();
  bynav_socket_.connectSocket(ip_address_, port_);

  nav_sat_fix_publisher_ = nh_->advertise<sensor_msgs::NavSatFix>("gnss", 1);
  heading_publisher_ = nh_->advertise<sensor_msgs::Imu>("heading", 1);

  if (publish_extended_data_)
  {
    gnss_extended_publisher_ = nh_->advertise<agribot_msgs::BynavGnssExtended>("gnss/extended", 1);
    heading_extended_publisher_ = nh_->advertise<agribot_msgs::BynavHeadingExtended>("heading/extended", 1);
  }
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
        ROS_WARN_STREAM("best_gnss_pos_position_status_ = " << static_cast<int>(bynav_.best_gnss_pos_position_status_)
                                                            << " is not recognized. Defaulting to STATUS_NO_FIX");
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

void HalBynavNode::publishGnssExtended()
{
  auto msg = agribot_msgs::BynavGnssExtended();

  msg.solution_status = static_cast<std::uint32_t>(bynav_.best_gnss_pos_solution_status_);
  msg.position_status = static_cast<std::uint32_t>(bynav_.best_gnss_pos_position_status_);
  msg.tracked_satellites_n = bynav_.best_gnss_pos_tracked_satellites_n_;
  msg.solution_satellites_n = bynav_.best_gnss_pos_solution_satellites_n_;

  msg.differential_age = bynav_.differential_age_;
  msg.solution_age = bynav_.solution_age_;

  msg.extended_solution_status.rtk_assist_active = bynav_.heading_extended_solution_status_.rtk_assist_active_;
  msg.extended_solution_status.iono_correction = bynav_.heading_extended_solution_status_.iono_correction_;
  msg.extended_solution_status.rtk_solution_verified =
      bynav_.heading_extended_solution_status_.rtk_solution_verified_;
  msg.extended_solution_status.antenna_status = bynav_.heading_extended_solution_status_.antenna_status_;

  gnss_extended_publisher_.publish(msg);
}

void HalBynavNode::publishHeadingExtended()
{
  auto msg = agribot_msgs::BynavHeadingExtended();

  msg.solution_status = static_cast<std::uint32_t>(bynav_.heading_solution_status_);
  msg.position_status = static_cast<std::uint32_t>(bynav_.heading_position_status_);
  msg.solution_source = static_cast<std::uint8_t>(bynav_.heading_solution_source_);
  msg.tracked_satellites_n = bynav_.heading_tracked_satellites_n_;
  msg.solution_satellites_n = bynav_.heading_solution_satellites_n_;
  msg.baseline_length = bynav_.heading_baseline_length_;

  msg.extended_solution_status.rtk_assist_active = bynav_.best_gnss_pos_extended_solution_status_.rtk_assist_active_;
  msg.extended_solution_status.iono_correction = bynav_.best_gnss_pos_extended_solution_status_.iono_correction_;
  msg.extended_solution_status.rtk_solution_verified =
      bynav_.best_gnss_pos_extended_solution_status_.rtk_solution_verified_;
  msg.extended_solution_status.antenna_status = bynav_.best_gnss_pos_extended_solution_status_.antenna_status_;

  heading_extended_publisher_.publish(msg);
}

void HalBynavNode::run()
{
  while (ros::ok())
  {
    if (error_counter_ == MAX_ERRORS_NUMBER)
    {
      ROS_WARN_STREAM("[HalBynav] Reconnecting socket...");

      bool reconnect_success = false;
      while (!reconnect_success && ros::ok())
      {
        try
        {
          bynav_socket_.reconnectSocket(ip_address_, port_);
          reconnect_success = true;
        }
        catch (const std::exception& e)
        {
          ROS_ERROR_STREAM("[HalBynav] Reconnection failed");
          ros::Duration(1.0).sleep();
        }
      }

      ROS_WARN_STREAM("[HalBynav] Socket reconnected...");
      error_counter_ = 0;
    }

    try
    {
      auto frame_length = bynav_socket_.read();
      BynavFrameParser::parsePacket(bynav_, bynav_socket_.getBuffer(), frame_length);
      publishNavSatFix();
      publishImu();

      if (publish_extended_data_)
      {
        publishGnssExtended();
        publishHeadingExtended();
      }

      error_counter_ = 0;
    }
    catch (const std::exception& e)
    {
      ROS_ERROR_STREAM("[HalBynav]" << e.what());
      error_counter_++;
    }

    ros::spinOnce();
  }
}
