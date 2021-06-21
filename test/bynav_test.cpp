/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <gtest/gtest.h>
#include <cstring>

#include "hal_bynav/BynavFrameFactory.h"

TEST(BynavFrameFactoryTest, CreatesHeadingFrame)
{
  BynavFrameFactory factory;

  ASSERT_TRUE(typeid(*(factory.createFrame(971))) == typeid(HeadingFrame));
}

TEST(BynavFrameFactoryTest, CreatesBestGnssPosFrame)
{
  BynavFrameFactory factory;

  ASSERT_TRUE(typeid(*(factory.createFrame(1429))) == typeid(BestGnssPosFrame));
}

TEST(BynavFrameFactoryTest, ThrowsInvalidArgumentExceptionForUnsuportedId)
{
  BynavFrameFactory factory;
  uint64_t id = 7823;

  try
  {
    factory.createFrame(id);
    FAIL() << "Expected std::invalid_argument";
  }
  catch (const std::invalid_argument& err)
  {
    EXPECT_EQ(err.what(), std::string("[BynavFrameFactory] id = " + std::to_string(id) + " is not supported"));
  }
  catch (...)
  {
    FAIL() << "Expected std::invalid_argument";
  }
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
