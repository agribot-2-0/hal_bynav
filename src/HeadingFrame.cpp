/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/HeadingFrame.h>

void HeadingFrame::parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                         const IParsingStrategy& strategy)
{
  // Parse and validate part common for all Bynav frames
  BynavFrame::parse(begin_it, end_it, strategy);

  // Check id validity
  if (message_id_ != ID)
  {
    // Shouldn't happen as frame creation is controlled with BynavFrameFactory, but double-check just in case
    // TODO(olek) - use custom exception?
    throw std::runtime_error("[HeadingFrame] ID missmatch");
  }

  // Get iterator to the main part of the frame (after the header)
  auto main_data_begin_it = std::next(begin_it, header_length_);

  // Get iterator to the end of the frame, excluding CRC bytes
  auto end_without_crc_it = std::prev(end_it, CRC_LENGTH);

  // Check CRC
  crc_in_frame_ = strategy.getUint32(std::next(main_data_begin_it, 44));
  auto crc_calculated = calculateCrc(begin_it, end_without_crc_it, strategy);

  if (crc_in_frame_ != crc_calculated)
  {
    throw std::runtime_error("[HeadingFrame] CRC missmatch. crc_in_frame_ = " + std::to_string(crc_in_frame_) +
                             " and crc_calculated = " + std::to_string(crc_calculated));
  }

  // Parse main data
  solution_status_ = static_cast<SolutionStatus>(strategy.getUint32(std::next(main_data_begin_it, 0)));
  heading_status_ = static_cast<PositionStatus>(strategy.getUint32(std::next(main_data_begin_it, 4)));
  baseline_length_ = strategy.getFloat(std::next(main_data_begin_it, 8));
  heading_ = bynavHeadingToRep103(strategy.getFloat(std::next(main_data_begin_it, 12)));
  pitch_ = bynavPitchToRep103(strategy.getFloat(std::next(main_data_begin_it, 16)));
  heading_std_ = common_functions::radians(strategy.getFloat(std::next(main_data_begin_it, 24)));
  pitch_std_ = common_functions::radians(strategy.getFloat(std::next(main_data_begin_it, 28)));
  station_id_ = strategy.getString(std::next(main_data_begin_it, 32), std::next(main_data_begin_it, 36));
  tracked_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 36));
  solution_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 37));
  above_elevation_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 38));
  l2_above_elevation_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 39));
  solution_source_ = static_cast<SolutionSource>(strategy.getUint8(std::next(main_data_begin_it, 40)));
  extended_solution_status_ = ExtendedSolutionStatus(strategy.getUint8(std::next(main_data_begin_it, 41)));
  galileo_beidou_signal_mask_ = strategy.getUint8(std::next(main_data_begin_it, 42));
  gps_glonass_signal_mask_ = strategy.getUint8(std::next(main_data_begin_it, 43));
}

float HeadingFrame::bynavHeadingToRep103(float bynav_heading)
{
  return -common_functions::radians(bynav_heading - 90.0f);
}

float HeadingFrame::bynavPitchToRep103(float bynav_pitch)
{
  // TODO(olek) - check if this is proper transformation
  return common_functions::radians(bynav_pitch);
}
