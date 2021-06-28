/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <gtest/gtest.h>
#include <cstring>

#include "hal_bynav/BynavFrameFactory.h"
#include "hal_bynav/BynavParser.h"
#include "agribot_common/connection/Buffer.h"
#include "agribot_common/connection/LsbFirstParsingStrategy.h"

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
    ASSERT_EQ(err.what(), std::string("[BynavFrameFactory] id = " + std::to_string(id) + " is not supported"));
  }
  catch (...)
  {
    FAIL() << "Expected std::invalid_argument";
  }
}

TEST(BynavParserTest, SplitsPacketBufferWithSyncBytesAtTheEnd)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  std::vector<uint8_t> first_frame = { 0x18, 0x65, 0x78, 0xAF, 0xFF };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), first_frame.begin(), first_frame.end());
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(1));
  ASSERT_TRUE(queue.front().buffer_ == first_frame);
}

TEST(BynavParserTest, SplitsPacketBufferWithSyncBytesAtTheBeginning)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };

  // Don't preallocate buffer size - allows to use packet_buffer.read_length_ = packet_buffer.buffer_.size();
  Buffer packet_buffer{ 0 };

  std::vector<uint8_t> first_frame = { 0x18, 0x65, 0x78, 0xAF, 0xFF };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), first_frame.begin(), first_frame.end());
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(1));
  ASSERT_TRUE(queue.front().buffer_ == first_frame);
}

TEST(BynavParserTest, SplitsPacketBufferWithSyncBytesAtTheBeginningAndAtTheEnd)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  std::vector<uint8_t> first_frame = { 0x18, 0x65, 0x78, 0xAF, 0xFF };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), first_frame.begin(), first_frame.end());
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(1));
  ASSERT_TRUE(queue.front().buffer_ == first_frame);
}

TEST(BynavParserTest, SplitsPacketBufferWithoutSyncBytes)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  std::vector<uint8_t> first_frame = { 0x18, 0x65, 0x78, 0xAF, 0xFF };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), first_frame.begin(), first_frame.end());
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(1));
  ASSERT_TRUE(queue.front().buffer_ == first_frame);
}

TEST(BynavParserTest, SplitsPacketBufferOnlyWithSyncBytes)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(0));
}

TEST(BynavParserTest, SplitsPacketBufferOnlyWithMultipleSyncBytes)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(0));
}

TEST(BynavParserTest, SplitsEmptyPacketBuffer)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(0));
}

TEST(BynavParserTest, SplitsPacketBufferWithMultipleFramesAndSyncBytes)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  std::vector<uint8_t> first_frame = { 0x18, 0x65, 0x78, 0xAF, 0xFF };
  std::vector<uint8_t> second_frame = { 0x32, 0x11, 0xBB };
  std::vector<uint8_t> third_frame = { 0x54, 0x97, 0xCD, 0x2A, 0x00, 0x21, 0x37, 0xAA, 0x44 };

  // [SYNC][SYNC][SYNC][FIRST][SYNC][SYNC][SECOND][SYNC][THIRD][SYNC][SYNC]
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), first_frame.begin(), first_frame.end());
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), second_frame.begin(), second_frame.end());
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), third_frame.begin(), third_frame.end());
  packet_buffer.read_length_ = packet_buffer.buffer_.size();
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  auto queue = parser.getFrameBuffersQueue();
  ASSERT_EQ(queue.size(), static_cast<std::size_t>(3));
  ASSERT_TRUE(queue.front().buffer_ == first_frame);
  queue.pop();
  ASSERT_TRUE(queue.front().buffer_ == second_frame);
  queue.pop();
  ASSERT_TRUE(queue.front().buffer_ == third_frame);
}

TEST(BynavParserTest, InputBufferIsCleared)
{
  // Prepare data
  std::unique_ptr<IParsingStrategy> parsing_strategy = std::make_unique<LsbFirstParsingStrategy>();
  std::unique_ptr<IFrameFactory> frame_factory = std::make_unique<BynavFrameFactory>();
  BynavParser parser{ std::move(parsing_strategy), std::move(frame_factory) };
  Buffer packet_buffer{ 0 };

  std::vector<uint8_t> first_frame = { 0x18, 0x65, 0x78, 0xAF, 0xFF };

  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), first_frame.begin(), first_frame.end());
  packet_buffer.buffer_.insert(packet_buffer.buffer_.end(), std::begin(BynavParser::sync_bytes_),
                               std::end(BynavParser::sync_bytes_));
  packet_buffer.read_length_ = packet_buffer.buffer_.size();

  // Perform operations
  parser.getFrameBuffersFromPacketBuffer(packet_buffer);

  // Assert
  ASSERT_EQ(packet_buffer.getReadLength(), static_cast<std::size_t>(0));
}

TEST(BynavFrameTest, CalculateCrcFromBuffer)
{
  // Prepare data
  BynavFrame frame;
  LsbFirstParsingStrategy strategy;
  uint32_t expected_crc = 2683756513;
  std::vector<uint8_t> buffer = { 0x14, 0x32, 0xFA, 0x92, 0xAA, 0xB5, 0x06, 0x65, 0x88, 0x39 };

  // Check assertion
  ASSERT_EQ(expected_crc, frame.calculateCrc(buffer.begin(), buffer.end(), strategy));
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
