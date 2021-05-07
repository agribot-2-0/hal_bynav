/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <strings.h>

class BynavSocket
{
private:
  static constexpr std::size_t buffer_size_ = 1024;

  int socket_fd_;

  uint8_t buffer_[buffer_size_];
  int buffer_length_;

public:
  BynavSocket();
  ~BynavSocket();
  void connectSocket(const std::string& ip_address, std::uint16_t port);
  int read();
  uint8_t* getBuffer();
};
