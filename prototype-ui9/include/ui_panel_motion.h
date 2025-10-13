#pragma once

#include "lvgl/lvgl.h"

/**
 * Motion Panel - XYZ Movement & Homing Control
 *
 * Provides manual jog controls with 3×3 directional pad,
 * distance selector, Z-axis controls, and real-time position display.
 */

// Jog distance options
typedef enum {
    JOG_DIST_0_1MM = 0,
    JOG_DIST_1MM = 1,
    JOG_DIST_10MM = 2,
    JOG_DIST_100MM = 3
} jog_distance_t;

// Jog direction
typedef enum {
    JOG_DIR_N,   // +Y
    JOG_DIR_S,   // -Y
    JOG_DIR_E,   // +X
    JOG_DIR_W,   // -X
    JOG_DIR_NE,  // +X+Y
    JOG_DIR_NW,  // -X+Y
    JOG_DIR_SE,  // +X-Y
    JOG_DIR_SW   // -X-Y
} jog_direction_t;

/**
 * Initialize motion panel reactive subjects.
 * Must be called before creating XML component.
 */
void ui_panel_motion_init_subjects();

/**
 * Setup event handlers for motion panel after XML creation.
 * @param panel - Root object of motion panel (returned from lv_xml_create)
 * @param parent_screen - Parent screen object (for navigation)
 */
void ui_panel_motion_setup(lv_obj_t* panel, lv_obj_t* parent_screen);

/**
 * Update XYZ position display.
 * @param x - X position in mm
 * @param y - Y position in mm
 * @param z - Z position in mm
 */
void ui_panel_motion_set_position(float x, float y, float z);

/**
 * Get currently selected jog distance.
 * @return Current jog distance setting
 */
jog_distance_t ui_panel_motion_get_distance();

/**
 * Set jog distance selection.
 * @param dist - Distance to select
 */
void ui_panel_motion_set_distance(jog_distance_t dist);

/**
 * Send jog command (mock for now - will integrate with Klipper API later).
 * @param direction - Direction to jog
 * @param distance_mm - Distance in mm
 */
void ui_panel_motion_jog(jog_direction_t direction, float distance_mm);

/**
 * Send home command (mock for now).
 * @param axis - 'X', 'Y', 'Z', or 'A' for all axes
 */
void ui_panel_motion_home(char axis);
