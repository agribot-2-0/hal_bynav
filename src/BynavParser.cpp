/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/BynavParser.h>

BynavParser::BynavParser(std::unique_ptr<IParsingStrategy> strategy, std::unique_ptr<IFrameFactory> factory)
  : IParser(std::move(strategy), std::move(factory))
{
}

uint32_t BynavParser::getCrcFromFrame(std::vector<uint8_t>::iterator begin_it)
{
  // TODO(olek)
  static_cast<void>(begin_it);
  return 0;
}

uint32_t BynavParser::calculateCrc(std::vector<uint8_t>::iterator begin_it)
{
  // TODO(olek)
  static_cast<void>(begin_it);
  return 0;
}

bool BynavParser::checkCrc(std::vector<uint8_t>::iterator begin_it)
{
  return (getCrcFromFrame(begin_it) == calculateCrc(begin_it));
}

uint64_t BynavParser::getId(std::vector<uint8_t>::iterator begin_it)
{
  return static_cast<uint64_t>(parsing_strategy_->getUint8(begin_it + 1));
}

std::vector<std::pair<std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator>>
BynavParser::split(Buffer& buffer)
{
  // TODO(olek)
  static_cast<void>(buffer);
  return std::vector<std::pair<std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator>>{};
}
