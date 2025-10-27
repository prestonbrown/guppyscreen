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

#include "catch_amalgamated.hpp"
#include "../mocks/lvgl_mock.h"
#include "../mocks/moonraker_client_mock.h"
#include <string>

/**
 * @brief Example tests demonstrating mock usage
 *
 * These tests show how to use MoonrakerClientMock and LVGLMock
 * for wizard UI integration testing without real WebSocket or LVGL.
 */

TEST_CASE("MoonrakerClientMock basic usage", "[mock][example]") {
    MoonrakerClientMock mock_client;

    SECTION("Connection tracking") {
        bool connected = false;
        bool disconnected = false;

        // Setup callbacks
        auto on_connected = [&]() { connected = true; };
        auto on_disconnected = [&]() { disconnected = true; };

        // Attempt connection
        int result = mock_client.connect("ws://192.168.1.100:7125/websocket",
                                         on_connected,
                                         on_disconnected);

        REQUIRE(result == 0);  // Always succeeds
        REQUIRE(mock_client.get_last_connect_url() == "ws://192.168.1.100:7125/websocket");
        REQUIRE_FALSE(connected);  // Not called until triggered
        REQUIRE_FALSE(disconnected);

        // Trigger success
        mock_client.trigger_connected();
        REQUIRE(connected);
        REQUIRE_FALSE(disconnected);
        REQUIRE(mock_client.is_connected());
    }

    SECTION("Connection failure") {
        bool connected = false;
        bool disconnected = false;

        auto on_connected = [&]() { connected = true; };
        auto on_disconnected = [&]() { disconnected = true; };

        mock_client.connect("ws://invalid:7125/websocket",
                           on_connected,
                           on_disconnected);

        // Trigger failure
        mock_client.trigger_disconnected();
        REQUIRE_FALSE(connected);
        REQUIRE(disconnected);
        REQUIRE_FALSE(mock_client.is_connected());
    }

    SECTION("RPC method tracking") {
        json params = {{"script", "G28"}};

        mock_client.send_jsonrpc("printer.gcode.script", params);
        mock_client.send_jsonrpc("printer.info", json{});

        const auto& methods = mock_client.get_rpc_methods();
        REQUIRE(methods.size() == 2);
        REQUIRE(methods[0] == "printer.gcode.script");
        REQUIRE(methods[1] == "printer.info");
    }

    SECTION("Reset clears state") {
        mock_client.connect("ws://test:7125/websocket", [](){}, [](){});
        mock_client.send_jsonrpc("test.method", json{});
        mock_client.trigger_connected();

        REQUIRE(mock_client.is_connected());
        REQUIRE_FALSE(mock_client.get_last_connect_url().empty());
        REQUIRE_FALSE(mock_client.get_rpc_methods().empty());

        mock_client.reset();

        REQUIRE_FALSE(mock_client.is_connected());
        REQUIRE(mock_client.get_last_connect_url().empty());
        REQUIRE(mock_client.get_rpc_methods().empty());
    }
}

