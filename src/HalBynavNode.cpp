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
  print_debug_info_ = ros_common_functions::getRosParam<bool>("~print_debug_info");

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

void HalBynavNode::publishNavSatFix(const IFrame& iframe) const
{
  auto frame = dynamic_cast<const BestGnssPosFrame*>(&iframe);

  auto msg = navSatFixMsgFromBestGnssPosFrame(*frame);
  msg.header.frame_id = frame_id_;
  msg.header.stamp = ros::Time::now();

  nav_sat_fix_publisher_.publish(msg);
}

void HalBynavNode::publishGnssExtended(const IFrame& iframe) const
{
  auto frame = dynamic_cast<const BestGnssPosFrame*>(&iframe);
  auto msg = bynavGnssExtendedMsgFromBestGnssPosFrame(*frame);
  gnss_extended_publisher_.publish(msg);
}

void HalBynavNode::publishImu(const IFrame& iframe) const
{
  auto frame = dynamic_cast<const HeadingFrame*>(&iframe);

  if (frame->solution_status_ != SolutionStatus::SolComputed)
  {
    ROS_WARN_STREAM_THROTTLE(5.0, "[HalBynavNode][HeadingFrame] Invalid solution_status_ = "
                                      << static_cast<int>(frame->solution_status_) << ". Skipping Imu publication...");
    return;
  }

  if (frame->heading_status_ != PositionStatus::WideInt && frame->heading_status_ != PositionStatus::NarrowInt &&
      frame->heading_status_ != PositionStatus::Psrdiff && frame->heading_status_ != PositionStatus::NarrowFloat &&
      frame->heading_status_ != PositionStatus::Single)
  {
    ROS_WARN_STREAM_THROTTLE(5.0, "[HalBynavNode][HeadingFrame] Invalid heading_status_ = "
                                      << static_cast<int>(frame->heading_status_) << ". Skipping Imu publication...");
    return;
  }

  auto msg = imuMsgFromHeadingFrame(*frame);
  msg.header.frame_id = frame_id_;
  msg.header.stamp = ros::Time::now();
  heading_publisher_.publish(msg);
}

void HalBynavNode::publishHeadingExtended(const IFrame& iframe) const
{
  auto frame = dynamic_cast<const HeadingFrame*>(&iframe);
  auto msg = bynavHeadingExtendedMsgFromHeadingFrame(*frame);
  heading_extended_publisher_.publish(msg);
}

void HalBynavNode::printBestGnssPosFrame(const IFrame& iframe) const
{
  auto frame = dynamic_cast<const BestGnssPosFrame*>(&iframe);

  ROS_WARN_STREAM("[HalBynavNode][BestGnssPosFrame]"
                  << std::endl
                  << "Solution status: " << static_cast<int>(frame->solution_status_) << std::endl
                  << "Position status: " << static_cast<int>(frame->position_status_) << std::endl
                  << "Latitude: " << frame->latitude_ << std::endl
                  << "Longitude: " << frame->longitude_ << std::endl
                  << "Undulation: " << frame->undulation_ << std::endl
                  << "Datum ID: " << frame->datum_id_ << std::endl
                  << "Latitude std: " << frame->latitude_std_ << std::endl
                  << "Longitude std: " << frame->longitude_std_ << std::endl
                  << "Altitude std: " << frame->altitude_std_ << std::endl
                  << "Station ID: " << frame->station_id_ << std::endl
                  << "Differential age [s]: " << frame->differential_age_s << std::endl
                  << "Solution age [s]: " << frame->solution_age_s << std::endl
                  << "Tracked satellites: " << frame->tracked_satellites_n_ << std::endl
                  << "Solution satellites: " << frame->solution_satellites_n_ << std::endl
                  << "L1 solution satellites: " << frame->l1_solution_satellites_n_ << std::endl
                  << "Multifreq solution satellites: " << frame->multi_solution_satellites_n_ << std::endl
                  << " === Extended solution status === " << std::endl
                  << "\tRTK solution verified: " << frame->extended_solution_status_.rtk_solution_verified_ << std::endl
                  << "\tIono correction: " << frame->extended_solution_status_.iono_correction_ << std::endl
                  << "\tRTK assist active: " << frame->extended_solution_status_.rtk_assist_active_ << std::endl
                  << "\tAntenna status: " << frame->extended_solution_status_.antenna_status_ << std::endl
                  << " ================================ " << std::endl
                  << "Galileo & Beidou signal mask: " << frame->galileo_beidou_signal_mask_ << std::endl
                  << "GPS & Glonass signal mask: " << frame->gps_glonass_signal_mask_ << std::endl);
}

