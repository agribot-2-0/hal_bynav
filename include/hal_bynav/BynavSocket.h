/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>

#include <stdexcept>
#include <string>

class BynavSocket
{
private:
  static constexpr std::size_t buffer_size_ = 1024;

  int socket_fd_;

  uint8_t buffer_[buffer_size_];  // NOLINT(runtime/arrays)
  int buffer_length_;

public:
  BynavSocket();
  ~BynavSocket();
  void createSocket();
  void connectSocket(const std::string& ip_address, std::uint16_t port);
  void closeSocket();
  int read();
  uint8_t* getBuffer();
};
