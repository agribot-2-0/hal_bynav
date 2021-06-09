/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/BynavFrameParser.h"

namespace BynavFrameParser
{
// Maybe change name to something like "updateFromFrame?"
// Or create config structs for various frames we want to receive?
void parsePacket(Bynav& bynav, std::uint8_t* packet_buffer, std::size_t packet_length)  // NOLINT(runtime/references)
{
  auto frames = findFrames(packet_buffer, packet_length);

  for (const auto& [frame_index, frame_length] : frames)
  {
    parseFrame(bynav, packet_buffer + frame_index, frame_length);
  }
}

std::vector<std::pair<int, std::size_t>> findFrames(std::uint8_t* packet_buffer, std::size_t packet_length)
{
  // Find all starting indexes for all frames in packet
  std::vector<int> frame_start_indexes;
  uint8_t* search_result = packet_buffer;  // Set to starting byte to avoid it being unitialized
  uint8_t* search_position = packet_buffer;
  uint8_t* end_position = packet_buffer + packet_length - 1;

  while (search_result != end_position)
  {
    search_result = std::search(search_position, end_position, std::begin(sync_bytes), std::end(sync_bytes));

    if (search_result != end_position)
    {
      frame_start_indexes.push_back(std::distance(packet_buffer, search_result));
      search_position = search_result + 1;
    }
  }

  // Create [frame_index, frame_length] pair for all found indexes
  std::vector<std::pair<int, std::size_t>> frames;

  if (frame_start_indexes.size() > 0)
  {
    for (std::size_t i = 0; i < frame_start_indexes.size() - 1; i++)
    {
      std::size_t frame_length = frame_start_indexes[i + 1] - frame_start_indexes[i];
      frames.push_back(std::make_pair(frame_start_indexes[i], frame_length));
    }

    // Push last frame (special case, because it doesn't have ending SyncBytes)
    auto last = frame_start_indexes.size() - 1;
    std::size_t frame_length = packet_length - frame_start_indexes[last];
    frames.push_back(std::make_pair(frame_start_indexes[last], frame_length));
  }

  return frames;
}

void parseFrame(Bynav& bynav, uint8_t* frame_buffer, std::size_t frame_length)  // NOLINT(runtime/references)
{
  checkCrc(frame_buffer, frame_length);

  auto header = parseHeader(frame_buffer);

  if (frame_length != header.frame_length_)
  {
    throw std::runtime_error("Frame length missmatch");
  }

  switch (header.message_id_)
  {
    case MessageId::Heading:
      parseHeadingMessage(bynav, frame_buffer + header.header_length_, header.message_length_);
      break;

    case MessageId::BestGnssPos:
      parseBestGnssPosMessage(bynav, frame_buffer + header.header_length_, header.message_length_);
      break;

    default:
      throw std::runtime_error("Message ID = " + std::to_string(static_cast<int>(header.message_id_)) +
                               " is not supported by parser");
      break;
  }
}

Header parseHeader(std::uint8_t* frame_buffer)
{
  Header header;

  header.header_length_ = frame_buffer[3];
  header.message_id_ = static_cast<MessageId>(combineTwoBytes(frame_buffer[4], frame_buffer[5]));
  header.message_length_ = combineTwoBytes(frame_buffer[8], frame_buffer[9]);
  header.frame_length_ = header.header_length_ + header.message_length_ + crc_length;
  return header;
}

void checkCrc(std::uint8_t* frame_buffer, std::size_t frame_length_with_crc)
{
  auto crc_in_frame =
      combineFourBytes(frame_buffer[frame_length_with_crc - 4], frame_buffer[frame_length_with_crc - 3],
                       frame_buffer[frame_length_with_crc - 2], frame_buffer[frame_length_with_crc - 1]);

  auto calculated_crc = calculateCrcForFrame(frame_buffer, frame_length_with_crc - crc_length);

  if (crc_in_frame != calculated_crc)
  {
    throw std::runtime_error("CRC mismatch");
  }
}

std::uint32_t calculateCrcForFrame(std::uint8_t* frame_buffer, std::size_t frame_length)
{
  std::uint64_t crc = 0;

  for (std::size_t i = 0; i < frame_length; i++)
  {
    auto tmp1 = (crc >> 8) & 0x00FFFFFFL;
    auto tmp2 = calculateCrcForUint64((crc ^ frame_buffer[i]) & 0xFF);
    crc = tmp1 ^ tmp2;
  }
  return static_cast<std::uint32_t>(crc);
}

std::uint64_t calculateCrcForUint64(std::uint64_t value)
{
  std::uint64_t crc = value;

  for (std::size_t i = 8; i > 0; --i)
  {
    if (crc & 1)
    {
      crc = (crc >> 1) ^ crc32_polynomial;
    }
    else
    {
      crc >>= 1;
    }
  }
  return crc;
}

void parseHeadingMessage(Bynav& bynav, std::uint8_t* message_buffer,  // NOLINT(runtime/references)
                         std::size_t message_length)
{
  if (message_length != heading_message_length_)
  {
    throw std::runtime_error("Heading message length missmatch. Should be " + std::to_string(heading_message_length_) +
                             ", but is " + std::to_string(message_length));
  }

  bynav.heading_solution_status_ = static_cast<SolutionStatus>(getUint32FromBuffer(&message_buffer[0]));
  bynav.heading_position_status_ = static_cast<PositionStatus>(getUint32FromBuffer(&message_buffer[4]));
  bynav.heading_baseline_length_ = getFloatFromBuffer(&message_buffer[8]);
  bynav.yaw_ = bynavYawToRep103(getFloatFromBuffer(&message_buffer[12]));
  bynav.pitch_ = bynavPitchToRep103(getFloatFromBuffer(&message_buffer[16]));
  bynav.yaw_std_ = common_functions::radians(getFloatFromBuffer(&message_buffer[24]));
  bynav.pitch_std_ = common_functions::radians(getFloatFromBuffer(&message_buffer[28]));
  bynav.heading_tracked_satellites_n_ = message_buffer[36];
  bynav.heading_solution_satellites_n_ = message_buffer[37];
  bynav.heading_solution_source_ = static_cast<SolutionSource>(message_buffer[40]);
  bynav.heading_extended_solution_status_ = getExtendedSolutionStatusFromUint8(message_buffer[41]);
}

void parseBestGnssPosMessage(Bynav& bynav, std::uint8_t* message_buffer,  // NOLINT(runtime/references)
                             std::size_t message_length)
{
  if (message_length != best_gnss_pos_message_length_)
  {
    throw std::runtime_error("BestGnssPos message length missmatch. Should be " +
                             std::to_string(best_gnss_pos_message_length_) + ", but is " +
                             std::to_string(message_length));
  }

  bynav.best_gnss_pos_solution_status_ = static_cast<SolutionStatus>(getUint32FromBuffer(&message_buffer[0]));
  bynav.best_gnss_pos_position_status_ = static_cast<PositionStatus>(getUint32FromBuffer(&message_buffer[4]));
  bynav.latitude_ = getDoubleFromBuffer(&message_buffer[8]);
  bynav.longitude_ = getDoubleFromBuffer(&message_buffer[16]);
  bynav.height_ = getDoubleFromBuffer(&message_buffer[24]);
  bynav.latitude_std_ = getFloatFromBuffer(&message_buffer[40]);
  bynav.longitude_std_ = getFloatFromBuffer(&message_buffer[44]);
  bynav.height_std_ = getFloatFromBuffer(&message_buffer[48]);
  bynav.differential_age_ = getFloatFromBuffer(&message_buffer[56]);
  bynav.solution_age_ = getFloatFromBuffer(&message_buffer[60]);
  bynav.best_gnss_pos_tracked_satellites_n_ = message_buffer[64];
  bynav.best_gnss_pos_solution_satellites_n_ = message_buffer[65];
  bynav.best_gnss_pos_extended_solution_status_ = getExtendedSolutionStatusFromUint8(message_buffer[69]);
}

std::uint16_t getUint16FromBuffer(std::uint8_t* buffer)
{
  return combineTwoBytes(buffer[0], buffer[1]);
}

std::uint32_t getUint32FromBuffer(std::uint8_t* buffer)
{
  return combineFourBytes(buffer[0], buffer[1], buffer[2], buffer[3]);
}

std::uint64_t getUint64FromBuffer(std::uint8_t* buffer)
{
  return combineEightBytes(buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7]);
}