void HalBynavNode::printHeadingFrame(const IFrame& iframe) const
{
  auto frame = dynamic_cast<const HeadingFrame*>(&iframe);

  ROS_WARN_STREAM("[HalBynavNode][HeadingFrame]"
                  << std::endl
                  << "Solution status: " << static_cast<int>(frame->solution_status_) << std::endl
                  << "Heading status: " << static_cast<int>(frame->heading_status_) << std::endl
                  << "Baseline length: " << frame->baseline_length_ << std::endl
                  << "Heading: " << frame->heading_ << std::endl
                  << "Pitch: " << frame->pitch_ << std::endl
                  << "Heading std: " << frame->heading_std_ << std::endl
                  << "Pitch std: " << frame->pitch_std_ << std::endl
                  << "Station ID: " << frame->station_id_ << std::endl
                  << "Tracked satellites: " << frame->tracked_satellites_n_ << std::endl
                  << "Solution satellites: " << frame->solution_satellites_n_ << std::endl
                  << "Above elevation satellites: " << frame->above_elevation_satellites_n_ << std::endl
                  << "L2 Above elevation satellites: " << frame->l2_above_elevation_satellites_n_ << std::endl
                  << "Solution source: " << static_cast<int>(frame->solution_source_) << std::endl
                  << " === Extended solution status === " << std::endl
                  << "\tRTK solution verified: " << frame->extended_solution_status_.rtk_solution_verified_ << std::endl
                  << "\tIono correction: " << frame->extended_solution_status_.iono_correction_ << std::endl
                  << "\tRTK assist active: " << frame->extended_solution_status_.rtk_assist_active_ << std::endl
                  << "\tAntenna status: " << frame->extended_solution_status_.antenna_status_ << std::endl
                  << " ================================ " << std::endl
                  << "Galileo & Beidou signal mask: " << frame->galileo_beidou_signal_mask_ << std::endl
                  << "GPS & Glonass signal mask: " << frame->gps_glonass_signal_mask_ << std::endl);
}

sensor_msgs::NavSatFix HalBynavNode::navSatFixMsgFromBestGnssPosFrame(const BestGnssPosFrame& frame) const
{
  sensor_msgs::NavSatFix msg;

  if (frame.solution_status_ == SolutionStatus::SolComputed)
  {
    switch (frame.position_status_)
    {
      case PositionStatus::WideInt:
      case PositionStatus::NarrowInt:
      case PositionStatus::Psrdiff:
      case PositionStatus::NarrowFloat:
        msg.status.status = msg.status.STATUS_GBAS_FIX;
        break;

      case PositionStatus::Single:
        msg.status.status = msg.status.STATUS_FIX;
        break;

      case PositionStatus::None:
        msg.status.status = msg.status.STATUS_NO_FIX;
        break;

      default:
        ROS_WARN_STREAM_THROTTLE(1.0, "[HalBynavNode][BestGnssPosFrame] position_status_ = "
                                          << static_cast<int>(frame.position_status_)
                                          << " is not recognized. Defaulting to STATUS_NO_FIX");
        msg.status.status = msg.status.STATUS_NO_FIX;
        break;
    }
  }
  else
  {
    msg.status.status = msg.status.STATUS_NO_FIX;
  }

  msg.latitude = frame.latitude_;
  msg.longitude = frame.longitude_;
  msg.altitude = frame.altitude_;

  msg.position_covariance[0] = frame.latitude_std_ * frame.latitude_std_;
  msg.position_covariance[4] = frame.longitude_std_ * frame.longitude_std_;
  msg.position_covariance[8] = frame.altitude_std_ * frame.altitude_std_;
  msg.position_covariance_type = sensor_msgs::NavSatFix::COVARIANCE_TYPE_DIAGONAL_KNOWN;

  return msg;
}

