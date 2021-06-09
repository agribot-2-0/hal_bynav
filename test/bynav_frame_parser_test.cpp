/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <cstring>

#include <gtest/gtest.h>
#include "hal_bynav/BynavFrameParser.h"

TEST(BynavFrameParserTest, ParseDoubleFromBuffer)
{
  // Prepare data
  double value = 456.324243543543;
  std::uint8_t buffer[8];
  std::memcpy(buffer, &value, sizeof(double));

  ASSERT_DOUBLE_EQ(value, BynavFrameParser::getDoubleFromBuffer(buffer));
}

TEST(BynavFrameParserTest, ParseFloatFromBuffer)
{
  // Prepare data
  float value = 124.8891;
  std::uint8_t buffer[4];
  std::memcpy(buffer, &value, sizeof(float));

  ASSERT_FLOAT_EQ(value, BynavFrameParser::getFloatFromBuffer(buffer));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
