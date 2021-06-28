/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/BynavFrame.h>

void BynavFrame::parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                       const IParsingStrategy& strategy)
{
  // TODO(olek)
  // Parse housekeeping data
  header_length_ = strategy.getUint8(std::next(begin_it, 0)) - SYNC_BYTES_LENGTH;
  message_id_ = strategy.getUint16(std::next(begin_it, 1));
  message_length_ = strategy.getUint16(std::next(begin_it, 5));
  gnss_week_ = strategy.getUint16(std::next(begin_it, 11));
  gnss_milliseconds_ = strategy.getUint32(std::next(begin_it, 13));

  // Check length validity
  if (auto expected_length = std::distance(begin_it, end_it);
      expected_length != (header_length_ + message_length_ + CRC_LENGTH))  // TODO(olek) - Maybe minus SYNC_BYTES_LENGTH
  {
    // TODO(olek) - use custom exception?
    throw std::runtime_error("Frame length missmatch");
  }
}

uint32_t BynavFrame::calculateCrc(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                                  const IParsingStrategy& strategy)
{
  uint64_t crc = 0;

  // for (const auto& it = begin_it; it != end_it; ++it)
  for (auto& it = begin_it; it != end_it; ++it)
  {
    auto tmp1 = (crc >> 8) & 0x00FFFFFFL;
    auto tmp2 = calculateCrcForUint64((crc ^ strategy.getUint8(it)) & 0xFF);
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
