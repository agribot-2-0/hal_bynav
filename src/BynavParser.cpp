/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include <hal_bynav/BynavParser.h>

BynavParser::BynavParser(std::unique_ptr<IParsingStrategy> strategy, std::unique_ptr<IFrameFactory> factory)
  : parsing_strategy_(std::move(strategy)), frame_factory_(std::move(factory))
{
}

void BynavParser::getFrameBuffersFromPacketBuffer(Buffer& packet_buffer)
{
  if (packet_buffer.getReadLength() > 0)
  {
    auto read_end_it = std::next(packet_buffer.buffer_.begin(), packet_buffer.getReadLength());
    auto it = packet_buffer.buffer_.begin();
    auto previous_it = packet_buffer.buffer_.begin();

    while (it != read_end_it)
    {
      it = std::search(it, read_end_it, std::begin(sync_bytes_), std::end(sync_bytes_));

      if (it != read_end_it)
      {
        if (std::distance(previous_it, it) > 0)
        {
          // TODO(olek) - maybe use make_move_iterator (https://en.cppreference.com/w/cpp/iterator/make_move_iterator)
          frame_buffers_.emplace(previous_it, it);
        }
        it += std::size(sync_bytes_);
        previous_it = it;
      }
      else
      {
        // Handles last frame if packet doesn't end with sync bytes
        if (previous_it != read_end_it)
        {
          frame_buffers_.emplace(previous_it, it);
        }
      }
    }
    packet_buffer.clear();
  }
}

bool BynavParser::haveFrameBufferToParse() const
{
  return (frame_buffers_.size() > 0);
}

void BynavParser::parseFrameBuffer()
{
  auto& frame_buffer = frame_buffers_.front();
  auto id = getId(frame_buffer);

  try
  {
    auto frame = frame_factory_->createFrame(id);
    frame->parse(frame_buffer.buffer_.begin(), frame_buffer.buffer_.end(), *parsing_strategy_);
    callCallbacks(id, *frame);
    frame_buffers_.pop();
  }
  catch (const std::exception& e)
  {
    frame_buffers_.pop();
    throw;
  }
}

void BynavParser::registerCallback(uint64_t id, const Callback& callback)
{
  callback_map_[id].push_back(callback);
}

void BynavParser::callCallbacks(uint64_t id, IFrame& frame)
{
  for (const auto& cb : callback_map_[id])
  {
    cb(frame);
  }
}

uint64_t BynavParser::getId(const Buffer& frame_buffer)
{
  return static_cast<uint64_t>(parsing_strategy_->getUint16(std::next(frame_buffer.buffer_.begin(), id_index_)));
}

std::queue<Buffer> BynavParser::getFrameBuffersQueue()
{
  return frame_buffers_;
}
