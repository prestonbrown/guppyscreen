/*
 * Copyright (C) 2025 356C LLC
 * Author: Preston Brown <pbrown@brown-house.net>
 *
 * This file is part of HelixScreen.
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

#include "moonraker_client_mock.h"
#include <spdlog/spdlog.h>

MoonrakerClientMock::MoonrakerClientMock() {
    reset();
}

int MoonrakerClientMock::connect(const char* url,
                                  std::function<void()> on_connected,
                                  std::function<void()> on_disconnected) {
    spdlog::debug("[MockMR] connect() called: {}", url);
    last_url_ = url;
    connected_callback_ = on_connected;
    disconnected_callback_ = on_disconnected;
    return 0;  // Success
}

int MoonrakerClientMock::send_jsonrpc(const std::string& method, const json& params) {
    (void)params;  // Not used in mock
    spdlog::debug("[MockMR] send_jsonrpc() called: {}", method);
    rpc_methods_.push_back(method);
    return 0;
}

int MoonrakerClientMock::send_jsonrpc(const std::string& method,
                                       const json& params,
                                       std::function<void(json&)> cb) {
    (void)params;  // Not used in mock
    (void)cb;      // Could store for later triggering, but not needed yet
    spdlog::debug("[MockMR] send_jsonrpc() with callback called: {}", method);
    rpc_methods_.push_back(method);
    return 0;
}

int MoonrakerClientMock::gcode_script(const std::string& gcode) {
    spdlog::debug("[MockMR] gcode_script() called: {}", gcode);
    return 0;
}

void MoonrakerClientMock::discover_printer(std::function<void()> on_complete) {
    (void)on_complete;  // Could trigger for testing, but not needed yet
    spdlog::debug("[MockMR] discover_printer() called");
    // No-op for now
}

void MoonrakerClientMock::trigger_connected() {
    spdlog::debug("[MockMR] trigger_connected() - simulating successful connection");
    connected_ = true;
    if (connected_callback_) {
        connected_callback_();
    }
}

void MoonrakerClientMock::trigger_disconnected() {
    spdlog::debug("[MockMR] trigger_disconnected() - simulating connection failure");
    connected_ = false;
    if (disconnected_callback_) {
        disconnected_callback_();
    }
}

void MoonrakerClientMock::reset() {
    spdlog::debug("[MockMR] reset() - clearing all mock state");
    connected_callback_ = nullptr;
    disconnected_callback_ = nullptr;
    last_url_.clear();
    rpc_methods_.clear();
    connected_ = false;
}
