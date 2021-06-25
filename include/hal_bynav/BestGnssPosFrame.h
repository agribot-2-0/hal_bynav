/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <hal_bynav/BynavFrame.h>

#include <string>
#include <vector>

struct BestGnssPosFrame : public BynavFrame
{
  BestGnssPosFrame() = default;
  ~BestGnssPosFrame() = default;
  BestGnssPosFrame(const BestGnssPosFrame& other) = default;
  BestGnssPosFrame(BestGnssPosFrame&& other) = default;
  BestGnssPosFrame& operator=(const BestGnssPosFrame& other) = default;
  BestGnssPosFrame& operator=(BestGnssPosFrame&& other) = default;

  void parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
             const IParsingStrategy& strategy) override;

  static constexpr uint64_t ID = { 1429 };

  // Main data
  SolutionStatus solution_status_;
  PositionStatus position_status_;
  double latitude_;
  double longitude_;
  double altitude_;
  float undulation_;
  uint32_t datum_id_;
  float latitude_std_;
  float longitude_std_;
  float altitude_std_;
  std::string station_id_;
  float differential_age_s;
  float solution_age_s;
  uint8_t tracked_satellites_n_;
  uint8_t solution_satellites_n_;
  uint8_t l1_solution_satellites_n_;
  uint8_t multi_solution_satellites_n_;
  ExtendedSolutionStatus extended_solution_status_;
  uint8_t galileo_beidou_signal_mask_;
  uint8_t gps_glonass_signal_mask_;
};
