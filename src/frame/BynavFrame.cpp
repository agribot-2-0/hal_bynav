/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/frame/BynavFrame.h>

void BynavFrame::deserialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it)
{
  auto frame_length = std::distance(begin_it, end_it);

  // Check if frame is long enough to parse both bytes of message length
  if (frame_length <= (BynavFrameIndex::MessageLength + 1))
  {
    throw std::runtime_error("Frame length too short to decode message_length_");
  }

  // Parse housekeeping data
  header_length_ = strategy_.getUint8(std::next(begin_it, BynavFrameIndex::HeaderLength));
  message_id_ = strategy_.getUint16(std::next(begin_it, BynavFrameIndex::MessageId));
  message_length_ = strategy_.getUint16(std::next(begin_it, BynavFrameIndex::MessageLength));
  gnss_week_ = strategy_.getUint16(std::next(begin_it, BynavFrameIndex::GnssWeek));
  gnss_milliseconds_ = strategy_.getUint32(std::next(begin_it, BynavFrameIndex::GnssMilliseconds));

  // Check length validity
  if (frame_length != (header_length_ + message_length_ + CRC_LENGTH))
  {
    throw std::runtime_error("Frame length mismatch");
  }
}

void BynavFrame::serialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it)
{
  // TODO(all) - implement in the future
  static_cast<void>(begin_it);
  static_cast<void>(end_it);
}

uint32_t BynavFrame::calculateCrc(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it)
{
  uint64_t crc = 0;

  for (auto it = begin_it; it != end_it; ++it)
  {
    auto tmp1 = (crc >> 8) & 0x00FFFFFFL;
    auto tmp2 = calculateCrcForUint64((crc ^ strategy_.getUint8(it)) & 0xFF);
    crc = tmp1 ^ tmp2;
  }
  return static_cast<uint32_t>(crc);
}

uint64_t BynavFrame::calculateCrcForUint64(uint64_t value)
{
  uint64_t crc = value;

  for (std::size_t i = 8; i > 0; --i)
  {
    if (crc & 1)
    {
      crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
    }
    else
    {
      crc >>= 1;
    }
  }
  return crc;
}
