/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/frame/HeadingFrame.h>

void HeadingFrame::deserialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it)
{
  // Parse and validate part common for all Bynav frames
  BynavFrame::deserialize(begin_it, end_it);

  // Check id validity
  if (message_id_ != ID)
  {
    // Shouldn't happen as frame creation is controlled with BynavFrameFactory, but double-check just in case
    throw std::runtime_error("[HeadingFrame] ID missmatch");
  }

  // Get iterator to the main part of the frame (after the header)
  auto main_data_begin_it = std::next(begin_it, header_length_);

  // Get iterator to the end of the frame, excluding CRC bytes
  auto end_without_crc_it = std::prev(end_it, CRC_LENGTH);

  // Check CRC
  crc_in_frame_ = strategy_.getUint32(std::next(main_data_begin_it, HeadingFrameIndex::Crc));
  auto crc_calculated = calculateCrc(begin_it, end_without_crc_it);

  if (crc_in_frame_ != crc_calculated)
  {
    throw std::runtime_error("[HeadingFrame] CRC missmatch. crc_in_frame_ = " + std::to_string(crc_in_frame_) +
                             " and crc_calculated = " + std::to_string(crc_calculated));
  }

  // Parse main data
  solution_status_ = static_cast<SolutionStatus>(
      strategy_.getUint32(std::next(main_data_begin_it, HeadingFrameIndex::SolutionStatus)));
  heading_status_ =
      static_cast<PositionStatus>(strategy_.getUint32(std::next(main_data_begin_it, HeadingFrameIndex::HeadingStatus)));
  baseline_length_ = strategy_.getFloat(std::next(main_data_begin_it, HeadingFrameIndex::BaselineLength));
  heading_ = bynavHeadingToRep103(strategy_.getFloat(std::next(main_data_begin_it, HeadingFrameIndex::Heading)));
  pitch_ = bynavPitchToRep103(strategy_.getFloat(std::next(main_data_begin_it, HeadingFrameIndex::Pitch)));
  heading_std_ =
      common_functions::radians(strategy_.getFloat(std::next(main_data_begin_it, HeadingFrameIndex::HeadingStd)));
  pitch_std_ =
      common_functions::radians(strategy_.getFloat(std::next(main_data_begin_it, HeadingFrameIndex::PitchStd)));

  tracked_satellites_n_ = strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::TrackedSatellitesN));
  solution_satellites_n_ = strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::SolutionSatellitesN));
  above_elevation_satellites_n_ =
      strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::AboveElevationSatellitesN));
  l2_above_elevation_satellites_n_ =
      strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::L2AboveElevationSatellitesN));
  solution_source_ =
      static_cast<SolutionSource>(strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::SolutionSource)));
  extended_solution_status_ = ExtendedSolutionStatus(
      strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::ExtendedSolutionStatus)));
  galileo_beidou_signal_mask_ =
      strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::GalileoBeidouSignalMask));
  gps_glonass_signal_mask_ = strategy_.getUint8(std::next(main_data_begin_it, HeadingFrameIndex::GpsGlonassSignalMask));
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
