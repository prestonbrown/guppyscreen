/*
 * Copyright (C) 2025 356C LLC
 * Author: Preston Brown <pbrown@brown-house.net>
 *
 * This file is part of HelixScreen.
 * Based on GuppyScreen WebSocket client implementation.
 *
 * HelixScreen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HelixScreen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HelixScreen. If not, see <https://www.gnu.org/licenses/>.
 */

#include "moonraker_client.h"

using namespace hv;

MoonrakerClient::MoonrakerClient(EventLoopPtr loop)
    : WebSocketClient(loop)
    , request_id_(0) {
}

MoonrakerClient::~MoonrakerClient() {
}

int MoonrakerClient::connect(const char* url,
                               std::function<void()> on_connected,
                               std::function<void()> on_disconnected) {
  spdlog::debug("Moonraker WebSocket connecting to {}", url);

  // Connection opened callback
  onopen = [this, on_connected]() {
    const HttpResponsePtr& resp = getHttpResponse();
    spdlog::info("Moonraker WebSocket connected: {}", resp->body.c_str());
    on_connected();
  };

  // Message received callback
  onmessage = [this, on_connected, on_disconnected](const std::string& msg) {
    // Parse JSON message
    json j;
    try {
      j = json::parse(msg);
    } catch (const json::parse_error& e) {
      spdlog::error("JSON parse error: {}", e.what());
      return;
    }

    // Handle responses with request IDs (one-time callbacks)
    if (j.contains("id")) {
      uint32_t id = j["id"].get<uint32_t>();
      auto it = callbacks_.find(id);
      if (it != callbacks_.end()) {
        it->second(j);           // Invoke callback
        callbacks_.erase(it);     // Remove after execution
      }
    }

    // Handle notifications (no request ID)
    if (j.contains("method")) {
      std::string method = j["method"].get<std::string>();

      // Printer status updates (most common)
      if (method == "notify_status_update") {
        for (auto& cb : notify_callbacks_) {
          cb(j);
        }
      }
      // File list changes
      else if (method == "notify_filelist_changed") {
        for (auto& cb : notify_callbacks_) {
          cb(j);
        }
      }
      // Klippy disconnected from Moonraker
      else if (method == "notify_klippy_disconnected") {
        spdlog::warn("Klipper disconnected from Moonraker");
        on_disconnected();
      }
      // Klippy reconnected to Moonraker
      else if (method == "notify_klippy_ready") {
        spdlog::info("Klipper ready");
        on_connected();
      }

      // Method-specific persistent callbacks
      auto method_it = method_callbacks_.find(method);
      if (method_it != method_callbacks_.end()) {
        for (auto& [handler_name, cb] : method_it->second) {
          cb(j);
        }
      }
    }
  };

  // Connection closed callback
  onclose = [on_disconnected]() {
    spdlog::warn("Moonraker WebSocket connection closed");
    on_disconnected();
  };

  // WebSocket ping (keepalive)
  setPingInterval(10000);  // 10 seconds

  // Automatic reconnection with exponential backoff
  reconn_setting_t reconn;
  reconn_setting_init(&reconn);
  reconn.min_delay = 200;     // Start at 200ms
  reconn.max_delay = 2000;    // Max 2 seconds
  reconn.delay_policy = 2;    // Exponential backoff
  setReconnect(&reconn);

  // Connect
  http_headers headers;
  return open(url, headers);
}

void MoonrakerClient::register_notify_update(std::function<void(json&)> cb) {
  notify_callbacks_.push_back(cb);
}

void MoonrakerClient::register_method_callback(const std::string& method,
                                                const std::string& handler_name,
                                                std::function<void(json&)> cb) {
  auto it = method_callbacks_.find(method);
  if (it == method_callbacks_.end()) {
    spdlog::debug("Registering new method callback: {} (handler: {})",
                  method, handler_name);
    std::map<std::string, std::function<void(json&)>> handlers;
    handlers.insert({handler_name, cb});
    method_callbacks_.insert({method, handlers});
  } else {
    spdlog::debug("Adding handler to existing method {}: {}",
                  method, handler_name);
    it->second.insert({handler_name, cb});
  }
}

int MoonrakerClient::send_jsonrpc(const std::string& method) {
  json rpc;
  rpc["jsonrpc"] = "2.0";
  rpc["method"] = method;
  rpc["id"] = request_id_++;

  spdlog::debug("send_jsonrpc: {}", rpc.dump());
  return send(rpc.dump());
}

int MoonrakerClient::send_jsonrpc(const std::string& method, const json& params) {
  json rpc;
  rpc["jsonrpc"] = "2.0";
  rpc["method"] = method;
  rpc["params"] = params;
  rpc["id"] = request_id_++;

  spdlog::debug("send_jsonrpc: {}", rpc.dump());
  return send(rpc.dump());
}

int MoonrakerClient::send_jsonrpc(const std::string& method,
                                   const json& params,
                                   std::function<void(json&)> cb) {
  uint32_t id = request_id_;

  // Register callback for this request ID
  auto it = callbacks_.find(id);
  if (it == callbacks_.end()) {
    callbacks_.insert({id, cb});
    return send_jsonrpc(method, params);
  } else {
    spdlog::warn("Request ID {} already has a registered callback", id);
    return -1;
  }
}

int MoonrakerClient::gcode_script(const std::string& gcode) {
  json params = {{"script", gcode}};
  return send_jsonrpc("printer.gcode.script", params);
}
