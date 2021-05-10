/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <iostream>

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

struct Bynav
{
  // BestGnssPos
  SolutionStatus best_gnss_pos_solution_status_;
  PositionStatus best_gnss_pos_position_status_;

  double latitude_;
  double longitude_;
  double height_;

  double latitude_std_;
  double longitude_std_;
  double height_std_;

  std::uint8_t tracked_satellites_n_;
  std::uint8_t solution_satellites_n_;

  // Heading
  SolutionStatus heading_solution_status_;
  PositionStatus heading_position_status_;
  SolutionSource heading_solution_source_;
  float heading_baseline_length_;

  double pitch_;
  double yaw_;

  double pitch_std_;
  double yaw_std_;

  Bynav() = default;
  ~Bynav() = default;
  friend std::ostream& operator<<(std::ostream& os, const Bynav& b);
};

std::ostream& operator<<(std::ostream& os, const Bynav& b);
