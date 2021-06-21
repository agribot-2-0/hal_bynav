/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/IFrame.h>

class HeadingFrame : public IFrame
{
private:
  float baseline_length_;

public:
  HeadingFrame() = default;
  ~HeadingFrame() = default;
  void parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
             IParsingStrategy& strategy) override;  // NOLINT(runtime/references)
};
