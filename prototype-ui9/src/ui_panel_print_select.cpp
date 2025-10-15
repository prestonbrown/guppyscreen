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

#include "ui_panel_print_select.h"
#include "ui_utils.h"
#include "ui_fonts.h"
#include "lvgl/src/others/xml/lv_xml.h"
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

// Default placeholder thumbnail for print files
static const char* DEFAULT_PLACEHOLDER_THUMB = "A:assets/images/placeholder_thumb_centered.png";

// ============================================================================
// File data structure
// ============================================================================
struct PrintFileData {
    std::string filename;
    std::string thumbnail_path;
    size_t file_size_bytes;      // File size in bytes
    time_t modified_timestamp;   // Last modified timestamp
    int print_time_minutes;      // Print time in minutes
    float filament_grams;        // Filament weight in grams

    // Formatted strings (cached for performance)
    std::string size_str;
    std::string modified_str;
    std::string print_time_str;
    std::string filament_str;
};

// ============================================================================
// Screen size responsive card component selection
// ============================================================================
struct CardLayout {
    const char* component_name;
    int gap;
};

static CardLayout get_card_layout() {
    lv_coord_t screen_width = lv_display_get_horizontal_resolution(lv_display_get_default());

    CardLayout layout;

    if (screen_width >= 1024) {
        // 1280 & 1024: 5 columns, large thumbnails (defined in print_file_card_5col.xml)
        layout.component_name = "print_file_card_5col";
        layout.gap = 20;
    } else if (screen_width >= 700) {
        // 800: 4 columns, medium thumbnails (defined in print_file_card_4col.xml)
        layout.component_name = "print_file_card_4col";
        layout.gap = 20;
    } else {
        // 480: 3 columns, small thumbnails (defined in print_file_card_3col.xml)
        layout.component_name = "print_file_card_3col";
        layout.gap = 12;
    }

    return layout;
}

// ============================================================================
// Static state
// ============================================================================
static std::vector<PrintFileData> file_list;
static PrintSelectViewMode current_view_mode = PrintSelectViewMode::CARD;
static PrintSelectSortColumn current_sort_column = PrintSelectSortColumn::FILENAME;
static PrintSelectSortDirection current_sort_direction = PrintSelectSortDirection::ASCENDING;

// Widget references
static lv_obj_t* panel_root_widget = nullptr;
static lv_obj_t* parent_screen_widget = nullptr;
static lv_obj_t* card_view_container = nullptr;
static lv_obj_t* list_view_container = nullptr;
static lv_obj_t* list_rows_container = nullptr;
static lv_obj_t* empty_state_container = nullptr;
static lv_obj_t* view_toggle_btn = nullptr;
static lv_obj_t* view_toggle_icon = nullptr;
static lv_obj_t* detail_view_widget = nullptr;
static lv_obj_t* confirmation_dialog_widget = nullptr;

// ============================================================================
// Reactive subjects for selected file state
// ============================================================================
static lv_subject_t selected_filename_subject;
static char selected_filename_buffer[128];

static lv_subject_t selected_thumbnail_subject;
static char selected_thumbnail_buffer[256];

static lv_subject_t selected_print_time_subject;
static char selected_print_time_buffer[32];

static lv_subject_t selected_filament_weight_subject;
static char selected_filament_weight_buffer[32];

static lv_subject_t detail_view_visible_subject;

// Forward declarations
static void populate_card_view();
static void populate_list_view();
static void apply_sort();
static void update_empty_state();
static void update_sort_indicators();
static void attach_card_click_handler(lv_obj_t* card, const PrintFileData& file_data);
static void attach_row_click_handler(lv_obj_t* row, const PrintFileData& file_data);
static void create_detail_view();
static void create_confirmation_dialog();