agribot_msgs::BynavGnssExtended
HalBynavNode::bynavGnssExtendedMsgFromBestGnssPosFrame(const BestGnssPosFrame& frame) const
{
  agribot_msgs::BynavGnssExtended msg;

  msg.solution_status = static_cast<std::uint32_t>(frame.solution_status_);
  msg.position_status = static_cast<std::uint32_t>(frame.position_status_);
  msg.tracked_satellites_n = frame.tracked_satellites_n_;
  msg.solution_satellites_n = frame.solution_satellites_n_;

  msg.differential_age = frame.differential_age_s;
  msg.solution_age = frame.solution_age_s;

  msg.extended_solution_status.rtk_assist_active = frame.extended_solution_status_.rtk_assist_active_;
  msg.extended_solution_status.iono_correction = frame.extended_solution_status_.iono_correction_;
  msg.extended_solution_status.rtk_solution_verified = frame.extended_solution_status_.rtk_solution_verified_;
  msg.extended_solution_status.antenna_status = frame.extended_solution_status_.antenna_status_;

  return msg;
}

sensor_msgs::Imu HalBynavNode::imuMsgFromHeadingFrame(const HeadingFrame& frame) const
{
  sensor_msgs::Imu msg;

  msg.header.frame_id = frame_id_;
  msg.header.stamp = ros::Time::now();

  tf2::Quaternion quat;
  quat.setRPY(0.0, frame.pitch_, frame.heading_);
  msg.orientation = tf2::toMsg(quat);

  msg.orientation_covariance[0] = 0.0;
  msg.orientation_covariance[4] = frame.pitch_std_ * frame.pitch_std_;
  msg.orientation_covariance[8] = frame.heading_std_ * frame.heading_std_;

  return msg;
}

agribot_msgs::BynavHeadingExtended
HalBynavNode::bynavHeadingExtendedMsgFromHeadingFrame(const HeadingFrame& frame) const
{
  agribot_msgs::BynavHeadingExtended msg;

  msg.solution_status = static_cast<std::uint32_t>(frame.solution_status_);
  msg.position_status = static_cast<std::uint32_t>(frame.heading_status_);
  msg.solution_source = static_cast<std::uint8_t>(frame.solution_source_);
  msg.tracked_satellites_n = frame.tracked_satellites_n_;
  msg.solution_satellites_n = frame.solution_satellites_n_;
  msg.baseline_length = frame.baseline_length_;

  msg.extended_solution_status.rtk_assist_active = frame.extended_solution_status_.rtk_assist_active_;
  msg.extended_solution_status.iono_correction = frame.extended_solution_status_.iono_correction_;
  msg.extended_solution_status.rtk_solution_verified = frame.extended_solution_status_.rtk_solution_verified_;
  msg.extended_solution_status.antenna_status = frame.extended_solution_status_.antenna_status_;

  return msg;
}

void HalBynavNode::connectSocket() const
{
  bool connected = false;

  try
  {
    socket_->open();
    socket_->connect();
    connected = true;
  }
  catch (const std::exception& e)
  {
    ROS_WARN_STREAM("[HalBynavNode] " << e.what() << ". Retrying...");
    ros::Duration(1.0).sleep();
  }

  if (!connected)
  {
    reconnectSocket();
  }

  ROS_WARN_STREAM("[HalBynavNode] Socket connected!");
}

void HalBynavNode::reconnectSocket() const
{
  bool connected = false;

  while (!connected)
  {
    try
    {
      socket_->reconnect();
      connected = true;
    }
    catch (const std::exception& e)
    {
      ROS_WARN_STREAM("[HalBynavNode] " << e.what() << ". Retrying...");
      ros::Duration(1.0).sleep();
    }
  }
  ROS_WARN_STREAM("[HalBynavNode] Socket reconnected!");
}

void HalBynavNode::run()
{
  connectSocket();

  while (ros::ok())
  {
    try
    {
      socket_->read(*buffer_);
      socket_read_errors_n_ = 0;
    }
    catch (const std::exception& e)
    {
      ROS_ERROR_STREAM(e.what());
      socket_read_errors_n_++;

      if (socket_read_errors_n_ >= MAX_SOCKET_READ_ERRORS_N_)
      {
        ROS_ERROR_STREAM("[HalBynavNode] Socket connection lost. Trying to reconnect...");
        reconnectSocket();
        socket_read_errors_n_ = 0;
      }
    }

    parser_->getFrameBuffersFromPacketBuffer(*buffer_);

    while (parser_->haveFrameBufferToParse())
    {
      try
      {
        parser_->parseFrameBuffer();
      }
      catch (const std::exception& e)
      {
        ROS_ERROR_STREAM("[HalBynavNode] " << e.what());
      }
    }

    ros::spinOnce();
  }
}
