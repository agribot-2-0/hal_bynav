/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/IParser.h>

class BynavParser : public IParser
{
private:
  uint32_t getCrcFromFrame(std::vector<uint8_t>::iterator begin_it);
  uint32_t calculateCrc(std::vector<uint8_t>::iterator begin_it);

public:
  BynavParser() = delete;
  BynavParser(std::unique_ptr<IParsingStrategy> strategy, std::unique_ptr<IFrameFactory> factory);
  ~BynavParser() = default;
  BynavParser(const BynavParser& other) = default;
  BynavParser(BynavParser&& other) = default;
  BynavParser& operator=(const BynavParser& other) = default;
  BynavParser& operator=(BynavParser&& other) = default;

  bool checkCrc(std::vector<uint8_t>::iterator begin_it) override;
  uint64_t getId(std::vector<uint8_t>::iterator begin_it) override;
  std::vector<std::pair<std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator>>
  split(Buffer& buffer) override;  // NOLINT(runtime/references)
};
