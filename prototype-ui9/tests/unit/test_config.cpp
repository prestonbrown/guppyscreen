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
 */

#include "../catch_amalgamated.hpp"
#include "config.h"

// Test fixture for Config class testing
class ConfigTestFixture {
protected:
    Config config;

    // Helper methods to access protected members
    void set_data_null(const std::string& json_ptr) {
        config.data[json::json_pointer(json_ptr)] = nullptr;
    }

    void set_data_empty() {
        config.data = {};
        config.default_printer = "/printers/default/";
    }

    void setup_default_config() {
        // Manually populate config.data with realistic test JSON
        config.data = {
            {"default_printer", "test_printer"},
            {"printers", {
                {"test_printer", {
                    {"moonraker_host", "192.168.1.100"},
                    {"moonraker_port", 7125},
                    {"log_level", "debug"},
                    {"hardware_map", {
                        {"heated_bed", "heater_bed"},
                        {"hotend", "extruder"}
                    }}
                }}
            }}
        };
        config.default_printer = "/printers/test_printer/";
    }

    void setup_minimal_config() {
        // Minimal config for wizard testing (default host)
        config.data = {
            {"default_printer", "default_printer"},
            {"printers", {
                {"default_printer", {
                    {"moonraker_host", "127.0.0.1"},
                    {"moonraker_port", 7125}
                }}
            }}
        };
        config.default_printer = "/printers/default_printer/";
    }

    void setup_incomplete_config() {
        // Config missing hardware_map (should trigger wizard)
        config.data = {
            {"default_printer", "default_printer"},
            {"printers", {
                {"default_printer", {
                    {"moonraker_host", "192.168.1.50"},
                    {"moonraker_port", 7125}
                }}
            }}
        };
        config.default_printer = "/printers/default_printer/";
    }
};

// ============================================================================
// get() without default parameter - Existing behavior
// ============================================================================

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() returns existing string value", "[config][get]") {
    setup_default_config();

    std::string host = config.get<std::string>("/printers/test_printer/moonraker_host");
    REQUIRE(host == "192.168.1.100");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() returns existing int value", "[config][get]") {
    setup_default_config();

    int port = config.get<int>("/printers/test_printer/moonraker_port");
    REQUIRE(port == 7125);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() returns existing nested value", "[config][get]") {
    setup_default_config();

    std::string bed = config.get<std::string>("/printers/test_printer/hardware_map/heated_bed");
    REQUIRE(bed == "heater_bed");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with df() prefix returns value", "[config][get]") {
    setup_default_config();

    std::string host = config.get<std::string>(config.df() + "moonraker_host");
    REQUIRE(host == "192.168.1.100");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with missing key throws exception", "[config][get]") {
    setup_default_config();

    REQUIRE_THROWS_AS(config.get<std::string>("/printers/test_printer/nonexistent_key"),
                      nlohmann::detail::type_error);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with missing nested key throws exception", "[config][get]") {
    setup_default_config();

    REQUIRE_THROWS_AS(config.get<std::string>("/printers/test_printer/hardware_map/missing"),
                      nlohmann::detail::type_error);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with type mismatch throws exception", "[config][get]") {
    setup_default_config();

    // Try to get string value as int
    REQUIRE_THROWS(config.get<int>("/printers/test_printer/moonraker_host"));
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with object returns nested structure", "[config][get]") {
    setup_default_config();

    auto hardware_map = config.get<json>("/printers/test_printer/hardware_map");
    REQUIRE(hardware_map.is_object());
    REQUIRE(hardware_map["heated_bed"] == "heater_bed");
    REQUIRE(hardware_map["hotend"] == "extruder");
}

// ============================================================================
// get() with default parameter - NEW behavior
// ============================================================================

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default returns value when key exists (string)", "[config][get][default]") {
    setup_default_config();

    std::string host = config.get<std::string>("/printers/test_printer/moonraker_host", "default.local");
    REQUIRE(host == "192.168.1.100");  // Ignores default
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default returns value when key exists (int)", "[config][get][default]") {
    setup_default_config();

    int port = config.get<int>("/printers/test_printer/moonraker_port", 9999);
    REQUIRE(port == 7125);  // Ignores default
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default returns default when key missing (string)", "[config][get][default]") {
    setup_default_config();

    std::string printer_name = config.get<std::string>("/printers/test_printer/printer_name", "My Printer");
    REQUIRE(printer_name == "My Printer");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default returns default when key missing (int)", "[config][get][default]") {
    setup_default_config();

    int timeout = config.get<int>("/printers/test_printer/timeout", 30);
    REQUIRE(timeout == 30);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default returns default when key missing (bool)", "[config][get][default]") {
    setup_default_config();

    bool api_key = config.get<bool>("/printers/test_printer/moonraker_api_key", false);
    REQUIRE(api_key == false);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default handles nested missing path", "[config][get][default]") {
    setup_default_config();

    std::string led = config.get<std::string>("/printers/test_printer/hardware_map/main_led", "none");
    REQUIRE(led == "none");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with empty string default", "[config][get][default]") {
    setup_default_config();

    std::string empty = config.get<std::string>("/printers/test_printer/empty_field", "");
    REQUIRE(empty == "");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default using df() prefix", "[config][get][default]") {
    setup_default_config();

    std::string printer_name = config.get<std::string>(config.df() + "printer_name", "");
    REQUIRE(printer_name == "");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default handles completely missing parent path", "[config][get][default]") {
    setup_default_config();

    std::string missing = config.get<std::string>("/nonexistent/path/key", "fallback");
    REQUIRE(missing == "fallback");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default prevents crashes on null keys", "[config][get][default]") {
    setup_minimal_config();

    // This is the bug we fixed - printer_name doesn't exist, should return default not throw
    std::string printer_name = config.get<std::string>(config.df() + "printer_name", "");
    REQUIRE(printer_name == "");
}

