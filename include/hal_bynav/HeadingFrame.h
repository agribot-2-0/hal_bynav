/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <hal_bynav/BynavFrame.h>
#include <agribot_common/CommonFunctions/common_functions.h>

#include <string>
#include <vector>

namespace HeadingFrameIndex
{
enum
{
  SolutionStatus = 0,
  HeadingStatus = 4,
  BaselineLength = 8,
  Heading = 12,
  Pitch = 16,
  HeadingStd = 24,
  PitchStd = 28,
  StationIdBegin = 32,
  StationIdEnd = 36,
  TrackedSatellitesN = 36,
  SolutionSatellitesN = 37,
  AboveElevationSatellitesN = 38,
  L2AboveElevationSatellitesN = 39,
  SolutionSource = 40,
  ExtendedSolutionStatus = 41,
  GalileoBeidouSignalMask = 42,
  GpsGlonassSignalMask = 43,
  Crc = 44
};
}  // namespace HeadingFrameIndex

struct HeadingFrame : public BynavFrame
{
  HeadingFrame() = default;
  ~HeadingFrame() = default;
  HeadingFrame(const HeadingFrame& other) = default;
  HeadingFrame(HeadingFrame&& other) = default;
  HeadingFrame& operator=(const HeadingFrame& other) = default;
  HeadingFrame& operator=(HeadingFrame&& other) = default;

  static float bynavHeadingToRep103(float bynav_heading);
  static float bynavPitchToRep103(float bynav_pitch);

  void deserialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it) override;

  static constexpr uint64_t ID{ 971 };

  // Main data
  SolutionStatus solution_status_;
  PositionStatus heading_status_;
  float baseline_length_;
  float heading_;
  float pitch_;
  float heading_std_;
  float pitch_std_;
  std::string station_id_;
  uint8_t tracked_satellites_n_;
  uint8_t solution_satellites_n_;
  uint8_t above_elevation_satellites_n_;
  uint8_t l2_above_elevation_satellites_n_;
  SolutionSource solution_source_;
  ExtendedSolutionStatus extended_solution_status_;
  uint8_t galileo_beidou_signal_mask_;
  uint8_t gps_glonass_signal_mask_;
};
