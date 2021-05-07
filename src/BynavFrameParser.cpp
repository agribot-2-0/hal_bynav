/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/BynavFrameParser.h"

namespace BynavFrameParser
{
// Maybe change name to something like "updateFromFrame?"
// Or create config structs for various frames we want to receive?
void parsePacket(Bynav& bynav, std::uint8_t* packet_buffer, std::size_t packet_length)
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
  uint8_t* search_result;
  uint8_t* search_position = packet_buffer;
  uint8_t* end_position = packet_buffer + packet_length;

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

  for (std::size_t i = 0; i < frame_start_indexes.size() - 1; i++)
  {
    std::size_t frame_length = frame_start_indexes[i + 1] - frame_start_indexes[i];
    frames.push_back(std::make_pair(frame_start_indexes[i], frame_length));
  }

  // Push last frame (special case, because it doesn't have ending SyncBytes)
  auto last = frame_start_indexes.size() - 1;
  std::size_t frame_length = packet_length - frame_start_indexes[last];
  frames.push_back(std::make_pair(frame_start_indexes[last], frame_length));

  (void)packet_buffer;
  (void)packet_length;

  return frames;
}

void parseFrame(Bynav& bynav, uint8_t* frame_buffer, std::size_t frame_length)
{
  checkCrc(frame_buffer, frame_length);

  auto header = parseHeader(frame_buffer);

  switch (static_cast<MessageId>(header.message_id_))
  {
    case MessageId::Heading:
      parseHeadingMessage(bynav, frame_buffer + header.header_length_, header.message_length_);
      break;

    case MessageId::BestGnssPos:
      parseBestGnssPosMessage(bynav, frame_buffer + header.header_length_, header.message_length_);
      break;

    default:
      throw std::runtime_error("Message ID = " + std::to_string(header.message_id_) + " is not supported by parser");
      break;
  }
}

BynavFrames::Header parseHeader(std::uint8_t* frame_buffer)
{
  // If sync bytes are wrong throw exception
  BynavFrames::Header header;

  auto combine_bytes = [](std::uint8_t lsb, std::uint8_t msb) {
    return (static_cast<std::uint16_t>(lsb)) | (static_cast<std::uint16_t>(msb) << 8);
  };

  header.header_length_ = frame_buffer[3];
  header.message_id_ = combine_bytes(frame_buffer[4], frame_buffer[5]);
  header.message_length_ = combine_bytes(frame_buffer[8], frame_buffer[9]);
  header.frame_length_ = header.header_length_ + header.message_length_ + crc_length;
  return header;
}

void checkCrc(std::uint8_t* frame_buffer, std::size_t frame_length)
{
  std::uint32_t crc_in_frame = static_cast<std::uint32_t>(frame_buffer[frame_length - 4]) |
                               static_cast<std::uint32_t>(frame_buffer[frame_length - 3]) << 8 |
                               static_cast<std::uint32_t>(frame_buffer[frame_length - 2]) << 16 |
                               static_cast<std::uint32_t>(frame_buffer[frame_length - 1]) << 24;

  auto calculated_crc = calculateCrcForFrame(frame_buffer, frame_length - 4);

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
    auto tmp2 = calculateCrc((crc ^ frame_buffer[i]) & 0xFF);
    crc = tmp1 ^ tmp2;
  }
  return static_cast<std::uint32_t>(crc);
}

std::uint64_t calculateCrc(std::uint64_t value)
{
  std::uint64_t crc = value;

  for (size_t i = 8; i > 0; --i)
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

void parseHeadingMessage(Bynav& bynav, std::uint8_t* message_buffer, std::size_t message_length)
{
  (void)bynav;
  (void)message_buffer;
  (void)message_length;
  ROS_WARN_STREAM("Parsing heading message");
}

void parseBestGnssPosMessage(Bynav& bynav, std::uint8_t* message_buffer, std::size_t message_length)
{
  (void)bynav;
  (void)message_buffer;
  (void)message_length;
  ROS_WARN_STREAM("Parsing bestGnssPos message");
}

}  // namespace BynavFrameParser
