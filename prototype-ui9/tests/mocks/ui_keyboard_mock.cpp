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

#include "lvgl_mock.h"
#include <spdlog/spdlog.h>

// Mock keyboard types (needed for ui_keyboard.h compatibility)
typedef enum {
    LV_KEYBOARD_MODE_TEXT_LOWER,
    LV_KEYBOARD_MODE_TEXT_UPPER,
    LV_KEYBOARD_MODE_SPECIAL,
    LV_KEYBOARD_MODE_NUMBER
} lv_keyboard_mode_t;

typedef enum {
    LV_ALIGN_BOTTOM_MID,
    LV_ALIGN_RIGHT_MID
} lv_align_t;

// Mock implementation of ui_keyboard functions (no-op for testing)

void ui_keyboard_init(lv_obj_t* parent) {
    (void)parent;
    spdlog::debug("[MockKeyboard] ui_keyboard_init()");
}

void ui_keyboard_register_textarea(lv_obj_t* textarea) {
    (void)textarea;
    spdlog::debug("[MockKeyboard] ui_keyboard_register_textarea()");
}

void ui_keyboard_show(lv_obj_t* textarea) {
    (void)textarea;
    spdlog::debug("[MockKeyboard] ui_keyboard_show()");
}

void ui_keyboard_hide() {
    spdlog::debug("[MockKeyboard] ui_keyboard_hide()");
}

bool ui_keyboard_is_visible() {
    return false;  // Always hidden in tests
}

lv_obj_t* ui_keyboard_get_instance() {
    return nullptr;  // No keyboard instance in tests
}

void ui_keyboard_set_mode(lv_keyboard_mode_t mode) {
    (void)mode;
    spdlog::debug("[MockKeyboard] ui_keyboard_set_mode()");
}

void ui_keyboard_set_position(lv_align_t align, int32_t x_ofs, int32_t y_ofs) {
    (void)align;
    (void)x_ofs;
    (void)y_ofs;
    spdlog::debug("[MockKeyboard] ui_keyboard_set_position()");
}