// ============================================================================
// Subject initialization
// ============================================================================
void ui_panel_print_select_init_subjects() {
    // Initialize selected file subjects
    lv_subject_init_string(&selected_filename_subject, selected_filename_buffer,
                          nullptr, sizeof(selected_filename_buffer), "");
    lv_xml_register_subject(nullptr, "selected_filename", &selected_filename_subject);

    lv_subject_init_string(&selected_thumbnail_subject, selected_thumbnail_buffer,
                          nullptr, sizeof(selected_thumbnail_buffer), DEFAULT_PLACEHOLDER_THUMB);
    lv_xml_register_subject(nullptr, "selected_thumbnail", &selected_thumbnail_subject);

    lv_subject_init_string(&selected_print_time_subject, selected_print_time_buffer,
                          nullptr, sizeof(selected_print_time_subject), "");
    lv_xml_register_subject(nullptr, "selected_print_time", &selected_print_time_subject);

    lv_subject_init_string(&selected_filament_weight_subject, selected_filament_weight_buffer,
                          nullptr, sizeof(selected_filament_weight_buffer), "");
    lv_xml_register_subject(nullptr, "selected_filament_weight", &selected_filament_weight_subject);

    // Initialize detail view visibility subject (0 = hidden, 1 = visible)
    lv_subject_init_int(&detail_view_visible_subject, 0);
    lv_xml_register_subject(nullptr, "detail_view_visible", &detail_view_visible_subject);

    LV_LOG_USER("Print select panel subjects initialized");
}

// ============================================================================
// Panel setup (call after XML creation)
// ============================================================================
void ui_panel_print_select_setup(lv_obj_t* panel_root, lv_obj_t* parent_screen) {
    if (!panel_root) {
        LV_LOG_ERROR("Cannot setup print select panel: panel_root is null");
        return;
    }

    panel_root_widget = panel_root;
    parent_screen_widget = parent_screen;

    // Find widget references
    card_view_container = lv_obj_find_by_name(panel_root, "card_view_container");
    list_view_container = lv_obj_find_by_name(panel_root, "list_view_container");
    list_rows_container = lv_obj_find_by_name(panel_root, "list_rows_container");
    empty_state_container = lv_obj_find_by_name(panel_root, "empty_state_container");
    view_toggle_btn = lv_obj_find_by_name(panel_root, "view_toggle_btn");
    view_toggle_icon = lv_obj_find_by_name(panel_root, "view_toggle_icon");

    if (!card_view_container || !list_view_container || !list_rows_container ||
        !empty_state_container || !view_toggle_btn || !view_toggle_icon) {
        LV_LOG_ERROR("Failed to find required widgets in print select panel");
        return;
    }

    // Wire up view toggle button
    lv_obj_add_event_cb(view_toggle_btn, [](lv_event_t* e) {
        (void)e;
        ui_panel_print_select_toggle_view();
    }, LV_EVENT_CLICKED, nullptr);

    // Wire up column header click handlers
    const char* header_names[] = {"header_filename", "header_size", "header_modified",
                                  "header_print_time"};
    PrintSelectSortColumn columns[] = {PrintSelectSortColumn::FILENAME,
                                       PrintSelectSortColumn::SIZE,
                                       PrintSelectSortColumn::MODIFIED,
                                       PrintSelectSortColumn::PRINT_TIME};

    for (int i = 0; i < 4; i++) {
        lv_obj_t* header = lv_obj_find_by_name(panel_root, header_names[i]);
        if (header) {
            lv_obj_add_event_cb(header, [](lv_event_t* e) {
                PrintSelectSortColumn* col = (PrintSelectSortColumn*)lv_event_get_user_data(e);
                ui_panel_print_select_sort_by(*col);
            }, LV_EVENT_CLICKED, &columns[i]);
        }
    }

    // Create detail view and confirmation dialog
    create_detail_view();
    create_confirmation_dialog();

    LV_LOG_USER("Print select panel setup complete");
}

