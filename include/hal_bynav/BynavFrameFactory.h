/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/IFrameFactory.h>
#include <hal_bynav/HeadingFrame.h>
#include <hal_bynav/BestGnssPosFrame.h>

#include <memory>
#include <stdexcept>

class BynavFrameFactory : public IFrameFactory
{
public:
  BynavFrameFactory() = default;
  ~BynavFrameFactory() = default;
  BynavFrameFactory(const BynavFrameFactory& other) = default;
  BynavFrameFactory(BynavFrameFactory&& other) = default;
  BynavFrameFactory& operator=(const BynavFrameFactory& other) = default;
  BynavFrameFactory& operator=(BynavFrameFactory&& other) = default;
  std::unique_ptr<IFrame> createFrame(uint64_t id) override;
};
