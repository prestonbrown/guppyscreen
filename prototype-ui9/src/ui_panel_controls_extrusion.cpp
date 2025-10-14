/*
 * Copyright (C) 2025 356C LLC
 * Author: Preston Brown <pbrown@brown-house.net>
 *
 * This file is part of GuppyScreen.
 *
 * GuppyScreen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GuppyScreen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GuppyScreen. If not, see <https://www.gnu.org/licenses/>.
 */

#include "ui_panel_controls_extrusion.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>  // for abs()

// Temperature subjects (reactive data binding)
static lv_subject_t temp_status_subject;
static lv_subject_t warning_temps_subject;

// Subject storage buffers
static char temp_status_buf[64];
static char warning_temps_buf[64];

// Current state
static int nozzle_current = 25;
static int nozzle_target = 0;
static int selected_amount = 10;  // Default: 10mm
static const int MIN_EXTRUSION_TEMP = 170;

// Panel widgets
static lv_obj_t* extrusion_panel = nullptr;
static lv_obj_t* parent_obj = nullptr;
static lv_obj_t* btn_extrude = nullptr;
static lv_obj_t* btn_retract = nullptr;
static lv_obj_t* safety_warning = nullptr;

// Amount button widgets (for visual feedback)
static lv_obj_t* amount_buttons[4] = {nullptr, nullptr, nullptr, nullptr};
static const int amount_values[4] = {5, 10, 25, 50};

// Forward declarations
static void update_temp_status();
static void update_warning_text();
static void update_safety_state();
static void update_amount_buttons_visual();

void ui_panel_controls_extrusion_init_subjects() {
    // Initialize subjects with default values
    snprintf(temp_status_buf, sizeof(temp_status_buf), "%d / %d°C", nozzle_current, nozzle_target);
    snprintf(warning_temps_buf, sizeof(warning_temps_buf), "Current: %d°C\nTarget: %d°C", nozzle_current, nozzle_target);

    lv_subject_init_string(&temp_status_subject, temp_status_buf, nullptr, sizeof(temp_status_buf), temp_status_buf);
    lv_subject_init_string(&warning_temps_subject, warning_temps_buf, nullptr, sizeof(warning_temps_buf), warning_temps_buf);

    // Register subjects with XML system (global scope)
    lv_xml_register_subject(NULL, "extrusion_temp_status", &temp_status_subject);
    lv_xml_register_subject(NULL, "extrusion_warning_temps", &warning_temps_subject);

    printf("[Extrusion] Subjects initialized: temp=%d/%d°C, amount=%dmm\n",
           nozzle_current, nozzle_target, selected_amount);
}

// Update temperature status display text
static void update_temp_status() {
    // Status indicator: ✓ (ready), ⚠ (heating), ✗ (too cold)
    const char* status_icon;
    if (nozzle_current >= MIN_EXTRUSION_TEMP) {
        // Within 5°C of target and hot enough
        if (nozzle_target > 0 && abs(nozzle_current - nozzle_target) <= 5) {
            status_icon = "✓";  // Ready
        } else {
            status_icon = "✓";  // Hot enough
        }
    } else if (nozzle_target >= MIN_EXTRUSION_TEMP) {
        status_icon = "⚠";  // Heating
    } else {
        status_icon = "✗";  // Too cold
    }

    snprintf(temp_status_buf, sizeof(temp_status_buf), "%d / %d°C %s",
             nozzle_current, nozzle_target, status_icon);
    lv_subject_copy_string(&temp_status_subject, temp_status_buf);
}

// Update warning card text
static void update_warning_text() {
    snprintf(warning_temps_buf, sizeof(warning_temps_buf), "Current: %d°C\nTarget: %d°C",
             nozzle_current, nozzle_target);
    lv_subject_copy_string(&warning_temps_subject, warning_temps_buf);
}

