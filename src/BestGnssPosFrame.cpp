/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/BestGnssPosFrame.h>

void BestGnssPosFrame::parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                             IParsingStrategy& strategy)
{
  // TODO(olek)
  static_cast<void>(end_it);
  latitude_ = strategy.getDouble(begin_it + 8);
}
