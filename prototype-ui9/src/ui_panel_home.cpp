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

#include "ui_panel_home.h"
#include "ui_theme.h"
#include "ui_fonts.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

static lv_obj_t* home_panel = nullptr;

// Widget references for direct updates
static lv_obj_t* network_icon_label = nullptr;
static lv_obj_t* network_text_label = nullptr;
static lv_obj_t* light_icon_label = nullptr;

// Subjects for reactive data binding
static lv_subject_t status_subject;
static lv_subject_t temp_subject;
static lv_subject_t network_icon_subject;
static lv_subject_t network_label_subject;
static lv_subject_t network_color_subject;
static lv_subject_t light_icon_color_subject;

static char status_buffer[128];
static char temp_buffer[32];
static char network_icon_buffer[8];
static char network_label_buffer[32];
static char network_color_buffer[16];

static bool subjects_initialized = false;
static bool light_on = false;
static network_type_t current_network = NETWORK_WIFI;

// Forward declarations
static void light_toggle_event_cb(lv_event_t* e);
static void network_observer_cb(lv_observer_t* observer, lv_subject_t* subject);
static void light_observer_cb(lv_observer_t* observer, lv_subject_t* subject);

void ui_panel_home_init_subjects() {
    if (subjects_initialized) {
        printf("WARNING: Home panel subjects already initialized\n");
        return;
    }

    printf("DEBUG: Initializing home panel subjects\n");

    // Initialize subjects with default values
    lv_subject_init_string(&status_subject, status_buffer, NULL, sizeof(status_buffer), "Hasta la vista, misprints!");
    lv_subject_init_string(&temp_subject, temp_buffer, NULL, sizeof(temp_buffer), "30 °C");
    lv_subject_init_string(&network_icon_subject, network_icon_buffer, NULL, sizeof(network_icon_buffer), ICON_WIFI);
    lv_subject_init_string(&network_label_subject, network_label_buffer, NULL, sizeof(network_label_buffer), "Wi-Fi");
    lv_subject_init_string(&network_color_subject, network_color_buffer, NULL, sizeof(network_color_buffer), "0xff4444");
    lv_subject_init_color(&light_icon_color_subject, lv_color_hex(0x909090));  // Muted gray for "off" state

    // Register subjects globally so XML can bind to them
    lv_xml_register_subject(NULL, "status_text", &status_subject);
    lv_xml_register_subject(NULL, "temp_text", &temp_subject);
    lv_xml_register_subject(NULL, "network_icon", &network_icon_subject);
    lv_xml_register_subject(NULL, "network_label", &network_label_subject);
    lv_xml_register_subject(NULL, "network_color", &network_color_subject);
    lv_xml_register_subject(NULL, "light_icon_color", &light_icon_color_subject);

    // Register event callbacks BEFORE loading XML
    lv_xml_register_event_cb(NULL, "light_toggle_cb", light_toggle_event_cb);

    subjects_initialized = true;
    printf("DEBUG: Registered subjects: status_text, temp_text, network_icon, network_label, network_color, light_icon_color\n");
    printf("DEBUG: Registered event callback: light_toggle_cb at address %p\n", (void*)light_toggle_event_cb);
}

