# LVGL 9 Style Elements - Complete Reference

**Complete documentation for `<style>` elements in LVGL 9 XML**

**Last Updated:** 2025-01-14
**LVGL Version:** 9.3
**Source:** `lvgl/src/others/xml/lv_xml_style.c`

---

## Table of Contents

1. [Overview](#overview)
2. [Style Element Syntax](#style-element-syntax)
3. [Special Attributes](#special-attributes)
4. [All Style Properties](#all-style-properties)
5. [Applying Styles to Widgets](#applying-styles-to-widgets)
6. [Advanced Features](#advanced-features)
7. [Examples](#examples)
8. [Source Reference](#source-reference)

---

## Overview

### What are Style Elements?

Style elements (`<style>`) are defined in `<styles>` sections within XML components. They create reusable style objects that can be applied to multiple widgets.

### Key Concept

**Style element attributes = Widget `style_*` attributes - "style_" prefix**

```xml
<!-- Define a style -->
<styles>
    <style name="my_button" bg_color="0x2196f3" radius="8"/>
</styles>

<!-- Apply to widget -->
<lv_button>
    <style name="my_button"/>
</lv_button>

<!-- Equivalent to inline styling -->
<lv_button style_bg_color="0x2196f3" style_radius="8"/>
```

### Where to Define Styles

**Global styles** (in `globals.xml` or project-level):
```xml
<component>
    <styles>
        <style name="style_button" bg_color="0x111" radius="8"/>
    </styles>
    <view extends="lv_obj"/>
</component>
```

**Local styles** (in component files):
```xml
<component>
    <styles>
        <style name="style_local" bg_color="0x333"/>
    </styles>
    <view extends="lv_obj">
        <!-- Use the local style -->
    </view>
</component>
```

---

## Style Element Syntax

### Basic Definition

```xml
<style name="style_name"
       property1="value1"
       property2="value2"
       property3="value3"/>
```

### Multiple Styles in Section

```xml
<styles>
    <style name="style_button" bg_color="0x2196f3" radius="8"/>
    <style name="style_card" bg_color="0x202020" shadow_width="10"/>
    <style name="style_text" text_color="0xffffff" text_font="montserrat_16"/>
</styles>
```

---

## Special Attributes

These attributes control style metadata and are not visual properties.

| Attribute | Type | Required | Description |
|-----------|------|----------|-------------|
| `name` | string | **YES** | Style name for reference. Must be unique within scope. |
| `help` | string | No | Help text or documentation (ignored by parser) |
| `figma_node_id` | string | No | Figma design node ID for tracking (ignored by parser) |

**Source:** `lv_xml_style.c:117-119`

### Example

```xml
<style name="primary_button"
       help="Main call-to-action button style"
       figma_node_id="42:1337"
       bg_color="0x2196f3"
       radius="8"/>
```

---

## All Style Properties

**Source:** `lv_xml_style.c:168-293`

Every property uses the **same name as widget `style_*` attributes, but WITHOUT the `style_` prefix**.

### Size & Dimensions

| Property | Type | Description |
|----------|------|-------------|
| `width` | size | Width (px, %, or "content") |
| `min_width` | size | Minimum width |
| `max_width` | size | Maximum width |
| `height` | size | Height (px, %, or "content") |
| `min_height` | size | Minimum height |
| `max_height` | size | Maximum height |
| `length` | size | Generic length (widget-specific) |
| `radius` | size | Corner radius |

**Lines:** 168-175

### Padding

| Property | Type | Description |
|----------|------|-------------|
| `pad_left` | int | Left padding |
| `pad_right` | int | Right padding |
| `pad_top` | int | Top padding |
| `pad_bottom` | int | Bottom padding |
| `pad_hor` | int | Horizontal padding (left + right) |
| `pad_ver` | int | Vertical padding (top + bottom) |
| `pad_all` | int | All sides padding |
| `pad_row` | int | Padding between rows (flex/grid) |
| `pad_column` | int | Padding between columns (flex/grid) |
| `pad_gap` | int | Gap between flex/grid items |
| `pad_radial` | int | Radial padding (for arcs) |

**Lines:** 177-187

### Margin

| Property | Type | Description |
|----------|------|-------------|
| `margin_left` | int | Left margin |
| `margin_right` | int | Right margin |
| `margin_top` | int | Top margin |
| `margin_bottom` | int | Bottom margin |
| `margin_hor` | int | Horizontal margin (left + right) |
| `margin_ver` | int | Vertical margin (top + bottom) |
| `margin_all` | int | All sides margin |

**Lines:** 189-195

### Layout Control

| Property | Type | Description |
|----------|------|-------------|
| `base_dir` | enum | Base text direction (`auto`, `ltr`, `rtl`) |
| `clip_corner` | bool | Clip content at rounded corners |
| `layout` | enum | Layout type (`none`, `flex`, `grid`) |

**Lines:** 197-198, 277

### Background

| Property | Type | Description |
|----------|------|-------------|
| `bg_opa` | opacity | Background opacity (0-255 or %) |
| `bg_color` | color | Background color (hex: `0xRRGGBB`) |
| `bg_grad_dir` | enum | Gradient direction (`none`, `hor`, `ver`) |
| `bg_grad_color` | color | Gradient end color |
| `bg_main_stop` | int | Gradient main color stop (0-255) |
| `bg_grad_stop` | int | Gradient end color stop (0-255) |
| `bg_grad` | gradient | Gradient descriptor reference |
| `bg_image_src` | image | Background image source |
| `bg_image_tiled` | bool | Tile background image |
| `bg_image_recolor` | color | Background image recolor tint |
| `bg_image_recolor_opa` | opacity | Background image recolor opacity |

**Lines:** 200-211

### Border

| Property | Type | Description |
|----------|------|-------------|
| `border_color` | color | Border color |
| `border_width` | int | Border width |
| `border_opa` | opacity | Border opacity |
| `border_side` | enum | Border sides (`none`, `top`, `bottom`, `left`, `right`, `full`) |
| `border_post` | bool | Draw border after children |

**Lines:** 213-217

### Outline

| Property | Type | Description |
|----------|------|-------------|
| `outline_color` | color | Outline color |
| `outline_width` | int | Outline width |
| `outline_opa` | opacity | Outline opacity |
| `outline_pad` | int | Space between widget and outline |

**Lines:** 219-222

### Shadow

| Property | Type | Description |
|----------|------|-------------|
| `shadow_width` | int | Shadow blur width |
| `shadow_color` | color | Shadow color |
| `shadow_offset_x` | int | Shadow horizontal offset |
| `shadow_offset_y` | int | Shadow vertical offset |
| `shadow_spread` | int | Shadow spread amount |
| `shadow_opa` | opacity | Shadow opacity |

**Lines:** 224-229

### Text

| Property | Type | Description |
|----------|------|-------------|
| `text_color` | color | Text color |
| `text_font` | font | Font reference |
| `text_opa` | opacity | Text opacity |
| `text_align` | enum | Text alignment (`left`, `right`, `center`, `auto`) |
| `text_letter_space` | int | Letter spacing |
| `text_line_space` | int | Line spacing |
| `text_decor` | enum | Text decoration (`none`, `underline`, `strikethrough`) |

**Lines:** 231-237

### Image

| Property | Type | Description |
|----------|------|-------------|
| `image_opa` | opacity | Image opacity |
| `image_recolor` | color | Image recolor tint |
| `image_recolor_opa` | opacity | Image recolor opacity |

**Lines:** 239-241

**⚠️ CRITICAL:** Must use `image` not `img`. See [LVGL9_XML_ATTRIBUTES_REFERENCE.md](LVGL9_XML_ATTRIBUTES_REFERENCE.md#critical-gotchas).

### Line

| Property | Type | Description |
|----------|------|-------------|
| `line_color` | color | Line color |
| `line_opa` | opacity | Line opacity |
| `line_width` | int | Line width |
| `line_dash_width` | int | Dash width for dashed lines |
| `line_dash_gap` | int | Gap between dashes |
| `line_rounded` | bool | Round line ends |

**Lines:** 243-248

### Arc

| Property | Type | Description |
|----------|------|-------------|
| `arc_color` | color | Arc color |
| `arc_opa` | opacity | Arc opacity |
| `arc_width` | int | Arc width |
| `arc_rounded` | bool | Round arc ends |
| `arc_image_src` | image | Arc image source |

**Lines:** 250-254

### Opacity & Effects

| Property | Type | Description |
|----------|------|-------------|
| `opa` | opacity | Overall widget opacity |
| `opa_layered` | opacity | Layered opacity |
| `color_filter_opa` | opacity | Color filter opacity |
| `anim_duration` | int | Animation duration (milliseconds) |
| `blend_mode` | enum | Blend mode (`normal`, `additive`, `subtractive`, `multiply`, `difference`) |
| `recolor` | color | General recolor tint |
| `recolor_opa` | opacity | General recolor opacity |

**Lines:** 256-260, 274-275

### Transforms

| Property | Type | Description |
|----------|------|-------------|
| `transform_width` | int | Transform width offset |
| `transform_height` | int | Transform height offset |
| `translate_x` | int | Horizontal translation |
| `translate_y` | int | Vertical translation |
| `translate_radial` | int | Radial translation |
| `transform_scale_x` | int | Horizontal scale (256 = 100%) |
| `transform_scale_y` | int | Vertical scale (256 = 100%) |
| `transform_rotation` | int | Rotation angle (0.1 degree units) |
| `transform_pivot_x` | int | Rotation pivot X |
| `transform_pivot_y` | int | Rotation pivot Y |
| `transform_skew_x` | int | Horizontal skew |
| `bitmap_mask_src` | image | Bitmap mask source |
| `rotary_sensitivity` | int | Rotary encoder sensitivity |

**Lines:** 261-273

### Flex Layout

| Property | Type | Description |
|----------|------|-------------|
| `flex_flow` | enum | Flex direction (`row`, `column`, `row_reverse`, `column_reverse`, etc.) |
| `flex_grow` | int | Flex grow factor |
| `flex_main_place` | enum | Main axis alignment (`start`, `end`, `center`, `space_between`, `space_around`, `space_evenly`) |
| `flex_cross_place` | enum | Cross axis alignment (`start`, `end`, `center`) |
| `flex_track_place` | enum | Track placement in wrap mode |

**Lines:** 279-283

### Grid Layout

| Property | Type | Description |
|----------|------|-------------|
| `grid_column_align` | enum | Column alignment (`start`, `end`, `center`, `stretch`, `space_between`, `space_around`, `space_evenly`) |
| `grid_row_align` | enum | Row alignment |
| `grid_cell_column_pos` | int | Cell column position |
| `grid_cell_column_span` | int | Cell column span |
| `grid_cell_x_align` | enum | Cell horizontal alignment |
| `grid_cell_row_pos` | int | Cell row position |
| `grid_cell_row_span` | int | Cell row span |
| `grid_cell_y_align` | enum | Cell vertical alignment |

**Lines:** 285-292

---

## Applying Styles to Widgets

### Basic Application

Use the `<style>` child element inside widget tags:

```xml
<!-- Define style -->
<styles>
    <style name="style_button" bg_color="0x2196f3" radius="8"/>
</styles>

<!-- Apply to widget -->
<lv_button>
    <style name="style_button"/>
    <lv_label text="Click Me"/>
</lv_button>
```

### State-Based Styling

Use the `selector` attribute to apply styles for specific states:

```xml
<lv_button>
    <!-- Default state -->
    <style name="style_button"/>

    <!-- Pressed state -->
    <style name="style_button_pressed" selector="pressed"/>

    <!-- Checked state -->
    <style name="style_button_checked" selector="checked"/>

    <!-- Disabled state -->
    <style name="style_button_disabled" selector="disabled"/>
</lv_button>
```

**Valid selectors:** `default`, `pressed`, `checked`, `hovered`, `scrolled`, `focused`, `focus_key`, `edited`, `disabled`

**Source:** `lv_xml_style.c:52-65`

### Part-Based Styling

Apply styles to specific widget parts:

```xml
<lv_slider>
    <!-- Main part -->
    <style name="style_slider_main" selector="main"/>

    <!-- Indicator part -->
    <style name="style_slider_indicator" selector="indicator"/>

    <!-- Knob part -->
    <style name="style_slider_knob" selector="knob"/>
</lv_slider>
```

**Valid parts:** `main`, `scrollbar`, `indicator`, `knob`, `selected`, `items`, `cursor`

**Source:** `lv_xml_style.c:67-78`

### Combining Selectors

You can combine state and part selectors with colons:

```xml
<lv_button>
    <!-- Indicator part in pressed state -->
    <style name="my_style" selector="indicator:pressed"/>
</lv_button>
```

### Multiple Styles

Apply multiple styles by separating with spaces in the `styles` attribute:

```xml
<lv_button styles="style_base style_blue">
    <lv_label text="Multi-styled Button"/>
</lv_button>
```

**Source:** `lv_xml_style.c:321-356`

---

## Advanced Features

### The "remove" Value

**Any style property can use `value="remove"` to remove that property from the style.**

**Source:** `lv_xml_style.c:132-167`

```xml
<styles>
    <!-- Base style with background -->
    <style name="style_base" bg_color="0x2196f3" pad_all="12"/>

    <!-- Extend base but remove background -->
    <style name="style_transparent" bg_color="remove"/>
</styles>
```

**Compound removals:**
- `pad_all="remove"` removes `pad_top`, `pad_bottom`, `pad_left`, `pad_right`
- `pad_hor="remove"` removes `pad_left`, `pad_right`
- `pad_ver="remove"` removes `pad_top`, `pad_bottom`
- `pad_gap="remove"` removes `pad_column`, `pad_row`
- `margin_all="remove"` removes all margin properties
- `margin_hor="remove"` removes `margin_left`, `margin_right`
- `margin_ver="remove"` removes `margin_top`, `margin_bottom`

### Style Extension

If a style with the same name already exists, new properties extend it:

**Source:** `lv_xml_style.c:93-100`

```xml
<!-- First definition -->
<styles>
    <style name="my_style" bg_color="0x2196f3"/>
</styles>

<!-- Later in same scope - extends the style -->
<styles>
    <style name="my_style" radius="8" pad_all="12"/>
</styles>

<!-- Result: my_style has bg_color, radius, AND pad_all -->
```

### Constant References

Style properties can reference global constants using `#` prefix:

```xml
<!-- Define constants -->
<consts>
    <color name="primary_color" value="0x2196f3"/>
    <px name="button_radius" value="8"/>
</consts>

<!-- Use in styles -->
<styles>
    <style name="style_button"
           bg_color="#primary_color"
           radius="#button_radius"/>
</styles>
```

**Source:** `lv_xml_style.c:121-130`

### Component Scoping

Styles can be scoped to specific components using dot notation:

```xml
<!-- Reference style from specific component -->
<lv_button>
    <style name="my_component.style_button"/>
</lv_button>
```

**Source:** `lv_xml_style.c:358-398`

If a style is not found in the current component scope, the parser automatically falls back to the `globals` scope.

---

## Examples

### Complete Button Style Set

```xml
<styles>
    <!-- Base button style -->
    <style name="btn_base"
           bg_color="0x2196f3"
           text_color="0xffffff"
           radius="8"
           pad_all="12"
           border_width="0"
           text_font="montserrat_16"/>

    <!-- Pressed state -->
    <style name="btn_pressed"
           bg_color="0x1976d2"/>

    <!-- Disabled state -->
    <style name="btn_disabled"
           bg_color="0x666666"
           text_color="0x999999"/>
</styles>

<!-- Apply to button -->
<lv_button>
    <style name="btn_base"/>
    <style name="btn_pressed" selector="pressed"/>
    <style name="btn_disabled" selector="disabled"/>
    <lv_label text="Click Me" align="center"/>
</lv_button>
```

### Card with Shadow

```xml
<styles>
    <style name="card"
           bg_color="0x202020"
           radius="12"
           shadow_width="20"
           shadow_color="0x000000"
           shadow_opa="80"
           shadow_offset_x="0"
           shadow_offset_y="4"
           shadow_spread="0"
           pad_all="20"
           border_width="0"/>
</styles>

<lv_obj>
    <style name="card"/>
    <!-- Card content -->
</lv_obj>
```

### Flex Container Style

```xml
<styles>
    <style name="flex_row_center"
           flex_flow="row"
           flex_main_place="center"
           flex_cross_place="center"
           pad_gap="12"
           bg_opa="0"
           border_width="0"/>
</styles>

<lv_obj>
    <style name="flex_row_center"/>
    <lv_button/>
    <lv_button/>
    <lv_button/>
</lv_obj>
```

### Gradient Background

```xml
<styles>
    <style name="gradient_bg"
           bg_color="0x2196f3"
           bg_grad_dir="ver"
           bg_grad_color="0x1976d2"
           bg_main_stop="0"
           bg_grad_stop="255"/>
</styles>

<lv_obj width="300" height="200">
    <style name="gradient_bg"/>
</lv_obj>
```

### Transparent Overlay

```xml
<styles>
    <style name="overlay"
           bg_color="0x000000"
           bg_opa="50%"
           radius="0"
           border_width="0"/>
</styles>

<lv_obj width="100%" height="100%">
    <style name="overlay"/>
    <!-- Modal content -->
</lv_obj>
```

### Custom Slider

```xml
<styles>
    <!-- Main track -->
    <style name="slider_main"
           bg_color="0x333333"
           radius="4"/>

    <!-- Indicator (filled portion) -->
    <style name="slider_indicator"
           bg_color="0x2196f3"
           radius="4"/>

    <!-- Knob (draggable handle) -->
    <style name="slider_knob"
           bg_color="0xffffff"
           radius="50%"
           border_color="0x2196f3"
           border_width="3"
           shadow_width="10"
           shadow_opa="50"/>
</styles>

<lv_slider value="50" range="0 100">
    <style name="slider_main" selector="main"/>
    <style name="slider_indicator" selector="indicator"/>
    <style name="slider_knob" selector="knob"/>
</lv_slider>
```

---

## Source Reference

**Primary Source File:** `lvgl/src/others/xml/lv_xml_style.c`

### Key Functions

| Function | Lines | Description |
|----------|-------|-------------|
| `lv_xml_style_register()` | 80-301 | Registers style from XML attributes |
| `lv_xml_style_add_to_obj()` | 321-356 | Applies style to widget |
| `lv_xml_get_style_by_name()` | 358-399 | Looks up style by name |
| `lv_xml_style_state_to_enum()` | 52-65 | Parses state selectors |
| `lv_xml_style_part_to_enum()` | 67-78 | Parses part selectors |
| `style_prop_text_to_enum()` | 416-539 | Maps property names to enums |

### Property Definitions

All 100+ style properties are handled in `lv_xml_style_register()`:
- **Lines 168-175:** Size & dimensions
- **Lines 177-187:** Padding
- **Lines 189-195:** Margin
- **Lines 197-198:** Layout control
- **Lines 200-211:** Background
- **Lines 213-217:** Border
- **Lines 219-222:** Outline
- **Lines 224-229:** Shadow
- **Lines 231-237:** Text
- **Lines 239-241:** Image
- **Lines 243-248:** Line
- **Lines 250-254:** Arc
- **Lines 256-260:** Opacity & effects
- **Lines 261-273:** Transforms
- **Lines 274-275:** Recolor
- **Lines 277:** Layout
- **Lines 279-283:** Flex layout
- **Lines 285-292:** Grid layout

### Related Documentation

- **[LVGL9_XML_ATTRIBUTES_REFERENCE.md](LVGL9_XML_ATTRIBUTES_REFERENCE.md)** - Complete widget attribute reference
- **[LVGL9_XML_GUIDE.md](LVGL9_XML_GUIDE.md)** - General XML UI system guide
- **LVGL Official Docs:** https://docs.lvgl.io/master/details/xml/

---

**Document Version:** 1.0
**LVGL Version:** 9.3
**Last Verified:** 2025-01-14
