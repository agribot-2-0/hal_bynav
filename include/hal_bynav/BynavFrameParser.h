/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include "hal_bynav/Bynav.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

#include <ros/ros.h>

namespace BynavFrameParser
{
constexpr std::uint8_t sync_bytes[] = { 0xAA, 0x44, 0x12 };
constexpr std::uint16_t crc_length = 4;
constexpr std::uint64_t crc32_polynomial = 0xEDB88320;

enum class MessageId : std::uint16_t
{
  Heading = 971,
  BestGnssPos = 1429,
};

struct Header
{
  MessageId message_id_;
  std::size_t header_length_;
  std::size_t message_length_;
  std::size_t frame_length_;
};

void parsePacket(Bynav& bynav, std::uint8_t* packet_buffer, std::size_t packet_length);
std::vector<std::pair<int, std::size_t>> findFrames(std::uint8_t* packet_buffer, std::size_t packet_length);
void parseFrame(Bynav& bynav, uint8_t* frame_buffer, std::size_t frame_length);
Header parseHeader(std::uint8_t* frame_buffer);
void checkCrc(std::uint8_t* frame_buffer, std::size_t frame_length);
std::uint32_t calculateCrcForFrame(std::uint8_t* frame_buffer, std::size_t frame_length);
std::uint64_t calculateCrc(std::uint64_t value);
void parseHeadingMessage(Bynav& bynav, std::uint8_t* message_buffer, std::size_t message_length);
void parseBestGnssPosMessage(Bynav& bynav, std::uint8_t* message_buffer, std::size_t message_length);

}  // namespace BynavFrameParser