void ui_panel_home_setup_observers(lv_obj_t* panel) {
    if (!subjects_initialized) {
        printf("ERROR: Subjects not initialized! Call ui_panel_home_init_subjects() first!\n");
        return;
    }

    home_panel = panel;

    // Use LVGL 9's name-based widget lookup - resilient to layout changes
    network_icon_label = lv_obj_find_by_name(home_panel, "network_icon");
    network_text_label = lv_obj_find_by_name(home_panel, "network_label");
    light_icon_label = lv_obj_find_by_name(home_panel, "light_icon");

    if (!network_icon_label || !network_text_label || !light_icon_label) {
        printf("ERROR: Failed to find named widgets (net_icon=%p, net_label=%p, light=%p)\n",
               network_icon_label, network_text_label, light_icon_label);
        return;
    }

    // Get screen dimensions for responsive sizing
    lv_display_t* display = lv_display_get_default();
    int32_t screen_height = lv_display_get_vertical_resolution(display);

    // 1. Set responsive printer image size and SCALE (not just crop/pad)
    lv_obj_t* printer_image = lv_obj_find_by_name(home_panel, "printer_image");
    if (printer_image) {
        int32_t printer_size;
        uint16_t zoom_level;  // 256 = 100%, 128 = 50%, 512 = 200%

        // Calculate printer image size and zoom based on screen height
        if (screen_height <= UI_SCREEN_TINY_H) {
            printer_size = 150;  // Tiny screens (480x320)
            zoom_level = 96;     // 37.5% zoom to scale 400px -> 150px
        } else if (screen_height <= UI_SCREEN_SMALL_H) {
            printer_size = 250;  // Small screens (800x480)
            zoom_level = 160;    // 62.5% zoom to scale 400px -> 250px
        } else if (screen_height <= UI_SCREEN_MEDIUM_H) {
            printer_size = 300;  // Medium screens (1024x600)
            zoom_level = 192;    // 75% zoom to scale 400px -> 300px
        } else {
            printer_size = 400;  // Large screens (1280x720+)
            zoom_level = 256;    // 100% zoom (original size)
        }

        lv_obj_set_width(printer_image, printer_size);
        lv_obj_set_height(printer_image, printer_size);
        lv_image_set_scale(printer_image, zoom_level);  // Actually scale the image
        LV_LOG_USER("Set printer image: size=%dpx, zoom=%d (%d%%) for screen height %d",
                    printer_size, zoom_level, (zoom_level * 100) / 256, screen_height);
    } else {
        LV_LOG_WARN("Printer image not found - size not adjusted");
    }

    // 2. Set responsive info card icon sizes
    const lv_font_t* info_icon_font;
    if (screen_height <= UI_SCREEN_TINY_H) {
        info_icon_font = &fa_icons_24;  // Tiny: 24px icons
    } else if (screen_height <= UI_SCREEN_SMALL_H) {
        info_icon_font = &fa_icons_24;  // Small: 24px icons
    } else {
        info_icon_font = &fa_icons_48;  // Medium/Large: 48px icons
    }

    lv_obj_t* temp_icon = lv_obj_find_by_name(home_panel, "temp_icon");
    if (temp_icon) lv_obj_set_style_text_font(temp_icon, info_icon_font, 0);

    if (network_icon_label) lv_obj_set_style_text_font(network_icon_label, info_icon_font, 0);
    if (light_icon_label) lv_obj_set_style_text_font(light_icon_label, info_icon_font, 0);

    // 3. Set responsive status text font for tiny screens
    if (screen_height <= UI_SCREEN_TINY_H) {
        lv_obj_t* status_text = lv_obj_find_by_name(home_panel, "status_text_label");
        if (status_text) {
            lv_obj_set_style_text_font(status_text, &lv_font_montserrat_20, 0);  // Smaller font for tiny
            LV_LOG_USER("Set status text to montserrat_20 for tiny screen");
        }
    }

    int icon_size = (info_icon_font == &fa_icons_24) ? 24 : (info_icon_font == &fa_icons_32) ? 32 : 48;
    LV_LOG_USER("Set info card icons to %dpx for screen height %d", icon_size, screen_height);

    // Add observers to watch subjects and update widgets
    lv_subject_add_observer(&network_icon_subject, network_observer_cb, nullptr);
    lv_subject_add_observer(&network_label_subject, network_observer_cb, nullptr);
    lv_subject_add_observer(&network_color_subject, network_observer_cb, nullptr);
    lv_subject_add_observer(&light_icon_color_subject, light_observer_cb, nullptr);

    // Apply initial light icon color (observers only fire on *changes*, not initial state)
    if (light_icon_label) {
        lv_color_t initial_color = lv_subject_get_color(&light_icon_color_subject);
        lv_obj_set_style_img_recolor(light_icon_label, initial_color, LV_PART_MAIN);
        lv_obj_set_style_img_recolor_opa(light_icon_label, 255, LV_PART_MAIN);
        printf("DEBUG: Applied initial light icon color\n");
    }

    printf("DEBUG: Home panel observers set up successfully\n");
}

