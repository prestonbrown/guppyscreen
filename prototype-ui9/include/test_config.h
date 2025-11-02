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

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

/**
 * @brief Test mode configuration for development and testing
 *
 * Controls which components use mock implementations vs real hardware.
 * In production mode (test_mode=false), NO mocks are ever used.
 * In test mode, mocks are used by default but can be overridden with --real-* flags.
 */
struct TestConfig {
    // Master test mode flag (--test)
    bool test_mode = false;

    // Individual component overrides (require --test to be set)
    bool use_real_wifi = false;       // --real-wifi
    bool use_real_ethernet = false;   // --real-ethernet
    bool use_real_moonraker = false;  // --real-moonraker
    bool use_real_files = false;      // --real-files

    /**
     * @brief Check if WiFi should use mock implementation
     * @return true if test mode is enabled and real WiFi is not requested
     */
    bool should_mock_wifi() const {
        return test_mode && !use_real_wifi;
    }

    /**
     * @brief Check if Ethernet should use mock implementation
     * @return true if test mode is enabled and real Ethernet is not requested
     */
    bool should_mock_ethernet() const {
        return test_mode && !use_real_ethernet;
    }

    /**
     * @brief Check if Moonraker should use mock implementation
     * @return true if test mode is enabled and real Moonraker is not requested
     */
    bool should_mock_moonraker() const {
        return test_mode && !use_real_moonraker;
    }

    /**
     * @brief Check if file list should use test data
     * @return true if test mode is enabled and real files are not requested
     */
    bool should_use_test_files() const {
        return test_mode && !use_real_files;
    }

    /**
     * @brief Check if we're in any form of test mode
     * @return true if test mode is enabled
     */
    bool is_test_mode() const {
        return test_mode;
    }
};

/**
 * @brief Get global test configuration
 * @return Reference to the global test configuration
 */
const TestConfig& get_test_config();

/**
 * @brief Get mutable test configuration (for initialization only)
 * @return Pointer to the global test configuration
 */
TestConfig* get_mutable_test_config();

#endif // TEST_CONFIG_H