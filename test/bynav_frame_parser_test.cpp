/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <gtest/gtest.h>
#include "hal_bynav/BynavFrameParser.h"

TEST(BynavFrameParserTest, ParsesDoubleFromBuffer)
{
  // Prepare data
  double value_double = 456.324243543543;
  std::uint8_t* value_uint8_ptr = reinterpret_cast<uint8_t*>(&value_double);

  ASSERT_DOUBLE_EQ(value_double, BynavFrameParser::getDoubleFromBuffer(value_uint8_ptr));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