// ============================================================================
// set() operations
// ============================================================================

TEST_CASE_METHOD(ConfigTestFixture, "Config: set() creates new top-level key", "[config][set]") {
    setup_default_config();

    config.set<std::string>("/new_key", "new_value");
    REQUIRE(config.get<std::string>("/new_key") == "new_value");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: set() updates existing key", "[config][set]") {
    setup_default_config();

    config.set<std::string>("/printers/test_printer/moonraker_host", "10.0.0.1");
    REQUIRE(config.get<std::string>("/printers/test_printer/moonraker_host") == "10.0.0.1");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: set() creates nested path", "[config][set]") {
    setup_default_config();

    config.set<std::string>("/printers/test_printer/hardware_map/main_led", "neopixel");
    REQUIRE(config.get<std::string>("/printers/test_printer/hardware_map/main_led") == "neopixel");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: set() updates nested value", "[config][set]") {
    setup_default_config();

    config.set<std::string>("/printers/test_printer/hardware_map/hotend", "extruder1");
    REQUIRE(config.get<std::string>("/printers/test_printer/hardware_map/hotend") == "extruder1");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: set() handles different types", "[config][set]") {
    setup_default_config();

    config.set<int>("/printers/test_printer/new_int", 42);
    config.set<bool>("/printers/test_printer/new_bool", true);
    config.set<std::string>("/printers/test_printer/new_string", "test");

    REQUIRE(config.get<int>("/printers/test_printer/new_int") == 42);
    REQUIRE(config.get<bool>("/printers/test_printer/new_bool") == true);
    REQUIRE(config.get<std::string>("/printers/test_printer/new_string") == "test");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: set() overwrites value of different type", "[config][set]") {
    setup_default_config();

    config.set<int>("/printers/test_printer/moonraker_port", 8080);
    REQUIRE(config.get<int>("/printers/test_printer/moonraker_port") == 8080);

    // Overwrite int with string
    config.set<std::string>("/printers/test_printer/moonraker_port", "9090");
    REQUIRE(config.get<std::string>("/printers/test_printer/moonraker_port") == "9090");
}

// ============================================================================
// is_wizard_required() logic
// ============================================================================

TEST_CASE_METHOD(ConfigTestFixture, "Config: is_wizard_required() returns true when host is 127.0.0.1", "[config][wizard]") {
    setup_minimal_config();

    REQUIRE(config.is_wizard_required() == true);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: is_wizard_required() returns true when hardware_map missing", "[config][wizard]") {
    setup_incomplete_config();

    REQUIRE(config.is_wizard_required() == true);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: is_wizard_required() returns true when hardware_map has null bed", "[config][wizard]") {
    setup_default_config();

    // Remove heated_bed from hardware_map
    set_data_null("/printers/test_printer/hardware_map/heated_bed");

    REQUIRE(config.is_wizard_required() == true);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: is_wizard_required() returns true when hardware_map has null hotend", "[config][wizard]") {
    setup_default_config();

    // Remove hotend from hardware_map
    set_data_null("/printers/test_printer/hardware_map/hotend");

    REQUIRE(config.is_wizard_required() == true);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: is_wizard_required() returns false when fully configured", "[config][wizard]") {
    setup_default_config();

    REQUIRE(config.is_wizard_required() == false);
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: is_wizard_required() returns true when moonraker_host is null", "[config][wizard]") {
    setup_default_config();

    set_data_null("/printers/test_printer/moonraker_host");

    REQUIRE(config.is_wizard_required() == true);
}

// ============================================================================
// Edge cases
// ============================================================================

TEST_CASE_METHOD(ConfigTestFixture, "Config: handles deeply nested structures", "[config][edge]") {
    setup_default_config();

    config.set<std::string>("/printers/test_printer/nested/level1/level2/level3", "deep");
    std::string deep = config.get<std::string>("/printers/test_printer/nested/level1/level2/level3");
    REQUIRE(deep == "deep");
}

TEST_CASE_METHOD(ConfigTestFixture, "Config: get() with default handles empty config", "[config][edge]") {
    // Empty config
    set_data_empty();

    std::string host = config.get<std::string>("/printers/default/moonraker_host", "localhost");
    REQUIRE(host == "localhost");
}