lv_obj_t* ui_panel_home_create(lv_obj_t* parent) {
    printf("DEBUG: ui_panel_home_create called\n");

    if (!subjects_initialized) {
        printf("ERROR: Subjects not initialized! Call ui_panel_home_init_subjects() first!\n");
        return nullptr;
    }

    // Create the XML component (will bind to subjects automatically)
    home_panel = (lv_obj_t*)lv_xml_create(parent, "home_panel", nullptr);
    if (!home_panel) {
        printf("ERROR: Failed to create home_panel from XML\n");
        return nullptr;
    }

    // Setup observers
    ui_panel_home_setup_observers(home_panel);

    printf("DEBUG: XML home_panel created successfully with reactive observers\n");
    return home_panel;
}

void ui_panel_home_update(const char* status_text, int temp) {
    // Update subjects - all bound widgets update automatically
    if (status_text) {
        lv_subject_copy_string(&status_subject, status_text);
        printf("DEBUG: Updated status_text subject to: %s\n", status_text);
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%d °C", temp);
    lv_subject_copy_string(&temp_subject, buf);
    printf("DEBUG: Updated temp_text subject to: %s\n", buf);
}

void ui_panel_home_set_network(network_type_t type) {
    current_network = type;

    switch (type) {
        case NETWORK_WIFI:
            lv_subject_copy_string(&network_icon_subject, ICON_WIFI);
            lv_subject_copy_string(&network_label_subject, "Wi-Fi");
            lv_subject_copy_string(&network_color_subject, "0xff4444");  // Primary color
            break;
        case NETWORK_ETHERNET:
            lv_subject_copy_string(&network_icon_subject, ICON_ETHERNET);
            lv_subject_copy_string(&network_label_subject, "Ethernet");
            lv_subject_copy_string(&network_color_subject, "0xff4444");  // Primary color
            break;
        case NETWORK_DISCONNECTED:
            lv_subject_copy_string(&network_icon_subject, ICON_WIFI_SLASH);
            lv_subject_copy_string(&network_label_subject, "Disconnected");
            lv_subject_copy_string(&network_color_subject, "0x909090");  // Text secondary
            break;
    }
    printf("DEBUG: Updated network status to type %d\n", type);
}

void ui_panel_home_set_light(bool is_on) {
    light_on = is_on;

    if (is_on) {
        // Light is on - show bright yellow/white
        lv_subject_set_color(&light_icon_color_subject, lv_color_hex(0xFFD700));
    } else {
        // Light is off - show muted gray
        lv_subject_set_color(&light_icon_color_subject, lv_color_hex(0x909090));
    }
    printf("DEBUG: Updated light state to: %s\n", is_on ? "ON" : "OFF");
}

bool ui_panel_home_get_light_state() {
    return light_on;
}

static void light_toggle_event_cb(lv_event_t* e) {
    (void)e;  // Unused parameter

    printf("====== LIGHT BUTTON CLICKED! ======\n");

    // Toggle the light state
    ui_panel_home_set_light(!light_on);

    // TODO: Add callback to send command to Klipper
    // For now, just log the state change
    printf("Light toggled: %s\n", light_on ? "ON" : "OFF");
}

// Observer callback for network icon/label/color changes
static void network_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
    (void)observer;  // Unused parameter

    if (!network_icon_label || !network_text_label) {
        return;
    }

    // Update network icon text
    const char* icon = lv_subject_get_string(&network_icon_subject);
    if (icon) {
        lv_label_set_text(network_icon_label, icon);
    }

    // Update network label text
    const char* label = lv_subject_get_string(&network_label_subject);
    if (label) {
        lv_label_set_text(network_text_label, label);
    }

    // Update network icon color
    const char* color_str = lv_subject_get_string(&network_color_subject);
    if (color_str) {
        uint32_t color = strtoul(color_str, nullptr, 16);
        lv_obj_set_style_text_color(network_icon_label, lv_color_hex(color), 0);
        lv_obj_set_style_text_color(network_text_label, lv_color_hex(color), 0);
    }

    printf("DEBUG: Network observer updated widgets\n");
}

// Observer callback for light icon color changes
static void light_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
    (void)observer;  // Unused parameter

    if (!light_icon_label) {
        return;
    }

    // Update light icon color using image recolor (Material Design icons are monochrome)
    lv_color_t color = lv_subject_get_color(subject);
    lv_obj_set_style_img_recolor(light_icon_label, color, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(light_icon_label, 255, LV_PART_MAIN);

    printf("DEBUG: Light observer updated icon color\n");
}
