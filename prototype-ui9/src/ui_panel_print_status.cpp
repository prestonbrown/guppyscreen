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

#include "ui_panel_print_status.h"
#include "ui_component_header_bar.h"
#include "ui_utils.h"
#include "ui_nav.h"
#include <stdio.h>
#include <string.h>

// Subjects for reactive data binding
static lv_subject_t filename_subject;
static lv_subject_t progress_text_subject;
static lv_subject_t layer_text_subject;
static lv_subject_t elapsed_subject;
static lv_subject_t remaining_subject;
static lv_subject_t nozzle_temp_subject;
static lv_subject_t bed_temp_subject;
static lv_subject_t speed_subject;
static lv_subject_t flow_subject;
static lv_subject_t pause_button_subject;

// Subject storage buffers
static char filename_buf[128];
static char progress_text_buf[32];
static char layer_text_buf[64];
static char elapsed_buf[32];
static char remaining_buf[32];
static char nozzle_temp_buf[32];
static char bed_temp_buf[32];
static char speed_buf[32];
static char flow_buf[32];
static char pause_button_buf[32];

// Current state
static print_state_t current_state = PRINT_STATE_IDLE;
static int current_progress = 0;
static int current_layer = 0;
static int total_layers = 0;
static int elapsed_seconds = 0;
static int remaining_seconds = 0;
static int nozzle_current = 0;
static int nozzle_target = 0;
static int bed_current = 0;
static int bed_target = 0;
static int speed_percent = 100;
static int flow_percent = 100;

// Mock simulation state
static bool mock_active = false;
static int mock_total_seconds = 0;
static int mock_elapsed_seconds = 0;
static int mock_total_layers = 0;

// Panel widgets
static lv_obj_t* print_status_panel = nullptr;
static lv_obj_t* parent_obj = nullptr;
static lv_obj_t* progress_bar = nullptr;

// Forward declarations
static void update_all_displays();
static void format_time(int seconds, char* buf, size_t buf_size);

void ui_panel_print_status_init_subjects() {
    // Initialize subjects with default values
    snprintf(filename_buf, sizeof(filename_buf), "No print active");
    snprintf(progress_text_buf, sizeof(progress_text_buf), "0%%");
    snprintf(layer_text_buf, sizeof(layer_text_buf), "Layer 0 / 0");
    snprintf(elapsed_buf, sizeof(elapsed_buf), "0h 00m");
    snprintf(remaining_buf, sizeof(remaining_buf), "0h 00m");
    snprintf(nozzle_temp_buf, sizeof(nozzle_temp_buf), "0 / 0°C");
    snprintf(bed_temp_buf, sizeof(bed_temp_buf), "0 / 0°C");
    snprintf(speed_buf, sizeof(speed_buf), "100%%");
    snprintf(flow_buf, sizeof(flow_buf), "100%%");
    snprintf(pause_button_buf, sizeof(pause_button_buf), "Pause");

    lv_subject_init_string(&filename_subject, filename_buf, nullptr, sizeof(filename_buf), filename_buf);
    lv_subject_init_string(&progress_text_subject, progress_text_buf, nullptr, sizeof(progress_text_buf), progress_text_buf);
    lv_subject_init_string(&layer_text_subject, layer_text_buf, nullptr, sizeof(layer_text_buf), layer_text_buf);
    lv_subject_init_string(&elapsed_subject, elapsed_buf, nullptr, sizeof(elapsed_buf), elapsed_buf);
    lv_subject_init_string(&remaining_subject, remaining_buf, nullptr, sizeof(remaining_buf), remaining_buf);
    lv_subject_init_string(&nozzle_temp_subject, nozzle_temp_buf, nullptr, sizeof(nozzle_temp_buf), nozzle_temp_buf);
    lv_subject_init_string(&bed_temp_subject, bed_temp_buf, nullptr, sizeof(bed_temp_buf), bed_temp_buf);
    lv_subject_init_string(&speed_subject, speed_buf, nullptr, sizeof(speed_buf), speed_buf);
    lv_subject_init_string(&flow_subject, flow_buf, nullptr, sizeof(flow_buf), flow_buf);
    lv_subject_init_string(&pause_button_subject, pause_button_buf, nullptr, sizeof(pause_button_buf), pause_button_buf);

    // Register subjects with XML system (global scope)
    lv_xml_register_subject(NULL, "print_filename", &filename_subject);
    lv_xml_register_subject(NULL, "print_progress_text", &progress_text_subject);
    lv_xml_register_subject(NULL, "print_layer_text", &layer_text_subject);
    lv_xml_register_subject(NULL, "print_elapsed", &elapsed_subject);
    lv_xml_register_subject(NULL, "print_remaining", &remaining_subject);
    lv_xml_register_subject(NULL, "nozzle_temp_text", &nozzle_temp_subject);
    lv_xml_register_subject(NULL, "bed_temp_text", &bed_temp_subject);
    lv_xml_register_subject(NULL, "print_speed_text", &speed_subject);
    lv_xml_register_subject(NULL, "print_flow_text", &flow_subject);
    lv_xml_register_subject(NULL, "pause_button_text", &pause_button_subject);

    printf("[PrintStatus] Subjects initialized\n");
}

