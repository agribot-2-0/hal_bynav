/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/IFrame.h>

class BestGnssPosFrame : public IFrame
{
private:
  double latitude_;

public:
  BestGnssPosFrame() = default;
  ~BestGnssPosFrame() = default;
  void parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
             IParsingStrategy& strategy) override;  // NOLINT(runtime/references)
};
