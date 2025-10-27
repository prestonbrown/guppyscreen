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

#ifndef LVGL_MOCK_H
#define LVGL_MOCK_H

#include <cstdint>
#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations for LVGL types (minimal subset for wizard testing)
typedef void lv_obj_t;
typedef void lv_subject_t;
typedef void lv_timer_t;

// Event types
typedef enum {
    LV_EVENT_CLICKED = 0,
    LV_EVENT_VALUE_CHANGED,
    LV_EVENT_FOCUSED,
    LV_EVENT_DEFOCUSED,
    LV_EVENT_READY,
} lv_event_code_t;

// Event structure
typedef struct {
    lv_obj_t* target;
    lv_event_code_t code;
    void* user_data;
    void* param;
} lv_event_t;

// Callback types
typedef void (*lv_event_cb_t)(lv_event_t* e);
typedef void (*lv_observer_cb_t)(lv_subject_t* subject, void* user_data);
typedef void (*lv_timer_cb_t)(lv_timer_t* timer);

/**
 * @brief Minimal LVGL mock for wizard UI testing
 *
 * Only mocks functions used by wizard connection screen:
 * - Widget creation/lookup
 * - Textarea operations
 * - Subject/observer system
 * - Event system
 * - Timers (for timeout testing)
 */
namespace LVGLMock {

/**
 * @brief Mock widget storage
 */
struct MockWidget {
    std::string name;
    std::string text_value;  // For textareas
    std::vector<std::function<void(lv_event_t*)>> event_callbacks;
    void* user_data = nullptr;
};

/**
 * @brief Mock subject storage
 */
struct MockSubject {
    std::string name;
    std::string value;
    char* buffer = nullptr;
    size_t buffer_size = 0;
};

/**
 * @brief Mock timer storage
 */
struct MockTimer {
    lv_timer_cb_t callback = nullptr;
    uint32_t period = 0;
    void* user_data = nullptr;
};

// Global mock state
extern std::unordered_map<lv_obj_t*, MockWidget> widgets;
extern std::unordered_map<lv_subject_t*, MockSubject> subjects;
extern std::unordered_map<std::string, lv_subject_t*> subject_registry;
extern std::unordered_map<lv_timer_t*, MockTimer> timers;
extern lv_obj_t* active_screen;
extern uint32_t mock_tick;

// Test control API

/**
 * @brief Initialize mock LVGL system
 */
void init();

/**
 * @brief Reset all mock state (clears widgets, subjects, timers)
 */
void reset();

/**
 * @brief Set textarea value by widget name
 *
 * @param name Widget name (from lv_obj_find_by_name)
 * @param value Text to set
 */
void set_textarea_value(const std::string& name, const std::string& value);

/**
 * @brief Get textarea value by widget name
 *
 * @param name Widget name
 * @return Current text value, or empty string if not found
 */
std::string get_textarea_value(const std::string& name);

/**
 * @brief Get subject value by registered name
 *
 * @param name Subject name (from lv_xml_register_subject)
 * @return Current subject value, or empty string if not found
 */
std::string get_subject_value(const std::string& name);

/**
 * @brief Trigger button click event
 *
 * @param button_name Widget name of button to click
 */
void trigger_button_click(const std::string& button_name);

/**
 * @brief Advance mock time (for timeout testing)
 *
 * @param ms Milliseconds to advance
 */
void advance_time(uint32_t ms);

/**
 * @brief Run all timer callbacks that are due
 */
void process_timers();

/**
 * @brief Get widget by name
 *
 * @param name Widget name
 * @return Widget pointer or nullptr if not found
 */
lv_obj_t* find_widget_by_name(const std::string& name);

} // namespace LVGLMock

// LVGL API mock functions (C linkage for compatibility)
#ifdef __cplusplus
extern "C" {
#endif

// Widget creation & lookup
lv_obj_t* lv_xml_create(lv_obj_t* parent, const char* component, void* user_data);
lv_obj_t* lv_obj_find_by_name(lv_obj_t* parent, const char* name);
lv_obj_t* lv_scr_act();

// Textarea operations
const char* lv_textarea_get_text(lv_obj_t* obj);
void lv_textarea_set_text(lv_obj_t* obj, const char* text);

// Subject/observer system
void lv_subject_init_string(lv_subject_t* subject, char* buffer,
                            lv_observer_cb_t observer_cb, size_t size, const char* init_value);
void lv_xml_register_subject(void* ctx, const char* name, lv_subject_t* subject);
void lv_subject_copy_string(lv_subject_t* subject, const char* value);

// Event system
void lv_obj_add_event_cb(lv_obj_t* obj, lv_event_cb_t event_cb, lv_event_code_t filter, void* user_data);
void lv_event_send(lv_obj_t* obj, lv_event_code_t code, void* param);
void lv_xml_register_event_cb(void* ctx, const char* name, lv_event_cb_t cb);

// Timer system
uint32_t lv_tick_get();
lv_timer_t* lv_timer_create(lv_timer_cb_t timer_cb, uint32_t period, void* user_data);
void lv_timer_del(lv_timer_t* timer);

#ifdef __cplusplus
}
#endif

#endif // LVGL_MOCK_H
