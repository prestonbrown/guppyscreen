# LVGL 9 XML UI System - Complete Guide

This comprehensive guide covers LVGL 9's declarative XML UI system with reactive data binding, based on LVGL 9.3 documentation and practical experience building the GuppyScreen UI prototype.

**Last Updated:** 2025-01-11

---

## Table of Contents

1. [Overview & Architecture](#overview--architecture)
2. [Project Structure](#project-structure)
3. [Core Concepts](#core-concepts)
4. [Layouts & Positioning](#layouts--positioning)
5. [Common UI Patterns](#common-ui-patterns)
6. [Styles & Theming](#styles--theming)
7. [Event Handling](#event-handling)
8. [Implementation Guide](#implementation-guide)
9. [Best Practices](#best-practices)
10. [Troubleshooting](#troubleshooting)
11. [Resources](#resources)

---

## Overview & Architecture

LVGL 9's XML system enables declarative UI development with reactive data binding through the Subject-Observer pattern. This approach separates UI layout (XML) from business logic (C++), similar to modern web frameworks like React or Vue.

### Architecture Diagram

```
┌─────────────────┐
│  XML Component  │ ← Declarative UI layout
│  (home_panel)   │
└────────┬────────┘
         │ bind_text="subject_name"
         ↓
┌─────────────────┐
│    Subjects     │ ← Reactive data (strings, ints, colors)
│  (status_text)  │
└────────┬────────┘
         │ lv_subject_copy_string()
         ↓
┌─────────────────┐
│  C++ Wrapper    │ ← Business logic & state updates
│ (ui_panel_*.cpp)│
└─────────────────┘
```

### Key Benefits

**XML Approach:**
- Declarative and concise
- Separation of concerns (layout vs. logic)
- Reactive data binding (automatic UI updates)
- Rapid iteration (no recompile for layout changes)
- Designer-friendly

**vs. Traditional C++ Approach:**
- Less verbose (3 lines XML vs. 15 lines C++)
- No manual widget management
- Type-safe at runtime through subject system
- Theme-able with global constants

---

## Project Structure

### Standard LVGL Editor Project Layout

```
project_name/
├── project.xml          # Project config (targets, display sizes)
├── globals.xml          # Global resources (fonts, images, subjects, consts)
├── screens/             # Screen XML files (<screen> tag)
├── components/          # Reusable components (<component> tag)
├── widgets/             # Custom widget definitions
├── ui_project_name.c    # User code (calls init_gen)
└── ui_project_name_gen.c # Generated code (auto-created by LVGL Editor)
```

### GuppyScreen Prototype Structure

```
prototype-ui9/
├── ui_xml/
│   ├── globals.xml            # Theme constants + auto-generated icons
│   ├── app_layout.xml         # Root: navbar + content area
│   ├── navigation_bar.xml     # 5-button vertical navigation
│   └── *_panel.xml            # Panel components (home, controls, etc.)
├── src/
│   ├── main.cpp               # Entry point, initialization
│   ├── ui_nav.cpp             # Navigation system
│   └── ui_panel_*.cpp         # Panel logic with subject management
├── include/
│   ├── ui_nav.h
│   ├── ui_panel_*.h
│   └── ui_fonts.h             # FontAwesome icon definitions
├── assets/
│   ├── fonts/                 # Custom fonts (FontAwesome 6)
│   └── images/                # UI images (printer, filament spool)
└── docs/
    └── LVGL9_XML_GUIDE.md     # This file
```

---

## Core Concepts

### 1. XML Components

Components are reusable UI pieces defined with the `<component>` tag.

#### Basic Structure

```xml
<component>
    <!-- Optional: Component API properties -->
    <api>
        <prop name="text" type="string" default="Click me"/>
        <prop name="subject" type="subject" default=""/>
        <prop name="enabled" type="bool" default="true"/>
    </api>

    <!-- Optional: Local constants -->
    <consts>
        <int name="button_size" value="36"/>
    </consts>

    <!-- Optional: Local styles -->
    <styles>
        <style name="style_base" bg_color="0x333" text_color="0xfff"/>
    </styles>

    <!-- The actual UI definition -->
    <view extends="lv_button" width="#button_size">
        <!-- Use API properties with $ prefix -->
        <lv_label text="$text" align="center"/>

        <!-- Reference local or global constants with # prefix -->
        <style name="style_base"/>
    </view>
</component>
```

#### Globals.xml Structure

The `globals.xml` file contains all shared resources:

```xml
<component>
    <consts>
        <!-- Colors -->
        <color name="bg_dark" value="0x1a1a1a"/>
        <color name="panel_bg" value="0x111410"/>
        <color name="text_primary" value="0xffffff"/>
        <color name="primary_color" value="0xff4444"/>

        <!-- Dimensions -->
        <px name="nav_width" value="102"/>
        <px name="padding_normal" value="20"/>
        <px name="accent_bar_width" value="4"/>
        <percent name="card_width" value="45%"/>

        <!-- Icon strings (auto-generated) -->
        <str name="icon_home" value=""/>  <!-- UTF-8 char -->
    </consts>

    <!-- Optional: Global styles -->
    <styles>
        <style name="style_button" bg_color="0x111" radius="8"/>
    </styles>

    <view extends="lv_obj"/>
</component>
```

**Using Constants:**
```xml
<!-- Reference with # prefix -->
<lv_obj width="#nav_width" style_bg_color="#primary_color"/>
```

### 2. Subjects (Reactive Data)

Subjects are observable data containers that automatically update all bound widgets when changed.

#### Subject Types

```c
lv_subject_init_string()  // String data (text labels)
lv_subject_init_int()     // Integer data (sliders, counters)
lv_subject_init_pointer() // Pointer data (custom objects)
lv_subject_init_color()   // Color data (dynamic theming)
```

#### Subject Lifecycle

```c
// 1. Create subject in C++
static lv_subject_t status_subject;
static char status_buffer[128];

// 2. Initialize with default value
lv_subject_init_string(&status_subject, status_buffer, NULL,
                       sizeof(status_buffer), "Initial status");

// 3. Register globally (before creating XML)
lv_xml_register_subject(NULL, "status_text", &status_subject);

// 4. Create XML (widgets automatically bind)
lv_obj_t* panel = lv_xml_create(parent, "home_panel", NULL);

// 5. Update subject (all bound widgets update automatically)
lv_subject_copy_string(&status_subject, "New status");
```

**⚠️ CRITICAL:** Register subjects BEFORE creating XML components that bind to them.

#### Scope: Global vs. Component

**Global scope** (`NULL` parameter):
- Subjects accessible to all XML components
- Use for shared data (status, temperature, active panel)

**Component scope** (non-NULL parameter):
- Subjects only accessible within specific component
- Use for component-internal state

### 3. Data Binding

XML widgets can bind to subjects using special attributes.

#### Supported Bindings

```xml
<!-- Bind label text to string subject -->
<lv_label bind_text="status_text" style_text_color="#text_primary"/>

<!-- Bind with format string -->
<lv_label bind_text="temp_value" bind_text-fmt="%.1f°C"/>

<!-- Bind slider value to integer subject -->
<lv_slider bind_value="volume" min_value="0" max_value="100"/>

<!-- Conditional flag binding (show/hide based on subject value) -->
<lv_obj bind_flag_if_eq="subject=active_panel flag=hidden ref_value=0"/>

<!-- Conditional style binding -->
<lv_obj>
    <bind_style name="style_dark" subject="dark_theme" ref_value="1"/>
</lv_obj>

<!-- Reactive color binding -->
<lv_label bind_style_text_color="nav_icon_0_color" text="#icon_home"/>
```

#### Defining Subjects in globals.xml (Optional)

While subjects can be defined in XML, it's recommended to initialize them in C++ for better control:

```xml
<!-- In globals.xml (optional - for documentation) -->
<subjects>
    <int name="volume" value="10"/>
    <string name="status" value="Ready"/>
    <float name="temp" value="25.0"/>
</subjects>
```

**Note:** If you define subjects in XML, they'll be registered with empty/default values before C++ initialization. Prefer C++ initialization for predictable behavior.

---

## Layouts & Positioning

### Flex Layout Fundamentals

LVGL 9 XML uses flexbox-style layouts for responsive positioning.

#### Flex Flow Options

```xml
<lv_obj flex_flow="row">          <!-- Horizontal left to right -->
<lv_obj flex_flow="row_reverse">  <!-- Horizontal right to left -->
<lv_obj flex_flow="column">       <!-- Vertical top to bottom -->
<lv_obj flex_flow="column_reverse"> <!-- Vertical bottom to top -->
```

#### ⚠️ CRITICAL: `flex_align` DOES NOT WORK!

**❌ INCORRECT - This attribute is silently ignored:**
```xml
<lv_obj flex_flow="row" flex_align="center center center">
```

**✓ CORRECT - Use `style_flex_*` properties:**
```xml
<lv_obj flex_flow="row"
        style_flex_main_place="center"
        style_flex_cross_place="center">
```

#### Flex Alignment Properties

**Values for `style_flex_main_place`** (alignment along primary axis):
- `start` - Align to start (default)
- `center` - Center align
- `end` - Align to end
- `space_between` - Distribute with space between items
- `space_evenly` - Distribute with even space around items
- `space_around` - Distribute with space around items

**Values for `style_flex_cross_place`** (alignment along cross axis):
- `start` - Align to start (default)
- `center` - Center align
- `end` - Align to end
- `stretch` - Stretch to fill

#### Axis Direction

**For `flex_flow="row"` (horizontal):**
- `style_flex_main_place` controls **horizontal** alignment
- `style_flex_cross_place` controls **vertical** alignment

**For `flex_flow="column"` (vertical):**
- `style_flex_main_place` controls **vertical** alignment
- `style_flex_cross_place` controls **horizontal** alignment

#### Flex Grow for Responsive Distribution

```xml
<lv_obj flex_flow="row">
    <lv_label text="Left"/>
    <lv_label text="Center" flex_grow="1"/>  <!-- Takes remaining space -->
    <lv_label text="Right"/>
</lv_obj>
```

**Equal distribution:**
```xml
<lv_obj flex_flow="row">
    <lv_obj flex_grow="1">Content 1</lv_obj>  <!-- 33% -->
    <lv_obj flex_grow="1">Content 2</lv_obj>  <!-- 33% -->
    <lv_obj flex_grow="1">Content 3</lv_obj>  <!-- 33% -->
</lv_obj>
```

#### Starting New Track (Line Wrap)

```xml
<lv_obj flex_flow="row">
    <lv_button/>
    <lv_button/>
    <lv_button flex_in_new_track="true"/>  <!-- Starts on new line -->
</lv_obj>
```

### Centering Techniques

Based on extensive testing with the home panel status card.

#### Horizontal Centering

**Text/Icons in Labels:**

```xml
<lv_label
    text="Content"
    style_text_align="center"
    width="100%"/>
```

**Key points:**
- `style_text_align="center"` centers text within the label
- `width="100%"` makes label span full container width
- **Both attributes required** - `style_text_align` alone won't work

**Containers with Flex:**

```xml
<lv_obj flex_flow="row" style_flex_main_place="center">
    <lv_label text="Centered content"/>
</lv_obj>
```

#### Vertical Centering

**Single-Level:**

```xml
<lv_obj flex_flow="column"
        style_flex_main_place="center"
        style_flex_cross_place="center">
    <lv_label text="Vertically centered"/>
</lv_obj>
```

**Multi-Level Nested Centering:**

```xml
<lv_obj flex_grow="1" height="100%"
        flex_flow="column"
        style_flex_main_place="center"
        style_flex_cross_place="center">
    <lv_label text="Icon" style_text_align="center" width="100%"/>
    <lv_label text="Text" style_text_align="center" width="100%"/>
</lv_obj>
```

**Key points:**
- Container needs `height="100%"` to enable vertical centering
- `style_flex_main_place="center"` centers along main axis (vertical for column)
- `style_flex_cross_place="center"` centers along cross axis (horizontal for column)
- Inner labels use `style_text_align="center"` + `width="100%"`

#### Common Centering Combinations

```xml
<!-- Center children both horizontally and vertically (row layout) -->
<lv_obj flex_flow="row"
        style_flex_main_place="center"
        style_flex_cross_place="center">
    <!-- Children centered in both directions -->
</lv_obj>

<!-- Evenly distribute horizontally, center vertically (row layout) -->
<lv_obj flex_flow="row"
        style_flex_main_place="space_evenly"
        style_flex_cross_place="center">
    <!-- Children spread out horizontally, centered vertically -->
</lv_obj>

<!-- Stack vertically, center horizontally (column layout) -->
<lv_obj flex_flow="column"
        style_flex_main_place="center"
        style_flex_cross_place="center">
    <!-- Children stacked vertically, centered horizontally -->
</lv_obj>
```

#### Widget Alignment (Alternative to Flex)

For centering children within a parent without flex:

```xml
<lv_button>
    <lv_label align="center" text="Click me"/>  <!-- Equivalent to lv_obj_center() -->
</lv_button>
```

**Alignment options:** `center`, `top_left`, `top_mid`, `top_right`, `bottom_left`, `bottom_mid`, `bottom_right`, `left_mid`, `right_mid`

### Dividers in Flex Layouts

**Vertical dividers** (in row layouts):

```xml
<lv_obj flex_flow="row" style_flex_cross_place="center">
    <lv_obj flex_grow="1">Section 1</lv_obj>

    <!-- Vertical divider -->
    <lv_obj width="1"
            style_align_self="stretch"
            style_bg_color="#text_secondary"
            style_bg_opa="50%"
            style_border_width="0"/>

    <lv_obj flex_grow="1">Section 2</lv_obj>
</lv_obj>
```

**Key points:**
- Use `width="1"` for vertical dividers in row layouts
- Use `style_align_self="stretch"` to extend full height
- Dividers participate in flex layout as children
- Set `style_border_width="0"` to avoid border rendering

---

## Common UI Patterns

### Vertical Accent Bar (Leading Edge Indicator)

A thin vertical colored bar positioned to the left of text, commonly used in material design to draw attention to important content.

**Visual:**
```
| Important message here
```

**Implementation:**

```xml
<!-- Container with flex-grow to fill available space -->
<lv_obj flex_grow="1" style_bg_opa="0%" style_border_width="0" style_pad_all="0"
        flex_flow="row" style_flex_cross_place="center" style_pad_gap="#accent_bar_gap">

    <!-- Vertical accent bar -->
    <lv_obj width="#accent_bar_width" height="#accent_bar_height"
            style_bg_color="#primary_color" style_bg_opa="255"
            style_radius="2" style_border_width="0"/>

    <!-- Content text (flex-grows to fill remaining width) -->
    <lv_label flex_grow="1" bind_text="status_text"
              style_text_color="#text_primary" style_text_font="montserrat_20"/>
</lv_obj>
```

**Required Constants in globals.xml:**

```xml
<px name="accent_bar_width" value="4"/>      <!-- Bar thickness -->
<px name="accent_bar_height" value="28"/>    <!-- Bar height -->
<px name="accent_bar_gap" value="18"/>       <!-- Spacing between bar and text -->
```

**When to use:**
- Status messages or notifications
- Section headers that need emphasis
- Call-to-action text
- Important state information

**Alternative: Full-height bar using `style_align_self="stretch"`:**

```xml
<!-- Bar automatically matches container height -->
<lv_obj width="#accent_bar_width"
        style_align_self="stretch"
        style_bg_color="#primary_color"
        style_bg_opa="255"/>
```

### Clickable Icon Button

Transparent button with FontAwesome icon:

```xml
<lv_button width="64" height="64"
           style_bg_opa="0%"
           style_border_width="0"
           style_shadow_width="0"
           flag_clickable="true">
    <lv_label text="#icon_home" style_text_font="fa_icons_64" align="center"/>
    <lv_event-call_function trigger="clicked" callback="icon_clicked" user_data="0"/>
</lv_button>
```

### Reactive Counter with Buttons

Uses subjects for automatic UI updates:

```xml
<!-- In globals.xml -->
<subjects>
    <int name="counter" value="0"/>
</subjects>

<!-- In component -->
<lv_obj flex_flow="row" style_flex_cross_place="center">
    <lv_button>
        <lv_label text="-"/>
        <subject_increment_event subject="counter" step="-1" min="0"/>
    </lv_button>

    <lv_label bind_text="counter" bind_text-fmt="%d" flex_grow="1" style_text_align="center"/>

    <lv_button>
        <lv_label text="+"/>
        <subject_increment_event subject="counter" step="1" max="100"/>
    </lv_button>
</lv_obj>
```

### Tabbed Navigation

Show/hide panels based on active tab subject:

```xml
<!-- Tab buttons -->
<lv_obj flex_flow="row">
    <lv_button flex_grow="1">
        <lv_label text="Home"/>
        <subject_set_int_event subject="active_tab" value="0"/>
    </lv_button>
    <lv_button flex_grow="1">
        <lv_label text="Settings"/>
        <subject_set_int_event subject="active_tab" value="1"/>
    </lv_button>
</lv_obj>

<!-- Home tab content -->
<lv_obj>
    <bind_flag_if_eq subject="active_tab" flag="hidden" ref_value="1"/>
    <!-- Home content -->
</lv_obj>

<!-- Settings tab content -->
<lv_obj>
    <bind_flag_if_eq subject="active_tab" flag="hidden" ref_value="0"/>
    <!-- Settings content -->
</lv_obj>
```

### Status Card with Icon Stacks

Vertical stacking of icon and label, centered:

```xml
<lv_obj flex_grow="1" height="100%"
        flex_flow="column"
        style_flex_main_place="center"
        style_flex_cross_place="center">
    <!-- Icon on top -->
    <lv_label text="#icon_temperature"
              style_text_font="fa_icons_48"
              style_text_align="center"
              width="100%"/>

    <!-- Text below -->
    <lv_label bind_text="temp_text"
              style_text_font="montserrat_16"
              style_text_align="center"
              width="100%"/>
</lv_obj>
```

---

## Styles & Theming

### Defining Styles

**Global styles** (in globals.xml):

```xml
<styles>
    <style name="style_button"
           bg_color="0x111"
           text_color="0xfff"
           radius="8"
           pad_all="12"/>

    <style name="style_card"
           bg_color="0x202020"
           radius="8"
           border_width="0"/>
</styles>
```

**Local styles** (in component):

```xml
<component>
    <styles>
        <style name="style_dark" bg_color="0x333"/>
    </styles>

    <view>
        <lv_obj>
            <style name="style_dark"/>
        </lv_obj>
    </view>
</component>
```

### Applying Styles

**By name:**

```xml
<lv_button>
    <style name="style_button"/>
</lv_button>
```

**With selector (state-based):**

```xml
<lv_button>
    <style name="style_base"/>
    <style name="style_pressed" selector="pressed"/>
    <style name="style_checked" selector="checked"/>
</lv_button>
```

**Inline styles:**

```xml
<lv_button style_bg_color="0x111" style_radius="8" style_pad_all="12"/>
```

### Conditional Style Binding

Dynamically apply styles based on subject value:

```xml
<lv_obj>
    <bind_style name="style_dark" subject="dark_theme" ref_value="1"/>
</lv_obj>
```

---

## Event Handling

### Custom Callback Events - Use C Code, Not XML

**IMPORTANT**: For custom callback events with application-specific logic, **add event handlers in C code**, not in XML.

XML event elements like `<event_cb>` and `<lv_event-call_function>` are internal implementation details and should not be used directly in most cases.

**Correct approach:**

**XML** (clean, no event elements):
```xml
<lv_button name="my_button">
    <lv_label text="Click me"/>
</lv_button>
```

**C Code** (add handlers after XML creation):
```c
// Get button widget from XML-created UI
lv_obj_t* button = lv_obj_find_by_name(parent, "my_button");

// Add event handler
lv_obj_add_event_cb(button, my_button_clicked, LV_EVENT_CLICKED, user_data);

// Event handler function
static void my_button_clicked(lv_event_t* e) {
    void* user_data = lv_event_get_user_data(e);
    // Handle click
}
```

### XML Event Elements - For Built-in Actions Only

XML supports built-in event types for common reactive patterns.

#### Event Triggers

Common trigger values:
- `"clicked"` - Button click (press + release)
- `"pressed"` - Button pressed down
- `"released"` - Button released
- `"long_pressed"` - Long press detected
- `"long_pressed_repeat"` - Held down (repeats)
- `"all"` - Any event

#### Custom Callback Events (Advanced)

**XML:**
```xml
<lv_button>
    <lv_label text="Click"/>
    <lv_event-call_function trigger="clicked" callback="my_handler" user_data="42"/>
</lv_button>
```

**C Code Registration (MUST happen BEFORE loading XML):**
```c
// Register callback with LVGL XML system
lv_xml_register_event_cb(NULL, "my_handler", my_handler_function);

// Standard LVGL event handler signature
void my_handler_function(lv_event_t* e) {
    const char* user_data = lv_event_get_user_data(e);
    int value = atoi(user_data);

    // Handle event
    LV_LOG_USER("Button clicked with value: %d", value);
}
```

#### Subject Manipulation Events

**Set Subject Value:**
```xml
<lv_button>
    <subject_set_int_event trigger="clicked" subject="counter" value="0"/>
</lv_button>
```

**Increment/Decrement Subject:**
```xml
<lv_button>
    <lv_label text="+"/>
    <subject_increment_event trigger="clicked" subject="volume" step="1" max="100"/>
</lv_button>

<lv_button>
    <lv_label text="-"/>
    <subject_increment_event trigger="clicked" subject="volume" step="-1" min="0"/>
</lv_button>
```

#### Screen Navigation Events

**Load Existing Screen:**
```xml
<lv_button>
    <screen_load_event trigger="clicked" screen="settings_screen"
                       anim_type="fade_in" duration="300"/>
</lv_button>
```

**Create Screen Dynamically:**
```xml
<lv_button>
    <screen_create_event trigger="clicked" screen="dialog_screen"/>
</lv_button>
```

---

## Implementation Guide

### Step-by-Step Pattern

#### Step 1: Define XML Layout

Create component in `ui_xml/component_name.xml`:

```xml
<component>
    <view extends="lv_obj" width="100%" height="100%" style_bg_color="#panel_bg">
        <!-- Status label bound to subject -->
        <lv_label bind_text="status_message" style_text_font="montserrat_20"/>

        <!-- Temperature label bound to subject -->
        <lv_label bind_text="temp_value" style_text_font="montserrat_28"/>
    </view>
</component>
```

#### Step 2: Create C++ Wrapper

**Header** (`include/ui_component_name.h`):

```cpp
#pragma once
#include "lvgl/lvgl.h"

// Initialize subjects (call before creating XML)
void ui_component_name_init_subjects();

// Create component with reactive data binding
lv_obj_t* ui_component_name_create(lv_obj_t* parent);

// Update component state
void ui_component_name_update(const char* status, int temp);
```

**Implementation** (`src/ui_component_name.cpp`):

```cpp
#include "ui_component_name.h"
#include <cstdio>
#include <cstring>

static lv_obj_t* component = nullptr;

// Subjects for reactive binding
static lv_subject_t status_subject;
static lv_subject_t temp_subject;
static char status_buffer[128];
static char temp_buffer[32];

void ui_component_name_init_subjects() {
    // Initialize subjects with default values
    lv_subject_init_string(&status_subject, status_buffer, NULL,
                           sizeof(status_buffer), "Initial status");
    lv_subject_init_string(&temp_subject, temp_buffer, NULL,
                           sizeof(temp_buffer), "25 °C");

    // Register subjects globally (NULL = global scope)
    lv_xml_register_subject(NULL, "status_message", &status_subject);
    lv_xml_register_subject(NULL, "temp_value", &temp_subject);
}

lv_obj_t* ui_component_name_create(lv_obj_t* parent) {
    // Create XML component (automatically binds to registered subjects)
    component = (lv_obj_t*)lv_xml_create(parent, "component_name", nullptr);
    return component;
}

void ui_component_name_update(const char* status, int temp) {
    // Update subjects - all bound widgets update automatically
    if (status) {
        lv_subject_copy_string(&status_subject, status);
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%d °C", temp);
    lv_subject_copy_string(&temp_subject, buf);
}
```

#### Step 3: Register and Use

In `main.cpp`:

```cpp
// 1. Register fonts, images (from ui_fonts.h, ui_images.h)
lv_xml_register_font(NULL, "montserrat_20", &lv_font_montserrat_20);

// 2. Register XML components (globals FIRST)
lv_xml_component_register_from_file("A:/path/to/ui_xml/globals.xml");
lv_xml_component_register_from_file("A:/path/to/ui_xml/component_name.xml");

// 3. Initialize subjects (BEFORE creating XML)
ui_component_name_init_subjects();

// 4. Create component (bindings happen automatically)
lv_obj_t* panel = ui_component_name_create(screen);

// 5. Update component state (triggers reactive updates)
ui_component_name_update("Printing...", 210);
```

### Working Example: Home Panel

**Files:**
- `ui_xml/home_panel.xml` - Layout with cards, status text, printer image
- `src/ui_panel_home.cpp` - Subject management and update API
- `include/ui_panel_home.h` - Public interface

**Subjects:**
- `status_text` - Status message (string)
- `temp_text` - Temperature display (string)

**XML Bindings:**
```xml
<lv_label bind_text="status_text" style_text_color="#text_primary"/>
<lv_label bind_text="temp_text" style_text_font="montserrat_16"/>
```

**Update API:**
```cpp
ui_panel_home_init_subjects();           // Call once during startup
ui_panel_home_update("Ready", 25);       // Updates both labels instantly
```

---

## Best Practices

### Development Guidelines

1. **One subject per updatable value** - Don't share subjects across unrelated widgets
2. **Initialize → Register → Create** - Follow strict ordering for subjects
3. **Use const references** - Pass string data efficiently to update functions
4. **Descriptive subject names** - `"printer_status"` not `"data1"`
5. **Component-local wrappers** - Each XML component gets its own C++ wrapper
6. **Name-based widget lookup** - Use `lv_obj_find_by_name()` instead of child indices
7. **Global constants** - Define all dimensions, colors, and sizes in `globals.xml`
8. **Avoid magic numbers** - Use named constants like `#padding_normal` instead of `"20"`

### Widget Lookup: ALWAYS Use Names

**✓ CORRECT - Name-based lookup (resilient to layout changes):**

```xml
<lv_label name="temperature_display" bind_text="temp_text"/>
```

```cpp
lv_obj_t* widget = lv_obj_find_by_name(parent, "temperature_display");
```

**✗ WRONG - Index-based lookup (fragile, breaks when layout changes):**

```cpp
lv_obj_t* widget = lv_obj_get_child(parent, 3);  // DON'T DO THIS
```

**Benefits:**
- Layout changes (reordering, adding/removing widgets) won't break code
- Self-documenting - widget name shows intent
- Works seamlessly with XML `name` attributes

### Subject Management

**String subject buffers must be persistent:**

```cpp
// ✓ CORRECT - Static or heap-allocated
static char status_buffer[128];
lv_subject_init_string(&subject, status_buffer, NULL, sizeof(status_buffer), "Initial");

// ✗ WRONG - Stack-allocated (will be destroyed)
char buffer[128];
lv_subject_init_string(&subject, buffer, ...);
```

### Theme System

**Centralize all UI constants:**

```xml
<!-- In globals.xml -->
<consts>
    <color name="primary_color" value="0xff4444"/>
    <px name="padding_normal" value="20"/>
    <px name="accent_bar_width" value="4"/>
</consts>
```

**Benefits:**
- Single source of truth for colors and dimensions
- Easy to adjust theme project-wide
- Consistency across all components

### Auto-Generated Icon Constants

FontAwesome icons are auto-generated to avoid UTF-8 encoding issues:

```bash
# Regenerate after adding icons to ui_fonts.h
python3 scripts/generate-icon-consts.py
```

The script reads icon definitions from `include/ui_fonts.h` and writes UTF-8 byte sequences to `globals.xml`.

**Usage in XML:**
```xml
<lv_label text="#icon_home" style_text_font="fa_icons_64"/>
```

---

## Troubleshooting

### Common Issues

#### "No constant was found with name X"

**Cause:** Constant not defined in globals.xml or not registered before component creation.

**Fix:** Ensure `globals.xml` is registered first:
```cpp
lv_xml_component_register_from_file(".../globals.xml");  // FIRST
lv_xml_component_register_from_file(".../other.xml");    // Then others
```

#### Labels not updating when subject changes

**Cause:** Subject not registered before XML creation, or wrong subject name.

**Fix:**
1. Check registration order (initialize → register → create XML)
2. Verify `bind_text="subject_name"` matches registered name
3. Add debug printf() in update function to verify calls

#### Centering not working

**Cause:** Using `flex_align` instead of `style_flex_*` properties.

**Fix:**
```xml
<!-- ✗ DOESN'T WORK -->
<lv_obj flex_align="center center center">

<!-- ✓ WORKS -->
<lv_obj style_flex_main_place="center" style_flex_cross_place="center">
```

#### Vertical centering fails

**Cause:** Container missing `height="100%"`.

**Fix:**
```xml
<lv_obj flex_flow="column" height="100%"
        style_flex_main_place="center"
        style_flex_cross_place="center">
```

#### Icons not centered in buttons

**Cause:** Missing `align="center"` on label.

**Fix:**
```xml
<lv_button>
    <lv_label align="center" text="#icon_home"/>
</lv_button>
```

#### Flex items not distributing evenly

**Cause:** Not using `flex_grow` on children.

**Fix:**
```xml
<lv_obj flex_flow="row">
    <lv_obj flex_grow="1">Item 1</lv_obj>
    <lv_obj flex_grow="1">Item 2</lv_obj>
    <lv_obj flex_grow="1">Item 3</lv_obj>
</lv_obj>
```

### Debugging Checklist

When layouts aren't working:

- [ ] Does the label have `style_text_align="center"` **AND** `width="100%"`?
- [ ] Does the parent container have `flex_flow` set correctly (row/column)?
- [ ] Does the parent use `style_flex_main_place` and `style_flex_cross_place` (NOT `flex_align`)?
- [ ] Are children using `flex_grow="1"` for equal space distribution?
- [ ] Does the container have `height="100%"` (required for vertical centering)?
- [ ] Have you added debug background colors to visualize actual container sizes?
- [ ] Are you avoiding mixing absolute positioning with flex layouts?
- [ ] Have you verified no `flex_align` attributes remain (they're silently ignored)?

### Visual Debugging

Unlike web browsers, there's no inspect tool. Use temporary background colors:

```xml
<lv_obj style_bg_color="#ff0000" style_bg_opa="100%">
    <!-- Check if this container has the expected size -->
</lv_obj>
```

### What DOESN'T Work in LVGL 9 XML

❌ **`flex_align` attribute** - Silently ignored, use `style_flex_*` instead
❌ **`style_flex_grow`** - Doesn't exist, use `flex_grow` attribute instead
❌ **Mixing explicit percentage widths with `flex_grow`** - Use one or the other
❌ **Stack-allocated string buffers for subjects** - Must be static or heap-allocated
❌ **Creating XML before registering subjects** - Bindings will fail silently

---

## Resources

### Official Documentation

- **LVGL XML Docs:** https://docs.lvgl.io/master/details/xml/
- **Subject-Observer:** https://docs.lvgl.io/master/details/auxiliary-modules/observer/observer.html
- **LVGL Events:** https://docs.lvgl.io/latest/en/html/details/xml/ui_elements/events.html
- **LVGL Editor Tutorials:** https://github.com/lvgl/lvgl_editor/tree/master/tutorials

### Project Examples

- **Navigation bar:** `ui_xml/navigation_bar.xml`
- **Home panel:** `ui_xml/home_panel.xml` + `src/ui_panel_home.cpp`
- **App layout:** `ui_xml/app_layout.xml`
- **Global constants:** `ui_xml/globals.xml`

### Related Project Documentation

- **CLAUDE.md** - Claude Code context for this project
- **STATUS.md** - Development journal with daily updates
- **README.md** - Project overview and quick start
- **ROADMAP.md** - Planned features and milestones

---

## Document History

**2025-01-11:** Initial consolidated guide combining:
- LVGL_XML_REFERENCE.md - XML syntax reference
- LVGL9_CENTERING_GUIDE.md - Centering techniques and flex_align discovery
- XML_UI_SYSTEM.md - Architecture and implementation patterns
- Vertical Accent Bar Pattern - New UI pattern documentation

**Contributors:** GuppyScreen development team, based on LVGL 9.3 documentation and extensive prototype testing.