// Update safety state (button enable/disable, warning visibility)
static void update_safety_state() {
    bool allowed = (nozzle_current >= MIN_EXTRUSION_TEMP);

    // Enable/disable extrude and retract buttons
    if (btn_extrude) {
        if (allowed) {
            lv_obj_clear_state(btn_extrude, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(btn_extrude, lv_color_hex(0x4caf50), 0);  // Green
        } else {
            lv_obj_add_state(btn_extrude, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(btn_extrude, lv_color_hex(0x3a3a3a), 0);  // Gray
        }
    }

    if (btn_retract) {
        if (allowed) {
            lv_obj_clear_state(btn_retract, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(btn_retract, lv_color_hex(0xff9800), 0);  // Orange
        } else {
            lv_obj_add_state(btn_retract, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(btn_retract, lv_color_hex(0x3a3a3a), 0);  // Gray
        }
    }

    // Show/hide safety warning
    if (safety_warning) {
        if (allowed) {
            lv_obj_add_flag(safety_warning, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_clear_flag(safety_warning, LV_OBJ_FLAG_HIDDEN);
        }
    }

    printf("[Extrusion] Safety state updated: allowed=%d (temp=%d°C)\n", allowed, nozzle_current);
}

// Update visual feedback for amount selector buttons
static void update_amount_buttons_visual() {
    for (int i = 0; i < 4; i++) {
        if (amount_buttons[i]) {
            if (amount_values[i] == selected_amount) {
                // Selected: red background, white text
                lv_obj_set_style_bg_color(amount_buttons[i], lv_color_hex(0xff4444), 0);
                lv_obj_t* label = lv_obj_get_child(amount_buttons[i], 0);
                if (label) {
                    lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), 0);
                }
            } else {
                // Unselected: dark gray background, light gray text
                lv_obj_set_style_bg_color(amount_buttons[i], lv_color_hex(0x3a3a3a), 0);
                lv_obj_t* label = lv_obj_get_child(amount_buttons[i], 0);
                if (label) {
                    lv_obj_set_style_text_color(label, lv_color_hex(0xb0b0b0), 0);
                }
            }
        }
    }
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================

// Event handler: Back button
static void back_button_cb(lv_event_t* e) {
    (void)e;

    // Hide extrusion panel
    if (extrusion_panel) {
        lv_obj_add_flag(extrusion_panel, LV_OBJ_FLAG_HIDDEN);
    }

    // Show controls panel launcher
    if (parent_obj) {
        lv_obj_t* controls_launcher = lv_obj_find_by_name(parent_obj, "controls_panel");
        if (controls_launcher) {
            lv_obj_clear_flag(controls_launcher, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// Event handler: Amount selector buttons
static void amount_button_cb(lv_event_t* e) {
    lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
    const char* name = lv_obj_get_name(btn);

    if (!name) return;

    if (strcmp(name, "amount_5mm") == 0) {
        selected_amount = 5;
    } else if (strcmp(name, "amount_10mm") == 0) {
        selected_amount = 10;
    } else if (strcmp(name, "amount_25mm") == 0) {
        selected_amount = 25;
    } else if (strcmp(name, "amount_50mm") == 0) {
        selected_amount = 50;
    }

    update_amount_buttons_visual();
    printf("[Extrusion] Amount selected: %dmm\n", selected_amount);
}

// Event handler: Extrude button
static void extrude_button_cb(lv_event_t* e) {
    (void)e;

    if (nozzle_current < MIN_EXTRUSION_TEMP) {
        printf("[Extrusion] ✗ Extrude blocked: nozzle too cold (%d°C < %d°C)\n",
               nozzle_current, MIN_EXTRUSION_TEMP);
        return;
    }

    printf("[Extrusion] ✓ Extruding %dmm of filament...\n", selected_amount);
    // TODO: Send command to printer (moonraker_extrude(selected_amount))
}

// Event handler: Retract button
static void retract_button_cb(lv_event_t* e) {
    (void)e;

    if (nozzle_current < MIN_EXTRUSION_TEMP) {
        printf("[Extrusion] ✗ Retract blocked: nozzle too cold (%d°C < %d°C)\n",
               nozzle_current, MIN_EXTRUSION_TEMP);
        return;
    }

    printf("[Extrusion] ✓ Retracting %dmm of filament...\n", selected_amount);
    // TODO: Send command to printer (moonraker_retract(selected_amount))
}

// ============================================================================
// PUBLIC API
// ============================================================================

void ui_panel_controls_extrusion_setup(lv_obj_t* panel, lv_obj_t* parent_screen) {
    extrusion_panel = panel;
    parent_obj = parent_screen;

    printf("[Extrusion] Setting up panel event handlers...\n");

    // Back button
    lv_obj_t* back_btn = lv_obj_find_by_name(panel, "back_button");
    if (back_btn) {
        lv_obj_add_event_cb(back_btn, back_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[Extrusion]   ✓ Back button\n");
    }

    // Amount selector buttons
    const char* amount_names[] = {"amount_5mm", "amount_10mm", "amount_25mm", "amount_50mm"};
    for (int i = 0; i < 4; i++) {
        amount_buttons[i] = lv_obj_find_by_name(panel, amount_names[i]);
        if (amount_buttons[i]) {
            lv_obj_add_event_cb(amount_buttons[i], amount_button_cb, LV_EVENT_CLICKED, nullptr);
        }
    }
    printf("[Extrusion]   ✓ Amount buttons (4)\n");

    // Extrude button
    btn_extrude = lv_obj_find_by_name(panel, "btn_extrude");
    if (btn_extrude) {
        lv_obj_add_event_cb(btn_extrude, extrude_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[Extrusion]   ✓ Extrude button\n");
    }

    // Retract button
    btn_retract = lv_obj_find_by_name(panel, "btn_retract");
    if (btn_retract) {
        lv_obj_add_event_cb(btn_retract, retract_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[Extrusion]   ✓ Retract button\n");
    }

    // Safety warning card
    safety_warning = lv_obj_find_by_name(panel, "safety_warning");

    // Initialize visual state
    update_amount_buttons_visual();
    update_temp_status();
    update_warning_text();
    update_safety_state();

    printf("[Extrusion] Panel setup complete!\n");
}

void ui_panel_controls_extrusion_set_temp(int current, int target) {
    nozzle_current = current;
    nozzle_target = target;
    update_temp_status();
    update_warning_text();
    update_safety_state();
}

int ui_panel_controls_extrusion_get_amount() {
    return selected_amount;
}

bool ui_panel_controls_extrusion_is_allowed() {
    return (nozzle_current >= MIN_EXTRUSION_TEMP);
}