// ============================================================================
// View toggle
// ============================================================================
void ui_panel_print_select_toggle_view() {
    if (current_view_mode == PrintSelectViewMode::CARD) {
        // Switch to list view
        current_view_mode = PrintSelectViewMode::LIST;
        lv_obj_add_flag(card_view_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(list_view_container, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(view_toggle_icon, ICON_TH_LARGE);  // Show card icon when in list mode
        LV_LOG_USER("Switched to list view");
    } else {
        // Switch to card view
        current_view_mode = PrintSelectViewMode::CARD;
        lv_obj_remove_flag(card_view_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(list_view_container, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(view_toggle_icon, ICON_LIST);  // Show list icon when in card mode
        LV_LOG_USER("Switched to card view");
    }

    update_empty_state();
}

// ============================================================================
// Sorting
// ============================================================================
static void apply_sort() {
    std::sort(file_list.begin(), file_list.end(), [](const PrintFileData& a, const PrintFileData& b) {
        bool result = false;

        switch (current_sort_column) {
            case PrintSelectSortColumn::FILENAME:
                result = a.filename < b.filename;
                break;
            case PrintSelectSortColumn::SIZE:
                result = a.file_size_bytes < b.file_size_bytes;
                break;
            case PrintSelectSortColumn::MODIFIED:
                result = a.modified_timestamp > b.modified_timestamp;  // Newer first by default
                break;
            case PrintSelectSortColumn::PRINT_TIME:
                result = a.print_time_minutes < b.print_time_minutes;
                break;
            case PrintSelectSortColumn::FILAMENT:
                result = a.filament_grams < b.filament_grams;
                break;
        }

        // Reverse if descending
        if (current_sort_direction == PrintSelectSortDirection::DESCENDING) {
            result = !result;
        }

        return result;
    });
}

void ui_panel_print_select_sort_by(PrintSelectSortColumn column) {
    // Toggle direction if same column, otherwise default to ascending
    if (column == current_sort_column) {
        current_sort_direction = (current_sort_direction == PrintSelectSortDirection::ASCENDING)
                                ? PrintSelectSortDirection::DESCENDING
                                : PrintSelectSortDirection::ASCENDING;
    } else {
        current_sort_column = column;
        current_sort_direction = PrintSelectSortDirection::ASCENDING;
    }

    apply_sort();
    update_sort_indicators();

    // Repopulate current view
    if (current_view_mode == PrintSelectViewMode::CARD) {
        populate_card_view();
    } else {
        populate_list_view();
    }

    LV_LOG_USER("Sorted by column %d, direction %d", (int)column, (int)current_sort_direction);
}

static void update_sort_indicators() {
    // Update column header labels to show sort direction
    const char* header_names[] = {"header_filename", "header_size", "header_modified",
                                  "header_print_time"};
    const char* column_labels[] = {"Filename", "Size", "Modified", "Time"};
    PrintSelectSortColumn columns[] = {PrintSelectSortColumn::FILENAME,
                                       PrintSelectSortColumn::SIZE,
                                       PrintSelectSortColumn::MODIFIED,
                                       PrintSelectSortColumn::PRINT_TIME};

    for (int i = 0; i < 4; i++) {
        lv_obj_t* header = lv_obj_find_by_name(panel_root_widget, header_names[i]);
        if (header) {
            // Get label child
            lv_obj_t* label = lv_obj_get_child(header, 0);
            if (label) {
                char text[64];
                if (columns[i] == current_sort_column) {
                    const char* arrow = (current_sort_direction == PrintSelectSortDirection::ASCENDING) ? "▲" : "▼";
                    snprintf(text, sizeof(text), "%s %s", column_labels[i], arrow);
                } else {
                    snprintf(text, sizeof(text), "%s", column_labels[i]);
                }
                lv_label_set_text(label, text);
            }
        }
    }
}

// ============================================================================
// Populate views
// ============================================================================
void ui_panel_print_select_populate_test_data(lv_obj_t* panel_root) {
    if (!panel_root) {
        LV_LOG_ERROR("Cannot populate: panel_root is null");
        return;
    }

    // Clear existing file list
    file_list.clear();

    // Generate test file data
    struct TestFile {
        const char* filename;
        size_t size_bytes;
        int days_ago;         // For timestamp calculation
        int print_time_mins;
        float filament_grams;
    };

    TestFile test_files[] = {
        {"Benchy.gcode", 1024 * 512, 1, 150, 45.0f},
        {"Calibration_Cube.gcode", 1024 * 128, 2, 45, 12.0f},
        {"Large_Vase_With_Very_Long_Filename_That_Should_Truncate.gcode", 1024 * 1024 * 2, 3, 30, 8.0f},
        {"Gear_Assembly.gcode", 1024 * 768, 5, 150, 45.0f},
        {"Flower_Pot.gcode", 1024 * 1024, 7, 240, 85.0f},
        {"Keychain.gcode", 1024 * 64, 10, 480, 120.0f},
        {"Lithophane_Test.gcode", 1024 * 1024 * 5, 14, 5, 2.0f},
        {"Headphone_Stand.gcode", 1024 * 256, 20, 15, 4.5f},
    };

    time_t now = time(nullptr);
    for (const auto& file : test_files) {
        PrintFileData data;
        data.filename = file.filename;
        data.thumbnail_path = DEFAULT_PLACEHOLDER_THUMB;
        data.file_size_bytes = file.size_bytes;
        data.modified_timestamp = now - (file.days_ago * 86400);  // 86400 seconds per day
        data.print_time_minutes = file.print_time_mins;
        data.filament_grams = file.filament_grams;

        // Format strings
        data.size_str = format_file_size(data.file_size_bytes);
        data.modified_str = format_modified_date(data.modified_timestamp);
        data.print_time_str = format_print_time(data.print_time_minutes);
        data.filament_str = format_filament_weight(data.filament_grams);

        file_list.push_back(data);
    }

    // Apply initial sort
    apply_sort();
    update_sort_indicators();

    // Populate both views
    populate_card_view();
    populate_list_view();

    // Update empty state
    update_empty_state();

    LV_LOG_USER("Populated print select panel with %d test files", (int)file_list.size());
}

static void populate_card_view() {
    if (!card_view_container) return;

    // Clear existing cards
    lv_obj_clean(card_view_container);

    // Get responsive layout for current screen size
    CardLayout layout = get_card_layout();

    // Update container gap
    lv_obj_set_style_pad_gap(card_view_container, layout.gap, LV_PART_MAIN);

    for (const auto& file : file_list) {
        // Create XML attributes array
        const char* attrs[] = {
            "thumbnail_src", file.thumbnail_path.c_str(),
            "filename", file.filename.c_str(),
            "print_time", file.print_time_str.c_str(),
            "filament_weight", file.filament_str.c_str(),
            NULL
        };

        // Create card from appropriate responsive component (5col, 4col, or 3col)
        lv_obj_t* card = (lv_obj_t*)lv_xml_create(card_view_container, layout.component_name, attrs);

        // Attach click handler
        if (card) {
            attach_card_click_handler(card, file);
        }
    }
}

static void populate_list_view() {
    if (!list_rows_container) return;

    // Clear existing rows
    lv_obj_clean(list_rows_container);

    for (const auto& file : file_list) {
        // Create XML attributes array
        const char* attrs[] = {
            "filename", file.filename.c_str(),
            "file_size", file.size_str.c_str(),
            "modified_date", file.modified_str.c_str(),
            "print_time", file.print_time_str.c_str(),
            NULL
        };

        // Create list row from XML component
        lv_obj_t* row = (lv_obj_t*)lv_xml_create(list_rows_container, "print_file_list_row", attrs);

        // Attach click handler
        if (row) {
            attach_row_click_handler(row, file);
        }
    }
}

static void update_empty_state() {
    if (!empty_state_container) return;

    bool is_empty = file_list.empty();

    if (is_empty) {
        // Show empty state, hide views
        lv_obj_remove_flag(empty_state_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(card_view_container, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(list_view_container, LV_OBJ_FLAG_HIDDEN);
    } else {
        // Hide empty state, show current view
        lv_obj_add_flag(empty_state_container, LV_OBJ_FLAG_HIDDEN);

        if (current_view_mode == PrintSelectViewMode::CARD) {
            lv_obj_remove_flag(card_view_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_add_flag(list_view_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(card_view_container, LV_OBJ_FLAG_HIDDEN);
            lv_obj_remove_flag(list_view_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

// ============================================================================
// Detail view management
// ============================================================================
void ui_panel_print_select_set_file(const char* filename, const char* thumbnail_src,
                                    const char* print_time, const char* filament_weight) {
    lv_subject_copy_string(&selected_filename_subject, filename);
    lv_subject_copy_string(&selected_thumbnail_subject, thumbnail_src);
    lv_subject_copy_string(&selected_print_time_subject, print_time);
    lv_subject_copy_string(&selected_filament_weight_subject, filament_weight);

    LV_LOG_USER("Selected file: %s", filename);
}

void ui_panel_print_select_show_detail_view() {
    if (detail_view_widget) {
        lv_obj_remove_flag(detail_view_widget, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(detail_view_widget);
        lv_subject_set_int(&detail_view_visible_subject, 1);
    }
}

void ui_panel_print_select_hide_detail_view() {
    if (detail_view_widget) {
        lv_obj_add_flag(detail_view_widget, LV_OBJ_FLAG_HIDDEN);
        lv_subject_set_int(&detail_view_visible_subject, 0);
    }
}

static void create_detail_view() {
    if (!panel_root_widget) {
        LV_LOG_ERROR("Cannot create detail view: panel_root_widget is null");
        return;
    }

    if (detail_view_widget) {
        LV_LOG_WARN("Detail view already exists");
        return;
    }

    // Create detail view from XML component
    detail_view_widget = (lv_obj_t*)lv_xml_create(panel_root_widget, "print_file_detail", nullptr);

    if (!detail_view_widget) {
        LV_LOG_ERROR("Failed to create detail view from XML");
        return;
    }

    lv_obj_set_pos(detail_view_widget, 0, 0);
    lv_obj_add_flag(detail_view_widget, LV_OBJ_FLAG_HIDDEN);

    // Set image scale manually (XML scale attribute doesn't work)
    lv_obj_t* thumbnail = lv_obj_find_by_name(detail_view_widget, "detail_thumbnail");
    if (thumbnail) {
        lv_image_set_scale(thumbnail, 568);
    }

    // Wire up back button
    lv_obj_t* back_button = lv_obj_find_by_name(detail_view_widget, "back_button");
    if (back_button) {
        lv_obj_add_event_cb(back_button, [](lv_event_t* e) {
            (void)e;
            ui_panel_print_select_hide_detail_view();
        }, LV_EVENT_CLICKED, nullptr);
    }

    // Wire up delete button
    lv_obj_t* delete_button = lv_obj_find_by_name(detail_view_widget, "delete_button");
    if (delete_button) {
        lv_obj_add_event_cb(delete_button, [](lv_event_t* e) {
            (void)e;
            ui_panel_print_select_show_delete_confirmation();
        }, LV_EVENT_CLICKED, nullptr);
    }

    // Click backdrop to close
    lv_obj_add_event_cb(detail_view_widget, [](lv_event_t* e) {
        lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
        lv_obj_t* current_target = (lv_obj_t*)lv_event_get_current_target(e);
        if (target == current_target) {
            ui_panel_print_select_hide_detail_view();
        }
    }, LV_EVENT_CLICKED, nullptr);

    LV_LOG_USER("Detail view created");
}

// ============================================================================
// Delete confirmation dialog
// ============================================================================
void ui_panel_print_select_show_delete_confirmation() {
    if (!confirmation_dialog_widget) {
        LV_LOG_ERROR("Confirmation dialog not created");
        return;
    }

    // Update dialog message with current filename
    lv_obj_t* message_label = lv_obj_find_by_name(confirmation_dialog_widget, "dialog_message");
    if (message_label) {
        char msg_buf[256];
        snprintf(msg_buf, sizeof(msg_buf), "Are you sure you want to delete '%s'? This action cannot be undone.",
                 selected_filename_buffer);
        lv_label_set_text(message_label, msg_buf);
    }

    lv_obj_remove_flag(confirmation_dialog_widget, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(confirmation_dialog_widget);
    LV_LOG_USER("Delete confirmation dialog shown");
}

static void hide_delete_confirmation() {
    if (confirmation_dialog_widget) {
        lv_obj_add_flag(confirmation_dialog_widget, LV_OBJ_FLAG_HIDDEN);
    }
}

static void create_confirmation_dialog() {
    if (!parent_screen_widget) {
        LV_LOG_ERROR("Cannot create confirmation dialog: parent_screen_widget is null");
        return;
    }

    if (confirmation_dialog_widget) {
        LV_LOG_WARN("Confirmation dialog already exists");
        return;
    }

    // Create confirmation dialog from XML component (as child of screen for correct z-order)
    const char* attrs[] = {
        "title", "Delete File?",
        "message", "Are you sure you want to delete this file? This action cannot be undone.",
        NULL
    };

    confirmation_dialog_widget = (lv_obj_t*)lv_xml_create(parent_screen_widget, "confirmation_dialog", attrs);

    if (!confirmation_dialog_widget) {
        LV_LOG_ERROR("Failed to create confirmation dialog from XML");
        return;
    }

    lv_obj_add_flag(confirmation_dialog_widget, LV_OBJ_FLAG_HIDDEN);

    // Wire up cancel button
    lv_obj_t* cancel_btn = lv_obj_find_by_name(confirmation_dialog_widget, "dialog_cancel_btn");
    if (cancel_btn) {
        lv_obj_add_event_cb(cancel_btn, [](lv_event_t* e) {
            (void)e;
            hide_delete_confirmation();
        }, LV_EVENT_CLICKED, nullptr);
    }

    // Wire up confirm button
    lv_obj_t* confirm_btn = lv_obj_find_by_name(confirmation_dialog_widget, "dialog_confirm_btn");
    if (confirm_btn) {
        lv_obj_add_event_cb(confirm_btn, [](lv_event_t* e) {
            (void)e;
            // TODO: Implement actual delete functionality
            LV_LOG_USER("File deleted (placeholder action)");
            hide_delete_confirmation();
            ui_panel_print_select_hide_detail_view();
        }, LV_EVENT_CLICKED, nullptr);
    }

    // Click backdrop to cancel
    lv_obj_add_event_cb(confirmation_dialog_widget, [](lv_event_t* e) {
        lv_obj_t* target = (lv_obj_t*)lv_event_get_target(e);
        lv_obj_t* current_target = (lv_obj_t*)lv_event_get_current_target(e);
        if (target == current_target) {
            hide_delete_confirmation();
        }
    }, LV_EVENT_CLICKED, nullptr);

    LV_LOG_USER("Confirmation dialog created");
}

// ============================================================================
// Event handlers
// ============================================================================
static void attach_card_click_handler(lv_obj_t* card, const PrintFileData& file_data) {
    // Allocate persistent data
    PrintFileData* data = new PrintFileData(file_data);

    lv_obj_add_event_cb(card, [](lv_event_t* e) {
        PrintFileData* data = (PrintFileData*)lv_event_get_user_data(e);

        ui_panel_print_select_set_file(
            data->filename.c_str(),
            data->thumbnail_path.c_str(),
            data->print_time_str.c_str(),
            data->filament_str.c_str()
        );

        ui_panel_print_select_show_detail_view();
    }, LV_EVENT_CLICKED, data);
}

static void attach_row_click_handler(lv_obj_t* row, const PrintFileData& file_data) {
    // Allocate persistent data
    PrintFileData* data = new PrintFileData(file_data);

    lv_obj_add_event_cb(row, [](lv_event_t* e) {
        PrintFileData* data = (PrintFileData*)lv_event_get_user_data(e);

        ui_panel_print_select_set_file(
            data->filename.c_str(),
            data->thumbnail_path.c_str(),
            data->print_time_str.c_str(),
            data->filament_str.c_str()
        );

        ui_panel_print_select_show_detail_view();
    }, LV_EVENT_CLICKED, data);
}
