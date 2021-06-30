/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/BestGnssPosFrame.h>

void BestGnssPosFrame::parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                             const IParsingStrategy& strategy)
{
  // Parse and validate part common for all Bynav frames
  BynavFrame::parse(begin_it, end_it, strategy);

  // Check id validity
  if (message_id_ != ID)
  {
    // Shouldn't happen as frame creation is controlled with BynavFrameFactory, but double-check just in case
    throw std::runtime_error("[BestGnssPosFrame] ID missmatch");
  }

  // Get iterator to the main part of the frame (after the header)
  auto main_data_begin_it = std::next(begin_it, header_length_);

  // Get iterator to the end of the frame, excluding CRC bytes
  auto end_without_crc_it = std::prev(end_it, CRC_LENGTH);

  // Check CRC
  crc_in_frame_ = strategy.getUint32(std::next(main_data_begin_it, BestGnssPosFrameIndex::Crc));
  auto crc_calculated = calculateCrc(begin_it, end_without_crc_it, strategy);

  if (crc_in_frame_ != calculateCrc(begin_it, end_without_crc_it, strategy))
  {
    throw std::runtime_error("[HeadingFrame] CRC missmatch. crc_in_frame_ = " + std::to_string(crc_in_frame_) +
                             " and crc_calculated = " + std::to_string(crc_calculated));
  }

  // Parse main data
  solution_status_ = static_cast<SolutionStatus>(
      strategy.getUint32(std::next(main_data_begin_it, BestGnssPosFrameIndex::SolutionStatus)));
  position_status_ = static_cast<PositionStatus>(
      strategy.getUint32(std::next(main_data_begin_it, BestGnssPosFrameIndex::PositionStatus)));
  latitude_ = strategy.getDouble(std::next(main_data_begin_it, BestGnssPosFrameIndex::Latitude));
  longitude_ = strategy.getDouble(std::next(main_data_begin_it, BestGnssPosFrameIndex::Longitude));
  altitude_ = strategy.getDouble(std::next(main_data_begin_it, BestGnssPosFrameIndex::Altitude));
  undulation_ = strategy.getFloat(std::next(main_data_begin_it, BestGnssPosFrameIndex::Undulation));
  datum_id_ = strategy.getUint32(std::next(main_data_begin_it, BestGnssPosFrameIndex::DatumId));
  latitude_std_ = strategy.getFloat(std::next(main_data_begin_it, BestGnssPosFrameIndex::LatitudeStd));
  longitude_std_ = strategy.getFloat(std::next(main_data_begin_it, BestGnssPosFrameIndex::LongitudeStd));
  altitude_std_ = strategy.getFloat(std::next(main_data_begin_it, BestGnssPosFrameIndex::AltitudeStd));
  station_id_ = strategy.getString(std::next(main_data_begin_it, BestGnssPosFrameIndex::StationIdBegin),
                                   std::next(main_data_begin_it, BestGnssPosFrameIndex::StationIdEnd));
  differential_age_s = strategy.getFloat(std::next(main_data_begin_it, BestGnssPosFrameIndex::DifferentialAge));
  solution_age_s = strategy.getFloat(std::next(main_data_begin_it, BestGnssPosFrameIndex::SolutionAge));
  tracked_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::TrackedSatellitesN));
  solution_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::SolutionSatellitesN));
  l1_solution_satellites_n_ =
      strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::L1SolutionSatellitesN));
  multi_solution_satellites_n_ =
      strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::MultiSolutionSatellitesN));
  extended_solution_status_ = ExtendedSolutionStatus(
      strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::ExtendedSolutionStatus)));
  galileo_beidou_signal_mask_ =
      strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::GalileoBeidouSignalMask));
  gps_glonass_signal_mask_ =
      strategy.getUint8(std::next(main_data_begin_it, BestGnssPosFrameIndex::GpsGlonassSignalMask));
}
