/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <agribot_common/connection/IParser.h>

#include <algorithm>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class BynavParser : public IParser
{
  using CallbackMap = std::unordered_map<uint64_t, std::vector<Callback>>;

public:
  BynavParser(std::unique_ptr<IParsingStrategy> strategy, std::unique_ptr<IFrameFactory> factory);
  BynavParser() = delete;
  ~BynavParser() = default;
  BynavParser(const BynavParser& other) = default;
  BynavParser(BynavParser&& other) = default;
  BynavParser& operator=(const BynavParser& other) = default;
  BynavParser& operator=(BynavParser&& other) = default;

  void getFrameBuffersFromPacketBuffer(Buffer& packet_buffer) override;
  bool haveFrameBufferToParse() const override;
  void parseFrameBuffer() override;
  void registerCallback(uint64_t id, const Callback& callback) override;
  std::queue<Buffer> getFrameBuffersQueue();

  static constexpr uint8_t sync_bytes_[3] = { 0xAA, 0x44, 0x12 };

private:
  std::unique_ptr<IParsingStrategy> parsing_strategy_;
  std::unique_ptr<IFrameFactory> frame_factory_;
  CallbackMap callback_map_;
  std::queue<Buffer> frame_buffers_;

  static constexpr uint8_t id_index_ = { 4 };

  void callCallbacks(uint64_t id, IFrame& frame);  // NOLINT(runtime/references)
  uint64_t getId(const Buffer&);
};
