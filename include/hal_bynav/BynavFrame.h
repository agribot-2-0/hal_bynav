/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/IFrame.h>

#include <stdexcept>
#include <vector>

enum class SolutionStatus
{
  SolComputed = 0,
  InsufficientObs = 1,
  NoCovergence = 2,
  Singularity = 3,
  CovTrace = 4,
  TestDist = 5,
  ColdStart = 6,
  VHLimit = 7,
  Variance = 8,
  Residuals = 9,
  IntegrityWarning = 13,
  Pending = 18,
  InvalidFix = 19,
  Unauthorized = 20,
  InvalidRate = 22,
};

enum class PositionStatus
{
  None = 0,
  Fixedpos = 1,
  Fixedheight = 2,
  Floatconv = 4,
  Widelane = 5,
  Narrowlane = 6,
  DopplerVelocity = 8,
  Single = 16,
  Psrdiff = 17,
  Waas = 18,
  Propagated = 19,
  L1Float = 32,
  IonofreeFloat = 33,
  NarrowFloat = 34,
  L1Int = 48,
  WideInt = 49,
  NarrowInt = 50,
  RtkDirectIns = 51,
  InsSbas = 52,
  InsPsrsp = 53,
  InsPsrdiff = 54,
  InsRtkfloat = 55,
  InsRtkfixed = 56,
  PppConverging = 68,
  Ppp = 69,
  Operational = 70,
  Warning = 71,
  OutOfBounds = 72,
  InsPppConverging = 73,
  InsPpp = 74,
  PppBasicConverging = 77,
  PppBasic = 78,
  InsPpppBasicConverging = 79,
  InsPpppBasic = 80
};

enum class SolutionSource
{
  PrimaryAntenna = 0,
  SecondaryAntenna = 4,
};

struct ExtendedSolutionStatus
{
  bool rtk_solution_verified_;
  uint8_t iono_correction_;
  bool rtk_assist_active_;
  uint8_t antenna_status_;

  explicit ExtendedSolutionStatus(uint8_t byte)
  {
    rtk_solution_verified_ = (byte & 0x01);
    iono_correction_ = (byte & 0x0E) >> 1;
    rtk_assist_active_ = (byte & 0x10);
    antenna_status_ = (byte & 0x20) >> 5;
  }

  ExtendedSolutionStatus() = default;
  ~ExtendedSolutionStatus() = default;
  ExtendedSolutionStatus(const ExtendedSolutionStatus&) = default;
  ExtendedSolutionStatus(ExtendedSolutionStatus&&) = default;
  ExtendedSolutionStatus& operator=(const ExtendedSolutionStatus&) = default;
  ExtendedSolutionStatus& operator=(ExtendedSolutionStatus&&) = default;
};

namespace BynavFrameIndex
{
enum
{
  HeaderLength = 3,
  MessageId = 4,
  MessageLength = 8,
  GnssWeek = 14,
  GnssMilliseconds = 16
};
}  // namespace BynavFrameIndex

struct BynavFrame : public IFrame
{
  BynavFrame() = default;
  virtual ~BynavFrame() = default;
  BynavFrame(const BynavFrame& other) = delete;
  BynavFrame(BynavFrame&& other) = delete;
  BynavFrame& operator=(const BynavFrame& other) = delete;
  BynavFrame& operator=(BynavFrame&& other) = delete;

  void deserialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                   const IParsingStrategy& strategy) override;
  void serialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                 const IParsingStrategy& strategy) override;
  uint32_t calculateCrc(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                        const IParsingStrategy& strategy);
  uint64_t calculateCrcForUint64(uint64_t value);

  static constexpr uint8_t CRC_LENGTH{ 4 };
  static constexpr std::uint64_t CRC32_POLYNOMIAL = 0xEDB88320;

  // Housekeeping (header) data
  uint8_t header_length_;
  uint16_t message_id_;
  uint16_t message_length_;
  uint16_t gnss_week_;
  uint32_t gnss_milliseconds_;
  uint32_t crc_in_frame_;
};
