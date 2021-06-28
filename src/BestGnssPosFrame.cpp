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
    // TODO(olek) - use custom exception?
    throw std::runtime_error("[BestGnssPosFrame] ID missmatch");
  }

  // Get iterator to the main part of the frame (after the header)
  auto main_data_begin_it = std::next(begin_it, header_length_);

  // Get iterator to the end of the frame, excluding CRC bytes
  auto end_without_crc_it = std::prev(end_it, CRC_LENGTH);

  // Check CRC
  crc_in_frame_ = strategy.getUint32(std::next(main_data_begin_it, 72));
  auto crc_calculated = calculateCrc(begin_it, end_without_crc_it, strategy);

  if (crc_in_frame_ != calculateCrc(begin_it, end_without_crc_it, strategy))
  {
    throw std::runtime_error("[HeadingFrame] CRC missmatch. crc_in_frame_ = " + std::to_string(crc_in_frame_) +
                             "and crc_calculated = " + std::to_string(crc_calculated));
  }

  // Parse main data
  solution_status_ = static_cast<SolutionStatus>(strategy.getUint32(std::next(main_data_begin_it, 0)));
  position_status_ = static_cast<PositionStatus>(strategy.getUint32(std::next(main_data_begin_it, 4)));
  latitude_ = strategy.getDouble(std::next(main_data_begin_it, 8));
  longitude_ = strategy.getDouble(std::next(main_data_begin_it, 16));
  altitude_ = strategy.getDouble(std::next(main_data_begin_it, 24));
  undulation_ = strategy.getFloat(std::next(main_data_begin_it, 32));
  datum_id_ = strategy.getUint32(std::next(main_data_begin_it, 36));
  latitude_std_ = strategy.getFloat(std::next(main_data_begin_it, 40));
  longitude_std_ = strategy.getFloat(std::next(main_data_begin_it, 44));
  altitude_std_ = strategy.getFloat(std::next(main_data_begin_it, 48));
  station_id_ = strategy.getString(std::next(main_data_begin_it, 52), std::next(main_data_begin_it, 56));
  differential_age_s = strategy.getFloat(std::next(main_data_begin_it, 56));
  solution_age_s = strategy.getFloat(std::next(main_data_begin_it, 60));
  tracked_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 64));
  solution_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 65));
  l1_solution_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 66));
  multi_solution_satellites_n_ = strategy.getUint8(std::next(main_data_begin_it, 67));
  extended_solution_status_ = ExtendedSolutionStatus(strategy.getUint8(std::next(main_data_begin_it, 69)));
  galileo_beidou_signal_mask_ = strategy.getUint8(std::next(main_data_begin_it, 70));
  gps_glonass_signal_mask_ = strategy.getUint8(std::next(main_data_begin_it, 71));
}