TEST_CASE("LVGLMock basic usage", "[mock][example]") {
    LVGLMock::init();

    SECTION("Textarea operations") {
        // Create widget on-demand via find_by_name
        lv_obj_t* ip_input = lv_obj_find_by_name(nullptr, "ip_input");
        REQUIRE(ip_input != nullptr);

        // Set value
        lv_textarea_set_text(ip_input, "192.168.1.100");

        // Get value via widget pointer
        const char* text = lv_textarea_get_text(ip_input);
        REQUIRE(std::string(text) == "192.168.1.100");

        // Get value via test API
        std::string value = LVGLMock::get_textarea_value("ip_input");
        REQUIRE(value == "192.168.1.100");
    }

    SECTION("Test control API") {
        // Set value directly (bypasses LVGL API)
        LVGLMock::set_textarea_value("port_input", "7125");

        // Verify via LVGL API
        lv_obj_t* port_input = lv_obj_find_by_name(nullptr, "port_input");
        REQUIRE(std::string(lv_textarea_get_text(port_input)) == "7125");
    }

    SECTION("Button click simulation") {
        bool clicked = false;

        // Find button
        lv_obj_t* btn = lv_obj_find_by_name(nullptr, "test_button");

        // Add event callback
        lv_obj_add_event_cb(btn, [](lv_event_t* e) {
            bool* flag = static_cast<bool*>(e->user_data);
            if (e->code == LV_EVENT_CLICKED) {
                *flag = true;
            }
        }, LV_EVENT_CLICKED, &clicked);

        // Trigger click
        REQUIRE_FALSE(clicked);
        LVGLMock::trigger_button_click("test_button");
        REQUIRE(clicked);
    }

    SECTION("Subject operations") {
        char buffer[128];
        // In mock, subject is just an opaque pointer - use dummy allocation
        lv_subject_t* subject = reinterpret_cast<lv_subject_t*>(new char[1]);

        // Initialize subject
        lv_subject_init_string(subject, buffer, nullptr, sizeof(buffer), "Initial");
        REQUIRE(std::string(buffer) == "Initial");

        // Register subject
        lv_xml_register_subject(nullptr, "status", subject);

        // Update subject
        lv_subject_copy_string(subject, "Updated");
        REQUIRE(std::string(buffer) == "Updated");

        // Verify via test API
        std::string value = LVGLMock::get_subject_value("status");
        REQUIRE(value == "Updated");

        delete[] reinterpret_cast<char*>(subject);
    }

    SECTION("Timer operations") {
        int call_count = 0;

        // Create timer
        lv_timer_t* timer = lv_timer_create([](lv_timer_t* t) {
            int* count = static_cast<int*>(LVGLMock::timers[t].user_data);
            (*count)++;
        }, 1000, &call_count);

        REQUIRE(timer != nullptr);
        REQUIRE(call_count == 0);

        // Process timers
        LVGLMock::process_timers();
        REQUIRE(call_count == 1);

        // Delete timer
        lv_timer_del(timer);
        LVGLMock::process_timers();
        REQUIRE(call_count == 1);  // Not called after deletion
    }

    SECTION("Time advancement") {
        uint32_t start = lv_tick_get();
        REQUIRE(start == 0);  // Starts at 0

        LVGLMock::advance_time(5000);
        REQUIRE(lv_tick_get() == 5000);

        LVGLMock::advance_time(3000);
        REQUIRE(lv_tick_get() == 8000);
    }

    SECTION("Reset clears state") {
        LVGLMock::set_textarea_value("test", "value");
        LVGLMock::advance_time(1000);

        lv_subject_t* subject = reinterpret_cast<lv_subject_t*>(new char[1]);
        char buffer[32];
        lv_subject_init_string(subject, buffer, nullptr, sizeof(buffer), "test");
        lv_xml_register_subject(nullptr, "test_subject", subject);

        REQUIRE_FALSE(LVGLMock::get_textarea_value("test").empty());
        REQUIRE(lv_tick_get() == 1000);
        REQUIRE_FALSE(LVGLMock::get_subject_value("test_subject").empty());

        LVGLMock::reset();

        REQUIRE(LVGLMock::get_textarea_value("test").empty());
        REQUIRE(lv_tick_get() == 0);
        REQUIRE(LVGLMock::get_subject_value("test_subject").empty());

        delete[] reinterpret_cast<char*>(subject);
    }
}

TEST_CASE("Combined mock usage", "[mock][example]") {
    LVGLMock::init();
    MoonrakerClientMock mock_client;

    SECTION("Simulated connection test flow") {
        // Setup UI
        LVGLMock::set_textarea_value("ip_input", "192.168.1.100");
        LVGLMock::set_textarea_value("port_input", "7125");

        // Setup status subject
        char status_buffer[256];
        lv_subject_t* status = reinterpret_cast<lv_subject_t*>(new char[1]);
        lv_subject_init_string(status, status_buffer, nullptr, sizeof(status_buffer), "");
        lv_xml_register_subject(nullptr, "connection_status", status);

        // Simulate connection test button click
        bool connection_attempted = false;

        lv_obj_t* btn = lv_obj_find_by_name(nullptr, "btn_test_connection");
        lv_obj_add_event_cb(btn, [](lv_event_t* e) {
            // In real code, this would read textareas and call mock_client.connect()
            bool* flag = static_cast<bool*>(e->user_data);
            *flag = true;
        }, LV_EVENT_CLICKED, &connection_attempted);

        LVGLMock::trigger_button_click("btn_test_connection");
        REQUIRE(connection_attempted);

        // Simulate actual connection attempt
        std::string ip = LVGLMock::get_textarea_value("ip_input");
        std::string port = LVGLMock::get_textarea_value("port_input");
        std::string url = "ws://" + ip + ":" + port + "/websocket";

        mock_client.connect(url.c_str(),
            [&]() {
                // On connected
                lv_subject_copy_string(status, "Connected!");
            },
            [&]() {
                // On disconnected
                lv_subject_copy_string(status, "Connection failed");
            });

        REQUIRE(mock_client.get_last_connect_url() == "ws://192.168.1.100:7125/websocket");

        // Simulate success
        mock_client.trigger_connected();
        REQUIRE(LVGLMock::get_subject_value("connection_status") == "Connected!");

        // Cleanup
        delete[] reinterpret_cast<char*>(status);
        LVGLMock::reset();
        mock_client.reset();
    }
}
