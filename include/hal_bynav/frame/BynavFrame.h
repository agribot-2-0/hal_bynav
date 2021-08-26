/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/frame/IFrame.h>
#include <agribot_common/connection/parsing_strategy/LsbFirstParsingStrategy.h>

#include <stdexcept>
#include <string>
#include <unordered_map>
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

  void deserialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it) override;
  void serialize(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it) override;
  uint32_t calculateCrc(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it);
  uint64_t calculateCrcForUint64(uint64_t value);

  static constexpr uint8_t CRC_LENGTH{ 4 };
  static constexpr uint64_t CRC32_POLYNOMIAL{ 0xEDB88320 };

  // Housekeeping (header) data
  uint8_t header_length_;
  uint16_t message_id_;
  uint16_t message_length_;
  uint16_t gnss_week_;
  uint32_t gnss_milliseconds_;
  uint32_t crc_in_frame_;

  // taken from http://www.asgeupos.pl/index.php?wpg_type=syst_descr&sub=ref_st
  inline static const std::unordered_map<int, std::string> station_ids_ = {
    { 31, "BART" },  { 41, "BIAL" },  { 112, "BILG" }, { 98, "BOGI" },  { 61, "BOR1" },  { 111, "BPDL" },
    { 42, "BRSK" },  { 141, "BUZD" }, { 81, "BYDG" },  { 97, "CBKA" },  { 92, "CCHN" },  { 113, "CHEL" },
    { 11, "CHNO" },  { 21, "CHOJ" },  { 12, "DRWP" },  { 32, "DZIA" },  { 33, "ELBL" },  { 22, "GDA1" },
    { 34, "GIZY" },  { 121, "GLOG" }, { 62, "GNIE" },  { 15, "GOLE" },  { 43, "GRAJ" },  { 82, "GRUD" },
    { 51, "GWWL" },  { 44, "HAJN" },  { 182, "HOZD" }, { 114, "HRUB" }, { 35, "ILAW" },  { 122, "JLGR" },
    { 93, "JOZ2" },  { 66, "KALI" },  { 13, "KAM1" },  { 161, "KATO" }, { 63, "KEPN" },  { 142, "KLCE" },
    { 123, "KLDZ" }, { 162, "KLOB" }, { 64, "KONI" },  { 23, "KOSC" },  { 14, "KOSZ" },  { 1, "KRA1" },
    { 181, "KROS" }, { 65, "KROT" },  { 131, "KUTN" }, { 37, "LAMA" },  { 124, "LEGN" }, { 163, "LELO" },
    { 71, "LESZ" },  { 132, "LODZ" }, { 45, "LOMZ" },  { 115, "LUBL" }, { 187, "MIEL" }, { 17, "MIES" },
    { 94, "MIMA" },  { 183, "MLCN" }, { 95, "MYSZ" },  { 96, "NODW" },  { 72, "NTML" },  { 172, "NWSC" },
    { 173, "NWT1" }, { 151, "NYSA" }, { 152, "OPLE" }, { 116, "OPLU" }, { 39, "OPNT" },  { 101, "OSMZ" },
    { 133, "PITR" }, { 67, "POZN" },  { 73, "PPIL" },  { 175, "PROS" }, { 186, "PRZM" }, { 102, "RADM" },
    { 24, "REDZ" },  { 134, "RWM1" }, { 117, "RYKI" }, { 135, "SIDZ" }, { 103, "SIED" }, { 104, "SIPC" },
    { 105, "SOCH" }, { 46, "SOKL" },  { 26, "STRG" },  { 54, "SWIB" },  { 47, "SWKI" },  { 16, "SZE2" },
    { 184, "TABG" }, { 166, "TAR1" }, { 83, "TORU" },  { 174, "TRNW" }, { 185, "USDL" }, { 106, "WAT1" },
    { 136, "WIEL" }, { 25, "WLAD" },  { 126, "WLBR" }, { 118, "WLDW" }, { 84, "WLOC" },  { 164, "WOD1" },
    { 74, "WRKI" },  { 125, "WROC" }, { 53, "ZARY" },  { 52, "ZIGR" },  { 165, "ZYWI" }, { 225, "CFRM" },
    { 228, "CJES" }, { 221, "CLIB" }, { 226, "CPAR" }, { 224, "CSUM" }, { 227, "CSVI" }, { 222, "CTRU" },
    { 252, "14" },   { 253, "17" },   { 251, "22" },   { 257, "26" },   { 255, "139" },  { 256, "147" },
    { 254, "807" },  { 241, "MRJM" }, { 242, "VARN" }, { 243, "VEIS" }, { 244, "VSTT" }, { 263, "GANP" },
    { 261, "KUZA" }, { 262, "LIE1" }, { 265, "SKSK" }, { 264, "SKSL" }, { 266, "SKSV" }
  };

protected:
  LsbFirstParsingStrategy strategy_;
};
