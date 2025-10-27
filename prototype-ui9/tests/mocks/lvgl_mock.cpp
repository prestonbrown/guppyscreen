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
#include <cstring>
#include <algorithm>

namespace LVGLMock {

// Global mock state
std::unordered_map<lv_obj_t*, MockWidget> widgets;
std::unordered_map<lv_subject_t*, MockSubject> subjects;
std::unordered_map<std::string, lv_subject_t*> subject_registry;
std::unordered_map<lv_timer_t*, MockTimer> timers;
lv_obj_t* active_screen = nullptr;
uint32_t mock_tick = 0;

// Internal counter for generating unique widget/subject pointers
static uint64_t widget_counter = 1;
static uint64_t timer_counter = 1;

void init() {
    spdlog::debug("[MockLVGL] Initializing mock LVGL system");
    reset();
    // Create active screen
    active_screen = reinterpret_cast<lv_obj_t*>(widget_counter++);
    widgets[active_screen] = MockWidget{"screen", "", {}};
}

void reset() {
    spdlog::debug("[MockLVGL] Resetting all mock state");
    widgets.clear();
    subjects.clear();
    subject_registry.clear();
    timers.clear();
    active_screen = nullptr;
    mock_tick = 0;
}

void set_textarea_value(const std::string& name, const std::string& value) {
    spdlog::debug("[MockLVGL] set_textarea_value('{}', '{}')", name, value);
    for (auto& [obj, widget] : widgets) {
        if (widget.name == name) {
            widget.text_value = value;
            return;
        }
    }
    // Widget not found - create it on-demand
    spdlog::debug("[MockLVGL] Widget '{}' not found - creating on-demand", name);
    lv_obj_t* obj = reinterpret_cast<lv_obj_t*>(widget_counter++);
    widgets[obj] = MockWidget{name, value, {}};
}

std::string get_textarea_value(const std::string& name) {
    for (const auto& [obj, widget] : widgets) {
        if (widget.name == name) {
            return widget.text_value;
        }
    }
    return "";
}

std::string get_subject_value(const std::string& name) {
    auto it = subject_registry.find(name);
    if (it != subject_registry.end()) {
        auto subj_it = subjects.find(it->second);
        if (subj_it != subjects.end()) {
            return subj_it->second.value;
        }
    }
    return "";
}

void trigger_button_click(const std::string& button_name) {
    spdlog::debug("[MockLVGL] trigger_button_click('{}')", button_name);
    for (auto& [obj, widget] : widgets) {
        if (widget.name == button_name) {
            lv_event_t e;
            e.target = obj;
            e.code = LV_EVENT_CLICKED;
            e.user_data = widget.user_data;
            e.param = nullptr;

            for (auto& cb : widget.event_callbacks) {
                cb(&e);
            }
            return;
        }
    }
    spdlog::warn("[MockLVGL] Button '{}' not found", button_name);
}

void advance_time(uint32_t ms) {
    spdlog::debug("[MockLVGL] advance_time({}ms)", ms);
    mock_tick += ms;
}

void process_timers() {
    spdlog::debug("[MockLVGL] process_timers() - checking {} timers", timers.size());
    for (auto& [timer, timer_data] : timers) {
        if (timer_data.callback) {
            timer_data.callback(timer);
        }
    }
}

lv_obj_t* find_widget_by_name(const std::string& name) {
    for (auto& [obj, widget] : widgets) {
        if (widget.name == name) {
            return obj;
        }
    }
    return nullptr;
}

} // namespace LVGLMock

// LVGL API mock implementations

