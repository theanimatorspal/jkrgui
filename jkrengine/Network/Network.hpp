#pragma once
#include <Window.hpp>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
namespace Jkr::Network {
class Connection;
class Message;
using OnClientValidationFunctionType    = std::function<void(sp<Connection>)>;
using OnClientConnectionFunctionType    = std::function<bool(sp<Connection>)>;
using OnClientDisConnectionFunctionType = std::function<void(sp<Connection>)>;
using OnMessageFunctionType             = std::function<void(sp<Connection>, Message&)>;
} // namespace Jkr::Network