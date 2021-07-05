/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/frame/BynavFrameFactory.h>

std::unique_ptr<IFrame> BynavFrameFactory::createFrame(uint64_t id) const
{
  switch (id)
  {
    case 971:
      return std::make_unique<HeadingFrame>();
    case 1429:
      return std::make_unique<BestGnssPosFrame>();
    default:
      throw std::invalid_argument("[BynavFrameFactory] id = " + std::to_string(id) + " is not supported");
  }
}
