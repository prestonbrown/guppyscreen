#include "ui_panel_controls.h"
#include "ui_component_keypad.h"
#include "ui_panel_motion.h"
#include <cstdio>

// Panel object references
static lv_obj_t* controls_panel = nullptr;
static lv_obj_t* motion_panel = nullptr;
static lv_obj_t* parent_screen = nullptr;

// Card click event handlers
static void card_motion_clicked(lv_event_t* e);
static void card_nozzle_temp_clicked(lv_event_t* e);
static void card_bed_temp_clicked(lv_event_t* e);
static void card_extrusion_clicked(lv_event_t* e);
static void card_fan_clicked(lv_event_t* e);
static void card_motors_clicked(lv_event_t* e);

void ui_panel_controls_init_subjects() {
    // TODO: Initialize subjects for sub-screens
    // For now, no subjects needed at launcher level
    LV_LOG_USER("Controls panel subjects initialized");
}

void ui_panel_controls_wire_events(lv_obj_t* panel_obj) {
    if (!panel_obj) {
        LV_LOG_ERROR("Cannot wire controls panel events: null panel object");
        return;
    }

    // Store parent screen reference (needed for motion panel creation)
    parent_screen = lv_obj_get_parent(panel_obj);

    // Find launcher card objects by name
    lv_obj_t* card_motion = lv_obj_find_by_name(panel_obj, "card_motion");
    lv_obj_t* card_nozzle_temp = lv_obj_find_by_name(panel_obj, "card_nozzle_temp");
    lv_obj_t* card_bed_temp = lv_obj_find_by_name(panel_obj, "card_bed_temp");
    lv_obj_t* card_extrusion = lv_obj_find_by_name(panel_obj, "card_extrusion");
    lv_obj_t* card_fan = lv_obj_find_by_name(panel_obj, "card_fan");
    lv_obj_t* card_motors = lv_obj_find_by_name(panel_obj, "card_motors");

    // Verify all cards found
    if (!card_motion || !card_nozzle_temp || !card_bed_temp ||
        !card_extrusion || !card_fan || !card_motors) {
        LV_LOG_ERROR("Failed to find all controls panel launcher cards");
        return;
    }

    // Wire click event handlers
    lv_obj_add_event_cb(card_motion, card_motion_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(card_nozzle_temp, card_nozzle_temp_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(card_bed_temp, card_bed_temp_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(card_extrusion, card_extrusion_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(card_fan, card_fan_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(card_motors, card_motors_clicked, LV_EVENT_CLICKED, NULL);

    // Make cards clickable
    lv_obj_add_flag(card_motion, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(card_nozzle_temp, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(card_bed_temp, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(card_extrusion, LV_OBJ_FLAG_CLICKABLE);
    // card_fan is disabled (Phase 2), don't make clickable
    lv_obj_add_flag(card_motors, LV_OBJ_FLAG_CLICKABLE);

    LV_LOG_USER("Controls panel events wired");
}

lv_obj_t* ui_panel_controls_get() {
    return controls_panel;
}

void ui_panel_controls_set(lv_obj_t* panel_obj) {
    controls_panel = panel_obj;
}

// ============================================================================
// Card Click Event Handlers
// ============================================================================

// Test callback for keypad confirmation
static void on_nozzle_temp_confirmed(float value, void* user_data) {
    (void)user_data;
    LV_LOG_USER("Nozzle temperature set to: %.0f°C", value);
    // TODO: Send temperature command to printer (moonraker_set_nozzle_temp(value))
}

static void card_motion_clicked(lv_event_t* e) {
    (void)e;
    LV_LOG_USER("Motion card clicked - opening Motion sub-screen");

    // Create motion panel on first access
    if (!motion_panel && parent_screen) {
        LV_LOG_USER("Creating motion panel...");
        motion_panel = (lv_obj_t*)lv_xml_create(parent_screen, "motion_panel", nullptr);

        if (!motion_panel) {
            LV_LOG_ERROR("Failed to create motion panel from XML");
            return;
        }

        // Setup event handlers for motion panel
        ui_panel_motion_setup(motion_panel, parent_screen);

        // Initially hidden
        lv_obj_add_flag(motion_panel, LV_OBJ_FLAG_HIDDEN);
        LV_LOG_USER("Motion panel created and initialized");
    }

    // Show motion panel, hide controls launcher
    if (motion_panel) {
        lv_obj_clear_flag(motion_panel, LV_OBJ_FLAG_HIDDEN);
    }

    // Find and hide the controls launcher content
    if (controls_panel) {
        lv_obj_add_flag(controls_panel, LV_OBJ_FLAG_HIDDEN);
    }
}

static void card_nozzle_temp_clicked(lv_event_t* e) {
    (void)e;
    LV_LOG_USER("Nozzle Temp card clicked - showing numeric keypad");

    // Test the numeric keypad with nozzle temperature config
    ui_keypad_config_t config = {
        .initial_value = 210.0f,
        .min_value = 0.0f,
        .max_value = 350.0f,
        .unit_label = "°C",
        .allow_decimal = false,  // Integer temps only
        .allow_negative = false,
        .callback = on_nozzle_temp_confirmed,
        .user_data = nullptr
    };

    ui_keypad_show(&config);
}

static void card_bed_temp_clicked(lv_event_t* e) {
    LV_LOG_USER("Bed Temp card clicked - opening Heatbed Temperature sub-screen");
    // TODO: Create and show heatbed temperature sub-screen
}

static void card_extrusion_clicked(lv_event_t* e) {
    LV_LOG_USER("Extrusion card clicked - opening Extrusion sub-screen");
    // TODO: Create and show extrusion sub-screen
}

static void card_fan_clicked(lv_event_t* e) {
    LV_LOG_USER("Fan card clicked - Phase 2 feature");
    // TODO: Create and show fan control sub-screen (Phase 2)
}

static void card_motors_clicked(lv_event_t* e) {
    LV_LOG_USER("Motors Disable card clicked");
    // TODO: Show confirmation dialog, then send motors disable command
}
