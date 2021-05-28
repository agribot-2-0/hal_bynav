/*
 * Copyright (C) 2021 Agribot. All rights reserved.
 */

#include "hal_bynav/BynavSocket.h"

BynavSocket::BynavSocket()
{
}

BynavSocket::~BynavSocket()
{
  closeSocket();
}

void BynavSocket::createSocket()
{
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_fd_ < 0)
  {
    throw std::runtime_error("Failed to create socket");
  }

  // Set 1 second timeout
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 500000;
  setsockopt(socket_fd_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}

void BynavSocket::connectSocket(const std::string& ip_address, std::uint16_t port)
{
  struct sockaddr_in server_address;
  server_address.sin_addr.s_addr = inet_addr(ip_address.c_str());
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);

  // Connect to the socket
  if (connect(socket_fd_, (struct sockaddr*)&server_address, sizeof(server_address)) != 0)
  {
    throw std::runtime_error("Failed to connect to socket at " + ip_address + ":" + std::to_string(port));
  }
}

void BynavSocket::closeSocket()
{
  // Close socket file descriptor
  close(socket_fd_);
}

int BynavSocket::read()
{
  // Clear buffer
  bzero(buffer_, buffer_size_);

  // Read
  buffer_length_ = recv(socket_fd_, buffer_, buffer_size_, 0);

  if (buffer_length_ < 0)
  {
    throw std::runtime_error("Error receiving from socket (recv return < 0)");
  }
  else if (buffer_length_ == 0)
  {
    throw std::runtime_error("No message available on a socket");
  }
  return buffer_length_;
}

uint8_t* BynavSocket::getBuffer()
{
  return buffer_;
}
