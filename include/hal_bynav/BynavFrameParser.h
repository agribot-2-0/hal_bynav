/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <cstring>

#include "agribot_common/CommonFunctions/common_functions.h"
#include "hal_bynav/Bynav.h"

namespace BynavFrameParser
{
constexpr std::uint8_t sync_bytes[] = { 0xAA, 0x44, 0x12 };
constexpr std::uint16_t crc_length = 4;
constexpr std::uint64_t crc32_polynomial = 0xEDB88320;

constexpr std::size_t heading_message_length_ = 44;
constexpr std::size_t best_gnss_pos_message_length_ = 72;

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

void parsePacket(Bynav& bynav, std::uint8_t* packet_buffer, std::size_t packet_length);  // NOLINT(runtime/references)
std::vector<std::pair<int, std::size_t>> findFrames(std::uint8_t* packet_buffer, std::size_t packet_length);
void parseFrame(Bynav& bynav, uint8_t* frame_buffer, std::size_t frame_length);  // NOLINT(runtime/references)
Header parseHeader(std::uint8_t* frame_buffer);
void checkCrc(std::uint8_t* frame_buffer, std::size_t frame_length);
std::uint32_t calculateCrcForFrame(std::uint8_t* frame_buffer, std::size_t frame_length);
std::uint64_t calculateCrc(std::uint64_t value);
void parseHeadingMessage(Bynav& bynav, std::uint8_t* message_buffer,  // NOLINT(runtime/references)
                         std::size_t message_length);
void parseBestGnssPosMessage(Bynav& bynav, std::uint8_t* message_buffer,  // NOLINT(runtime/references)
                             std::size_t message_length);

inline std::uint16_t getUint16FromBuffer(std::uint8_t* buffer);
inline std::uint32_t getUint32FromBuffer(std::uint8_t* buffer);
inline std::uint64_t getUint64FromBuffer(std::uint8_t* buffer);
float getFloatFromBuffer(std::uint8_t* buffer);
double getDoubleFromBuffer(std::uint8_t* buffer);
inline std::uint16_t combineTwoBytes(std::uint8_t lsb, std::uint8_t msb);
inline std::uint32_t combineFourBytes(std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3);
inline std::uint64_t combineEightBytes(std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3,
                                       std::uint8_t b4, std::uint8_t b5, std::uint8_t b6, std::uint8_t b7);
inline float bynavPitchToRep103(float bynav_pitch);
inline float bynavYawToRep103(float bynav_yaw);
ExtendedSolutionStatus getExtendedSolutionStatusFromUint8(std::uint8_t byte);

}  // namespace BynavFrameParser
