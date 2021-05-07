/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <ros/ros.h>

#include "hal_bynav/Bynav.h"
#include "hal_bynav/BynavSocket.h"
#include "hal_bynav/BynavFrameParser.h"

class HalBynavNode
{
private:
  BynavSocket bynav_socket_;
  Bynav bynav_;

public:
  HalBynavNode();
  ~HalBynavNode();
  void run();
};