extern "C" {

lv_obj_t* lv_xml_create(lv_obj_t* parent, const char* component, void* user_data) {
    spdlog::debug("[MockLVGL] lv_xml_create('{}', parent={})", component, (void*)parent);

    // Create new widget pointer
    lv_obj_t* obj = reinterpret_cast<lv_obj_t*>(LVGLMock::widget_counter++);

    LVGLMock::MockWidget widget;
    widget.name = component;
    widget.user_data = user_data;

    LVGLMock::widgets[obj] = widget;

    return obj;
}

lv_obj_t* lv_obj_find_by_name(lv_obj_t* parent, const char* name) {
    (void)parent;  // Unused - we search globally for simplicity
    spdlog::debug("[MockLVGL] lv_obj_find_by_name('{}')", name);

    std::string name_str(name);
    for (auto& [obj, widget] : LVGLMock::widgets) {
        if (widget.name == name_str) {
            spdlog::debug("[MockLVGL]   Found: {}", (void*)obj);
            return obj;
        }
    }

    // Widget not found - create it on-demand for convenience
    spdlog::debug("[MockLVGL]   Not found - creating on-demand");
    lv_obj_t* obj = reinterpret_cast<lv_obj_t*>(LVGLMock::widget_counter++);
    LVGLMock::widgets[obj] = LVGLMock::MockWidget{name_str, "", {}};
    return obj;
}

lv_obj_t* lv_scr_act() {
    return LVGLMock::active_screen;
}

const char* lv_textarea_get_text(lv_obj_t* obj) {
    auto it = LVGLMock::widgets.find(obj);
    if (it != LVGLMock::widgets.end()) {
        spdlog::debug("[MockLVGL] lv_textarea_get_text('{}') = '{}'",
                     it->second.name, it->second.text_value);
        return it->second.text_value.c_str();
    }
    spdlog::warn("[MockLVGL] lv_textarea_get_text() - widget not found");
    return "";
}

void lv_textarea_set_text(lv_obj_t* obj, const char* text) {
    auto it = LVGLMock::widgets.find(obj);
    if (it != LVGLMock::widgets.end()) {
        spdlog::debug("[MockLVGL] lv_textarea_set_text('{}', '{}')",
                     it->second.name, text);
        it->second.text_value = text;
    } else {
        spdlog::warn("[MockLVGL] lv_textarea_set_text() - widget not found");
    }
}

void lv_subject_init_string(lv_subject_t* subject, char* buffer,
                            lv_observer_cb_t observer_cb, size_t size, const char* init_value) {
    (void)observer_cb;  // Not used in mock - observers not implemented
    spdlog::debug("[MockLVGL] lv_subject_init_string(init_value='{}')", init_value ? init_value : "");

    LVGLMock::MockSubject subj;
    subj.buffer = buffer;
    subj.buffer_size = size;
    subj.value = init_value ? init_value : "";

    // Copy initial value to buffer if provided
    if (buffer && init_value) {
        strncpy(buffer, init_value, size - 1);
        buffer[size - 1] = '\0';
    }

    LVGLMock::subjects[subject] = subj;
}

void lv_xml_register_subject(void* ctx, const char* name, lv_subject_t* subject) {
    (void)ctx;  // XML context not used in mock
    spdlog::debug("[MockLVGL] lv_xml_register_subject('{}')", name);
    LVGLMock::subject_registry[name] = subject;

    auto it = LVGLMock::subjects.find(subject);
    if (it != LVGLMock::subjects.end()) {
        it->second.name = name;
    }
}

void lv_subject_copy_string(lv_subject_t* subject, const char* value) {
    auto it = LVGLMock::subjects.find(subject);
    if (it != LVGLMock::subjects.end()) {
        spdlog::debug("[MockLVGL] lv_subject_copy_string('{}', '{}')",
                     it->second.name, value);
        it->second.value = value;

        // Also copy to buffer if present
        if (it->second.buffer) {
            strncpy(it->second.buffer, value, it->second.buffer_size - 1);
            it->second.buffer[it->second.buffer_size - 1] = '\0';
        }
    } else {
        spdlog::warn("[MockLVGL] lv_subject_copy_string() - subject not found");
    }
}

void lv_obj_add_event_cb(lv_obj_t* obj, lv_event_cb_t event_cb, lv_event_code_t filter, void* user_data) {
    auto it = LVGLMock::widgets.find(obj);
    if (it != LVGLMock::widgets.end()) {
        spdlog::debug("[MockLVGL] lv_obj_add_event_cb('{}', code={})",
                     it->second.name, (int)filter);
        it->second.event_callbacks.push_back(event_cb);
        it->second.user_data = user_data;
    } else {
        spdlog::warn("[MockLVGL] lv_obj_add_event_cb() - widget not found");
    }
}

void lv_event_send(lv_obj_t* obj, lv_event_code_t code, void* param) {
    auto it = LVGLMock::widgets.find(obj);
    if (it != LVGLMock::widgets.end()) {
        spdlog::debug("[MockLVGL] lv_event_send('{}', code={})",
                     it->second.name, (int)code);

        lv_event_t e;
        e.target = obj;
        e.code = code;
        e.user_data = it->second.user_data;
        e.param = param;

        for (auto& cb : it->second.event_callbacks) {
            cb(&e);
        }
    } else {
        spdlog::warn("[MockLVGL] lv_event_send() - widget not found");
    }
}

void lv_xml_register_event_cb(void* ctx, const char* name, lv_event_cb_t cb) {
    (void)ctx;  // XML context not used
    (void)cb;   // Callback stored via lv_obj_add_event_cb instead
    spdlog::debug("[MockLVGL] lv_xml_register_event_cb('{}')", name);
    // No-op for now - events are registered via lv_obj_add_event_cb
}

uint32_t lv_tick_get() {
    return LVGLMock::mock_tick;
}

lv_timer_t* lv_timer_create(lv_timer_cb_t timer_cb, uint32_t period, void* user_data) {
    spdlog::debug("[MockLVGL] lv_timer_create(period={}ms)", period);

    lv_timer_t* timer = reinterpret_cast<lv_timer_t*>(LVGLMock::timer_counter++);

    LVGLMock::MockTimer timer_data;
    timer_data.callback = timer_cb;
    timer_data.period = period;
    timer_data.user_data = user_data;

    LVGLMock::timers[timer] = timer_data;

    return timer;
}

void lv_timer_del(lv_timer_t* timer) {
    spdlog::debug("[MockLVGL] lv_timer_del()");
    LVGLMock::timers.erase(timer);
}

} // extern "C"
