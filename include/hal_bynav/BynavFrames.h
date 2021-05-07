/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <cstdint>

namespace BynavFrames
{
struct Header
{
  std::uint16_t message_id_;
  std::size_t header_length_;
  std::size_t message_length_;
  std::size_t frame_length_;
};
}  // namespace BynavFrames
