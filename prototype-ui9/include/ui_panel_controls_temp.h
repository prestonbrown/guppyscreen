#pragma once

#include "lvgl/lvgl.h"

/**
 * Temperature Control Panels - Nozzle & Heatbed
 *
 * Provides temperature management with preset buttons and custom keypad input.
 * Shared logic for both nozzle and bed temperature screens.
 */

/**
 * Initialize temperature panel reactive subjects.
 * Must be called before creating XML components.
 */
void ui_panel_controls_temp_init_subjects();

/**
 * Setup event handlers for nozzle temperature panel after XML creation.
 * @param panel - Root object of nozzle_temp_panel (returned from lv_xml_create)
 * @param parent_screen - Parent screen object (for navigation)
 */
void ui_panel_controls_temp_nozzle_setup(lv_obj_t* panel, lv_obj_t* parent_screen);

/**
 * Setup event handlers for bed temperature panel after XML creation.
 * @param panel - Root object of bed_temp_panel (returned from lv_xml_create)
 * @param parent_screen - Parent screen object (for navigation)
 */
void ui_panel_controls_temp_bed_setup(lv_obj_t* panel, lv_obj_t* parent_screen);

/**
 * Update nozzle temperature display.
 * @param current - Current nozzle temperature in °C
 * @param target - Target nozzle temperature in °C
 */
void ui_panel_controls_temp_set_nozzle(int current, int target);

/**
 * Update bed temperature display.
 * @param current - Current bed temperature in °C
 * @param target - Target bed temperature in °C
 */
void ui_panel_controls_temp_set_bed(int current, int target);

/**
 * Get current nozzle target temperature.
 * @return Target temperature in °C
 */
int ui_panel_controls_temp_get_nozzle_target();

/**
 * Get current bed target temperature.
 * @return Target temperature in °C
 */
int ui_panel_controls_temp_get_bed_target();
