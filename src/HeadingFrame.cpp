/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/HeadingFrame.h>

void HeadingFrame::parse(std::vector<uint8_t>::iterator begin_it, std::vector<uint8_t>::iterator end_it,
                         IParsingStrategy& strategy)
{
  // TODO(olek)
  static_cast<void>(end_it);
  baseline_length_ = strategy.getFloat(begin_it + 8);
}