// Format seconds to "Xh YYm" format
static void format_time(int seconds, char* buf, size_t buf_size) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    snprintf(buf, buf_size, "%dh %02dm", hours, minutes);
}

// Update all display elements
static void update_all_displays() {
    // Progress text
    snprintf(progress_text_buf, sizeof(progress_text_buf), "%d%%", current_progress);
    lv_subject_copy_string(&progress_text_subject, progress_text_buf);

    // Layer text
    snprintf(layer_text_buf, sizeof(layer_text_buf), "Layer %d / %d", current_layer, total_layers);
    lv_subject_copy_string(&layer_text_subject, layer_text_buf);

    // Time displays
    format_time(elapsed_seconds, elapsed_buf, sizeof(elapsed_buf));
    lv_subject_copy_string(&elapsed_subject, elapsed_buf);

    format_time(remaining_seconds, remaining_buf, sizeof(remaining_buf));
    lv_subject_copy_string(&remaining_subject, remaining_buf);

    // Temperatures
    snprintf(nozzle_temp_buf, sizeof(nozzle_temp_buf), "%d / %d°C", nozzle_current, nozzle_target);
    lv_subject_copy_string(&nozzle_temp_subject, nozzle_temp_buf);

    snprintf(bed_temp_buf, sizeof(bed_temp_buf), "%d / %d°C", bed_current, bed_target);
    lv_subject_copy_string(&bed_temp_subject, bed_temp_buf);

    // Speeds
    snprintf(speed_buf, sizeof(speed_buf), "%d%%", speed_percent);
    lv_subject_copy_string(&speed_subject, speed_buf);

    snprintf(flow_buf, sizeof(flow_buf), "%d%%", flow_percent);
    lv_subject_copy_string(&flow_subject, flow_buf);

    // Update progress bar widget directly
    if (progress_bar) {
        lv_bar_set_value(progress_bar, current_progress, LV_ANIM_OFF);
    }

    // Update pause button text based on state
    if (current_state == PRINT_STATE_PAUSED) {
        snprintf(pause_button_buf, sizeof(pause_button_buf), "Resume");
    } else {
        snprintf(pause_button_buf, sizeof(pause_button_buf), "Pause");
    }
    lv_subject_copy_string(&pause_button_subject, pause_button_buf);
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================

// Event handler: Back button (from header_bar component)
static void back_button_cb(lv_event_t* e) {
    (void)e;
    printf("[PrintStatus] Back button clicked\n");

    // Use navigation history to go back to previous panel
    if (!ui_nav_go_back()) {
        // Fallback: If navigation history is empty, manually hide and show home panel
        if (print_status_panel) {
            lv_obj_add_flag(print_status_panel, LV_OBJ_FLAG_HIDDEN);
        }

        if (parent_obj) {
            lv_obj_t* home_panel = lv_obj_find_by_name(parent_obj, "home_panel");
            if (home_panel) {
                lv_obj_clear_flag(home_panel, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
}

// Event handler: Nozzle temperature card
static void nozzle_temp_card_cb(lv_event_t* e) {
    (void)e;
    printf("[PrintStatus] Nozzle temp card clicked\n");
    // TODO: Show nozzle temperature adjustment panel
}

// Event handler: Bed temperature card
static void bed_temp_card_cb(lv_event_t* e) {
    (void)e;
    printf("[PrintStatus] Bed temp card clicked\n");
    // TODO: Show bed temperature adjustment panel
}

// Event handler: Light toggle button
static void light_button_cb(lv_event_t* e) {
    (void)e;
    printf("[PrintStatus] Light button clicked\n");
    // TODO: Toggle printer LED/light on/off
}

// Event handler: Pause/Resume button
static void pause_button_cb(lv_event_t* e) {
    (void)e;

    if (current_state == PRINT_STATE_PRINTING) {
        printf("[PrintStatus] Pausing print...\n");
        ui_panel_print_status_set_state(PRINT_STATE_PAUSED);
        // TODO: Send pause command to printer
    } else if (current_state == PRINT_STATE_PAUSED) {
        printf("[PrintStatus] Resuming print...\n");
        ui_panel_print_status_set_state(PRINT_STATE_PRINTING);
        // TODO: Send resume command to printer
    }
}

// Event handler: Tune button
static void tune_button_cb(lv_event_t* e) {
    (void)e;
    printf("[PrintStatus] Tune button clicked (not yet implemented)\n");
    // TODO: Open tuning overlay with speed/flow/temp adjustments
}

// Event handler: Cancel button
static void cancel_button_cb(lv_event_t* e) {
    (void)e;
    printf("[PrintStatus] Cancel button clicked\n");
    // TODO: Show confirmation dialog, then cancel print
    ui_panel_print_status_set_state(PRINT_STATE_CANCELLED);
    ui_panel_print_status_stop_mock_print();
}

// ============================================================================
// Image scaling helper
// ============================================================================

static void scale_thumbnail_images() {
    if (!print_status_panel) return;

    // Find thumbnail section to get target dimensions
    lv_obj_t* thumbnail_section = lv_obj_find_by_name(print_status_panel, "thumbnail_section");
    if (!thumbnail_section) {
        LV_LOG_WARN("Thumbnail section not found, cannot scale images");
        return;
    }

    lv_coord_t section_width = lv_obj_get_width(thumbnail_section);
    lv_coord_t section_height = lv_obj_get_height(thumbnail_section);

    // Scale gradient background to cover the entire section
    lv_obj_t* gradient_bg = lv_obj_find_by_name(print_status_panel, "gradient_background");
    if (gradient_bg) {
        ui_image_scale_to_cover(gradient_bg, section_width, section_height);
    }

    // Scale thumbnail to contain within section (no cropping)
    lv_obj_t* thumbnail = lv_obj_find_by_name(print_status_panel, "print_thumbnail");
    if (thumbnail) {
        ui_image_scale_to_contain(thumbnail, section_width, section_height, LV_IMAGE_ALIGN_TOP_MID);
    }
}

// ============================================================================
// Resize callback for thumbnail scaling
// ============================================================================

static void on_resize() {
    LV_LOG_USER("Print status panel handling resize event");

    // Update content padding
    if (print_status_panel && parent_obj) {
        lv_obj_t* content_container = lv_obj_find_by_name(print_status_panel, "content_container");
        if (content_container) {
            lv_coord_t padding = ui_get_header_content_padding(lv_obj_get_height(parent_obj));
            lv_obj_set_style_pad_all(content_container, padding, 0);
        }
    }

    scale_thumbnail_images();
}

// ============================================================================
// PUBLIC API
// ============================================================================

void ui_panel_print_status_setup(lv_obj_t* panel, lv_obj_t* parent_screen) {
    print_status_panel = panel;
    parent_obj = parent_screen;

    // Calculate width to fill remaining space after navigation bar (screen-size agnostic)
    lv_coord_t screen_width = lv_obj_get_width(parent_screen);
    lv_coord_t nav_width = screen_width / 10;  // UI_NAV_WIDTH macro: screen_width / 10
    lv_obj_set_width(panel, screen_width - nav_width);

    // Setup header for responsive height
    lv_obj_t* print_status_header = lv_obj_find_by_name(panel, "print_status_header");
    if (print_status_header) {
        ui_component_header_bar_setup(print_status_header, parent_screen);
    }

    // Set responsive padding for content area
    lv_obj_t* content_container = lv_obj_find_by_name(panel, "content_container");
    if (content_container) {
        lv_coord_t padding = ui_get_header_content_padding(lv_obj_get_height(parent_screen));
        lv_obj_set_style_pad_all(content_container, padding, 0);
        printf("[PrintStatus]   ✓ Content padding: %dpx (responsive)\n", padding);
    }

    // Force layout calculation before scaling images (flex layout needs this)
    lv_obj_update_layout(panel);

    // Perform initial image scaling (CRITICAL: must be done after layout calculation)
    scale_thumbnail_images();

    // Register resize callback for responsive thumbnail scaling
    ui_resize_handler_register(on_resize);

    printf("[PrintStatus] Setting up panel event handlers...\n");

    // Back button (from header_bar component)
    lv_obj_t* back_btn = lv_obj_find_by_name(panel, "back_button");
    if (back_btn) {
        lv_obj_add_event_cb(back_btn, back_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Back button\n");
    } else {
        printf("[PrintStatus]   ✗ Back button NOT FOUND\n");
    }

    // Nozzle temperature card (clickable)
    lv_obj_t* nozzle_card = lv_obj_find_by_name(panel, "nozzle_temp_card");
    if (nozzle_card) {
        lv_obj_add_event_cb(nozzle_card, nozzle_temp_card_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Nozzle temp card\n");
    } else {
        printf("[PrintStatus]   ✗ Nozzle temp card NOT FOUND\n");
    }

    // Bed temperature card (clickable)
    lv_obj_t* bed_card = lv_obj_find_by_name(panel, "bed_temp_card");
    if (bed_card) {
        lv_obj_add_event_cb(bed_card, bed_temp_card_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Bed temp card\n");
    } else {
        printf("[PrintStatus]   ✗ Bed temp card NOT FOUND\n");
    }

    // Light button
    lv_obj_t* light_btn = lv_obj_find_by_name(panel, "btn_light");
    if (light_btn) {
        lv_obj_add_event_cb(light_btn, light_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Light button\n");
    } else {
        printf("[PrintStatus]   ✗ Light button NOT FOUND\n");
    }

    // Pause button
    lv_obj_t* pause_btn = lv_obj_find_by_name(panel, "btn_pause");
    if (pause_btn) {
        lv_obj_add_event_cb(pause_btn, pause_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Pause button\n");
    } else {
        printf("[PrintStatus]   ✗ Pause button NOT FOUND\n");
    }

    // Tune button
    lv_obj_t* tune_btn = lv_obj_find_by_name(panel, "btn_tune");
    if (tune_btn) {
        lv_obj_add_event_cb(tune_btn, tune_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Tune button\n");
    } else {
        printf("[PrintStatus]   ✗ Tune button NOT FOUND\n");
    }

    // Cancel button
    lv_obj_t* cancel_btn = lv_obj_find_by_name(panel, "btn_cancel");
    if (cancel_btn) {
        lv_obj_add_event_cb(cancel_btn, cancel_button_cb, LV_EVENT_CLICKED, nullptr);
        printf("[PrintStatus]   ✓ Cancel button\n");
    } else {
        printf("[PrintStatus]   ✗ Cancel button NOT FOUND\n");
    }

    // Get progress bar widget for direct updates
    progress_bar = lv_obj_find_by_name(panel, "progress_bar");
    if (progress_bar) {
        lv_bar_set_range(progress_bar, 0, 100);
        lv_bar_set_value(progress_bar, 0, LV_ANIM_OFF);
        printf("[PrintStatus]   ✓ Progress bar\n");
    } else {
        printf("[PrintStatus]   ✗ Progress bar NOT FOUND\n");
    }

    printf("[PrintStatus] Panel setup complete!\n");
}

void ui_panel_print_status_set_filename(const char* filename) {
    snprintf(filename_buf, sizeof(filename_buf), "%s", filename);
    lv_subject_copy_string(&filename_subject, filename_buf);
}

void ui_panel_print_status_set_progress(int percent) {
    current_progress = percent;
    if (current_progress < 0) current_progress = 0;
    if (current_progress > 100) current_progress = 100;
    update_all_displays();
}

void ui_panel_print_status_set_layer(int current, int total) {
    current_layer = current;
    total_layers = total;
    update_all_displays();
}

void ui_panel_print_status_set_times(int elapsed_secs, int remaining_secs) {
    elapsed_seconds = elapsed_secs;
    remaining_seconds = remaining_secs;
    update_all_displays();
}

void ui_panel_print_status_set_temperatures(int nz_cur, int nz_tgt, int bd_cur, int bd_tgt) {
    nozzle_current = nz_cur;
    nozzle_target = nz_tgt;
    bed_current = bd_cur;
    bed_target = bd_tgt;
    update_all_displays();
}

void ui_panel_print_status_set_speeds(int speed_pct, int flow_pct) {
    speed_percent = speed_pct;
    flow_percent = flow_pct;
    update_all_displays();
}

void ui_panel_print_status_set_state(print_state_t state) {
    current_state = state;
    update_all_displays();
    printf("[PrintStatus] State changed to: %d\n", state);
}

// ============================================================================
// MOCK PRINT SIMULATION
// ============================================================================

void ui_panel_print_status_start_mock_print(const char* filename, int layers, int duration_secs) {
    mock_active = true;
    mock_total_seconds = duration_secs;
    mock_elapsed_seconds = 0;
    mock_total_layers = layers;

    ui_panel_print_status_set_filename(filename);
    ui_panel_print_status_set_progress(0);
    ui_panel_print_status_set_layer(0, layers);
    ui_panel_print_status_set_times(0, duration_secs);
    ui_panel_print_status_set_temperatures(215, 215, 60, 60);
    ui_panel_print_status_set_speeds(100, 100);
    ui_panel_print_status_set_state(PRINT_STATE_PRINTING);

    printf("[PrintStatus] Mock print started: %s (%d layers, %d seconds)\n",
           filename, layers, duration_secs);
}

void ui_panel_print_status_stop_mock_print() {
    mock_active = false;
    printf("[PrintStatus] Mock print stopped\n");
}

void ui_panel_print_status_tick_mock_print() {
    if (!mock_active) return;
    if (current_state != PRINT_STATE_PRINTING) return;
    if (mock_elapsed_seconds >= mock_total_seconds) {
        // Print complete
        ui_panel_print_status_set_state(PRINT_STATE_COMPLETE);
        ui_panel_print_status_stop_mock_print();
        printf("[PrintStatus] Mock print complete!\n");
        return;
    }

    // Advance simulation by 1 second
    mock_elapsed_seconds++;

    // Calculate progress
    int progress = (mock_elapsed_seconds * 100) / mock_total_seconds;
    int remaining = mock_total_seconds - mock_elapsed_seconds;
    int layer = (mock_elapsed_seconds * mock_total_layers) / mock_total_seconds;

    // Update displays
    ui_panel_print_status_set_progress(progress);
    ui_panel_print_status_set_layer(layer, mock_total_layers);
    ui_panel_print_status_set_times(mock_elapsed_seconds, remaining);

    // Simulate temperature fluctuations (±2°C)
    int nozzle_var = (mock_elapsed_seconds % 4) - 2;
    int bed_var = (mock_elapsed_seconds % 6) - 3;
    ui_panel_print_status_set_temperatures(215 + nozzle_var, 215, 60 + bed_var, 60);
}

print_state_t ui_panel_print_status_get_state() {
    return current_state;
}

int ui_panel_print_status_get_progress() {
    return current_progress;
}
