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

#ifndef MOONRAKER_CLIENT_MOCK_H
#define MOONRAKER_CLIENT_MOCK_H

#include "hv/json.hpp"
#include <functional>
#include <string>
#include <vector>

using json = nlohmann::json;

/**
 * @brief Mock MoonrakerClient for testing wizard connection flow
 *
 * Simulates WebSocket connection behavior without real network I/O.
 * Allows tests to trigger connection success/failure and verify URL.
 */
class MoonrakerClientMock {
public:
    MoonrakerClientMock();
    ~MoonrakerClientMock() = default;

    /**
     * @brief Mock connection attempt (stores callbacks, does not connect)
     *
     * @param url WebSocket URL to connect to (stored for verification)
     * @param on_connected Callback to invoke when connection succeeds
     * @param on_disconnected Callback to invoke when connection fails/closes
     * @return Always returns 0 (success)
     */
    int connect(const char* url,
                std::function<void()> on_connected,
                std::function<void()> on_disconnected);

    /**
     * @brief Mock send_jsonrpc (no-op, stores method for verification)
     */
    int send_jsonrpc(const std::string& method, const json& params);

    /**
     * @brief Mock send_jsonrpc with callback
     */
    int send_jsonrpc(const std::string& method,
                     const json& params,
                     std::function<void(json&)> cb);

    /**
     * @brief Mock gcode_script (no-op)
     */
    int gcode_script(const std::string& gcode);

    /**
     * @brief Mock discover_printer (no-op)
     */
    void discover_printer(std::function<void()> on_complete);

    /**
     * @brief Check if connected
     */
    bool is_connected() const { return connected_; }

    // Test control methods

    /**
     * @brief Simulate successful connection (triggers on_connected callback)
     */
    void trigger_connected();

    /**
     * @brief Simulate connection failure (triggers on_disconnected callback)
     */
    void trigger_disconnected();

    /**
     * @brief Get the last URL passed to connect()
     */
    std::string get_last_connect_url() const { return last_url_; }

    /**
     * @brief Get all RPC methods called via send_jsonrpc
     */
    const std::vector<std::string>& get_rpc_methods() const { return rpc_methods_; }

    /**
     * @brief Reset mock state (clears callbacks, URL, methods)
     */
    void reset();

private:
    std::function<void()> connected_callback_;
    std::function<void()> disconnected_callback_;
    std::string last_url_;
    std::vector<std::string> rpc_methods_;
    bool connected_ = false;
};

#endif // MOONRAKER_CLIENT_MOCK_H
