/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <gtest/gtest.h>
#include <cstring>

#include "hal_bynav/Bynav.h"
#include "hal_bynav/BynavFrameParser.h"

TEST(BynavFrameParserTest, ParseDoubleFromBuffer)
{
  // Prepare data
  double value = 456.324243543543;
  std::uint8_t buffer[8];
  std::memcpy(buffer, &value, sizeof(double));

  // Check assertion
  ASSERT_DOUBLE_EQ(value, BynavFrameParser::getDoubleFromBuffer(buffer));
}

TEST(BynavFrameParserTest, ParseFloatFromBuffer)
{
  // Prepare data
  float value = 124.8891;
  std::uint8_t buffer[4];
  std::memcpy(buffer, &value, sizeof(float));

  // Check assertion
  ASSERT_FLOAT_EQ(value, BynavFrameParser::getFloatFromBuffer(buffer));
}

TEST(BynavFrameParserTest, GetUint16FromBuffer)
{
  // Prepare data
  std::uint16_t value = 3487;
  std::uint8_t buffer[2];
  buffer[0] = static_cast<std::uint8_t>(value & 0xFF);
  buffer[1] = static_cast<std::uint8_t>((value & 0xFF00) >> 8);

  // Check assertion
  ASSERT_EQ(value, BynavFrameParser::getUint16FromBuffer(buffer));
}

TEST(BynavFrameParserTest, GetUint32FromBuffer)
{
  // Prepare data
  std::uint32_t value = 912387;
  std::uint8_t buffer[4];
  buffer[0] = static_cast<std::uint8_t>(value & 0xFF);
  buffer[1] = static_cast<std::uint8_t>((value & 0xFF00) >> 8);
  buffer[2] = static_cast<std::uint8_t>((value & 0xFF0000) >> 16);
  buffer[3] = static_cast<std::uint8_t>((value & 0xFF000000) >> 24);

  // Check assertion
  ASSERT_EQ(value, BynavFrameParser::getUint32FromBuffer(buffer));
}

TEST(BynavFrameParserTest, GetUint64FromBuffer)
{
  // Prepare data
  std::uint64_t value = 324294967296;
  std::uint8_t buffer[8];
  buffer[0] = static_cast<std::uint8_t>(value & 0xFF);
  buffer[1] = static_cast<std::uint8_t>((value & 0xFF00) >> 8);
  buffer[2] = static_cast<std::uint8_t>((value & 0xFF0000) >> 16);
  buffer[3] = static_cast<std::uint8_t>((value & 0xFF000000) >> 24);
  buffer[4] = static_cast<std::uint8_t>((value & 0xFF00000000) >> 32);
  buffer[5] = static_cast<std::uint8_t>((value & 0xFF0000000000) >> 40);
  buffer[6] = static_cast<std::uint8_t>((value & 0xFF000000000000) >> 48);
  buffer[7] = static_cast<std::uint8_t>((value & 0xFF00000000000000) >> 56);

  // Check assertion
  ASSERT_EQ(value, BynavFrameParser::getUint64FromBuffer(buffer));
}

TEST(BynavFrameParserTest, CalculateCrcForFrame)
{
  // Prepare data
  std::uint32_t expected_crc = 2683756513;
  constexpr std::size_t frame_length = 10;
  std::uint8_t frame[frame_length] = { 0x14, 0x32, 0xFA, 0x92, 0xAA, 0xB5, 0x06, 0x65, 0x88, 0x39 };

  // Check assertion
  ASSERT_EQ(expected_crc, BynavFrameParser::calculateCrcForFrame(frame, frame_length));
}

TEST(BynavFrameParserTest, BynavYawToRep103)
{
  // Prepare data
  float ned_yaw_degrees = 63.78;
  float expected_enu_yaw_radians = 0.45762533;

  // Check assertion
  ASSERT_FLOAT_EQ(expected_enu_yaw_radians, BynavFrameParser::bynavYawToRep103(ned_yaw_degrees));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