float getFloatFromBuffer(std::uint8_t* buffer)
{
  auto uint32_from_buffer = getUint32FromBuffer(buffer);
  float float_from_buffer;
  std::memcpy(&float_from_buffer, &uint32_from_buffer, sizeof(float));

  return float_from_buffer;
}

double getDoubleFromBuffer(std::uint8_t* buffer)
{
  auto uint64_from_buffer = getUint64FromBuffer(buffer);
  double double_from_buffer;
  std::memcpy(&double_from_buffer, &uint64_from_buffer, sizeof(double));

  return double_from_buffer;
}

std::uint16_t combineTwoBytes(std::uint8_t lsb, std::uint8_t msb)
{
  return (static_cast<std::uint16_t>(lsb) | static_cast<std::uint16_t>(msb) << 8);
}

std::uint32_t combineFourBytes(std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3)
{
  return (static_cast<std::uint32_t>(b0) | static_cast<std::uint32_t>(b1) << 8 | static_cast<std::uint32_t>(b2) << 16 |
          static_cast<std::uint32_t>(b3) << 24);
}

std::uint64_t combineEightBytes(std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3, std::uint8_t b4,
                                std::uint8_t b5, std::uint8_t b6, std::uint8_t b7)
{
  return (static_cast<std::uint64_t>(b0) | static_cast<std::uint64_t>(b1) << 8 | static_cast<std::uint64_t>(b2) << 16 |
          static_cast<std::uint64_t>(b3) << 24 | static_cast<std::uint64_t>(b4) << 32 |
          static_cast<std::uint64_t>(b5) << 40 | static_cast<std::uint64_t>(b6) << 48 |
          static_cast<std::uint64_t>(b7) << 56);
}

float bynavPitchToRep103(float bynav_pitch)
{
  // TODO(olek) - check if this is proper transformation
  return common_functions::radians(bynav_pitch);
}

float bynavYawToRep103(float bynav_yaw)
{
  return -common_functions::radians(bynav_yaw - 90);
}

ExtendedSolutionStatus getExtendedSolutionStatusFromUint8(std::uint8_t byte)
{
  auto ext_status = ExtendedSolutionStatus{};

  ext_status.rtk_solution_verified_ = (byte & 0x01);
  ext_status.iono_correction_ = (byte & 0x0E) >> 1;
  ext_status.rtk_assist_active_ = (byte & 0x10);
  ext_status.antenna_status_ = (byte & 0x20) >> 5;

  return ext_status;
}

}  // namespace BynavFrameParser
