# Project Status - LVGL 9 UI Prototype

**Last Updated:** 2025-10-13 (Motion Panel XYZ Controls COMPLETE)

## Current State

✅ **Motion Panel with 8-Direction Jog Pad and Z-Axis Controls COMPLETE (Phase 5.3 finished)**
✅ **Numeric Keypad Modal Component COMPLETE (Phase 5.2 finished)**
✅ **Controls Panel Launcher with 6-Card Menu COMPLETE (Phase 5.1 finished)**
✅ **Print Select Panel with Dual Views, Sorting, Empty State, Confirmation Dialogs COMPLETE (Phase 4 finished)**

### What Works

- **6 Panel Navigation** - Click icons to switch between Home, Controls, Filament, Settings, Advanced, and Print Select panels
- **Command-Line Panel Selection** - Launch directly to any panel via CLI argument for testing
- **Reactive Icon Colors** - Active panel icon shows in red (#ff4444), inactive in white (#ffffff)
- **Print Select Panel with Cards** - Dynamic XML-based file cards in **4-column grid layout** (1024×800 display)
  - Card dimensions: 204×280px (increased to fit metadata without scrollbar)
  - Thumbnail: 180×180px centered images
  - Responsive constants in globals.xml (future: 3-col for smaller displays)
- **File Card Component** - Reusable XML component (print_file_card.xml) with API properties
  - Thumbnail display with centering
  - Filename truncation with ellipsis (single line, 204px width)
  - **Metadata row with icons** - Clock (print time) + Leaf (filament weight)
  - **Icon + text layout** - Red accent icons with gray text per design spec
- **Mock Data System** - 30-file generator with varied print times, filament weights, and filenames
- **Utility Functions** - Time/filament formatting (ui_utils.cpp) for clean display strings
- **Home Panel Content** - Temperature display, network status (WiFi icon), light control button (Bambu X1C-inspired)
- **Hybrid Icon Support** - Mix FontAwesome fonts (fa_icons_64, fa_icons_48, fa_icons_32, fa_icons_16) and custom PNG images
- **Subject-Observer Pattern** - Automatic UI updates via LVGL 9's reactive data binding
- **XML-Based UI** - Complete layout defined declaratively in XML files
- **FontAwesome Icons** - Multiple sizes (64px, 48px, 32px, 16px) with auto-generated UTF-8 constants
- **Custom Icons** - SVG-to-PNG conversion with `style_img_recolor` support
- **Theme System** - Global color/size constants in `globals.xml`
- **Screenshot Utility** - Automated 2-second capture to `/tmp` with panel argument support
- **Testing Framework** - Catch2-based unit tests with `make test` integration
- **Clean Architecture** - Minimal C++ code, mostly initialization and reactive updates
- **Comprehensive Documentation** - HANDOFF.md, ROADMAP.md, STATUS.md, LVGL9_XML_GUIDE.md with LV_SIZE_CONTENT troubleshooting

- **Controls Panel Launcher** - 6-card menu (400×200px cards) in 2×3 scrollable grid
  - Card-based navigation to sub-screens (Motion, Temps, Extrusion, etc.)
  - Click handlers wired and ready for sub-screen implementation
  - Clean card design with icons, titles, and subtitles
  - Proper flex wrapping (row_wrap) and vertical scrolling

- **Numeric Keypad Modal Component** - Reusable integer input widget (700px wide, right-docked)
  - Full-screen semi-transparent backdrop with click-to-cancel
  - Large input display (montserrat_48 font) with unit labels
  - 3×4 button grid (0-9 + backspace) with proper centering
  - Header bar with back button, dynamic title, and OK button
  - Callback-based API with min/max validation
  - FontAwesome backspace icon (fa_icons_32) in orange accent color
  - Single reusable instance pattern (create once, reconfigure on show)

- **Motion Panel (Sub-Screen)** - Complete XYZ movement controls
  - 8-direction jog pad: ↖ ↑ ↗, ← ⊙ →, ↙ ↓ ↘ (3×3 grid)
  - Custom bold arrow font (diagonal_arrows_40) with Unicode glyphs
  - Z-axis controls: +10mm, +1mm, -1mm, -10mm buttons
  - Distance selector: 0.1mm, 1mm, 10mm, 100mm radio buttons
  - Position display card with reactive X/Y/Z coordinates
  - Home buttons: All, X, Y, Z axes
  - Mock position simulation for testing
  - Back button returns to Controls launcher

### Active Development

**Current Focus:** Phase 5.4 - Temperature Sub-Screens (Nozzle + Bed control)

**Completed Today (2025-10-13):**
- ✅ Created motion panel XML with 3×3 jog pad grid (motion_panel.xml)
- ✅ Generated custom diagonal_arrows_40 font with Unicode arrows (←↑→↓↖↗↙↘)
- ✅ Updated generate-icon-consts.py to use Unicode arrow codepoints (U+2190-2193, U+2196-2199)
- ✅ Fixed Z-axis button event handlers by removing unnecessary container layers
- ✅ Implemented C++ motion panel wrapper (ui_panel_motion.cpp/h)
- ✅ Added reactive position display with X/Y/Z subject bindings
- ✅ Wired all jog pad buttons (8 directions + center home)
- ✅ Wired distance selector buttons with visual feedback
- ✅ Wired home buttons (All, X, Y, Z)
- ✅ Implemented mock position simulation (X/Y jog moves, Z buttons increment/decrement)
- ✅ Added motion panel dimensions to globals.xml
- ✅ Integrated motion panel into Controls launcher

**Arrow Icon Evolution:**
1. Initial attempt: Tried using FontAwesome arrows (missing diagonal glyphs in Free version)
2. Second attempt: Tried Montserrat font (Unicode arrows not included)
3. Final solution: Generated custom font from Arial Unicode MS with bold 40px arrows
4. Result: Consistent bold styling across all 8 directions

**Button Layout Fix:**
- Z-axis buttons originally had nested `lv_obj` containers for icon+text layout
- Containers captured click events, preventing button callbacks from firing
- Solution: Applied flex layout directly to `lv_button` widgets, removed container layer
- Pattern now matches jog pad buttons (labels directly inside buttons)

**Completed Earlier (2025-10-12 Late Night):**
- ✅ Created numeric keypad modal XML component (numeric_keypad_modal.xml)
- ✅ Implemented C++ wrapper with callback API (ui_component_keypad.cpp/h)
- ✅ Added keypad dimension constants to globals.xml (700px width, 140×100px buttons)
- ✅ Regenerated fa_icons_32 font to include backspace icon (U+F55A)
- ✅ Added backspace icon to font generation pipeline (package.json)
- ✅ Installed lv_font_conv npm package for font regeneration
- ✅ Wired all button event handlers (digits, backspace, OK, cancel)
- ✅ Implemented input state management with string buffer
- ✅ Added dynamic title and unit label support
- ✅ Integrated keypad into main app initialization
- ✅ Tested with interactive demo (launches on startup)

**Font Generation Workflow Established:**
1. Update icon ranges in package.json (convert-font-32, convert-font-64, etc.)
2. Run `npm run convert-font-XX` to regenerate font C files
3. Run `python3 scripts/generate-icon-consts.py` to update globals.xml
4. Rebuild binary with `make`

**Completed Earlier (2025-10-12 Night):**
- ✅ Created comprehensive 70-page Controls Panel UI design specification
- ✅ Implemented 6-card launcher panel (Movement, Nozzle Temp, Bed Temp, Extrusion, Fan, Motors)
- ✅ Added new FontAwesome icons to ui_fonts.h (motion, temperature, extrusion icons)
- ✅ Regenerated icon constants with generate-icon-consts.py (27 total icons)
- ✅ Fixed card layout: proper flex row_wrap for 2×3 grid
- ✅ Adjusted card dimensions: 400×200px (fits 2 columns with 20px gaps)
- ✅ Created C++ panel integration (ui_panel_controls.cpp/h)
- ✅ Wired click event handlers for all 6 cards
- ✅ Integrated Controls Panel into main navigation system
- ✅ Fan Control card styled as "Coming soon" placeholder (dimmed)
- ✅ All cards render cleanly without scrollbars

**Design Decisions:**
- Card-based launcher menu (Bambu X1C style) instead of inline controls
- Each card opens a dedicated sub-screen for focused control
- Reusable numeric keypad modal for temperature input
- Motion sub-screen will use button grid (simplified from circular jog pad)
- Used existing fa_icons_64 glyphs (arrows, home, settings) as placeholders

**Next Steps:**
- Phase 3: Implement Motion sub-screen with directional jog buttons
- Phase 4: Implement Temperature sub-screens (wire keypad to Nozzle + Bed cards)
- Phase 5: Implement Extrusion sub-screen with load/unload controls
- Phase 6: Wire all controls to mock printer state (simulate API calls)

**Completed Earlier (2025-10-12 Evening):**
- ✅ Icon-only view toggle button (40×40px, fa-list ↔ fa-th-large)
- ✅ Dual view modes: Card view (grid) + List view (sortable table)
- ✅ List view with 4-column layout: Filename | Size | Modified | Time
- ✅ Column sorting with visual indicators (▲/▼ arrows)
- ✅ Empty state message ("No files available for printing")
- ✅ Reusable confirmation dialog component (confirmation_dialog.xml)
- ✅ Utility functions: format_file_size(), format_modified_date()
- ✅ Updated all documentation (requirements v2.0, HANDOFF.md, STATUS.md)
- ✅ Added ICON_LIST and ICON_TH_LARGE to icon system

**Design Decisions:**
- Simplified list view from 7 columns to 4 (removed Thumbnail, Filament, Slicer)
- View toggle shows opposite mode icon (list icon in card mode, card icon in list mode)
- Default view: Card mode (user preference remembered per session)
- Default sort: Filename ascending

**Next Phase: Phase 5 - Remaining Panels OR Moonraker Integration**
- Option A: Build out Controls, Filament, Settings, Advanced panels
- Option B: Wire Print/Delete buttons to Moonraker API
- Future: Tab bar for storage sources, search/filter, hover effects

## Recent Achievements (2025-10-12 Evening)

### ✅ Print Select Panel Metadata Labels Fixed

**Root Cause Identified:**
- Metadata labels were created but invisible due to **LV_SIZE_CONTENT width bug**
- LVGL's auto-sizing calculations fail for labels inside XML components with property substitution
- Labels rendered with zero width despite having content

**Solution Implemented:**
- Replaced `width="LV_SIZE_CONTENT"` with explicit pixel dimensions
- Time labels: `width="65"` (fits "2h30m" format)
- Weight labels: `width="55"` (fits "120g" format)
- All labels now render correctly with proper spacing

**Enhancements Added:**
- **fa_icons_16 font** - Created 16px FontAwesome font for metadata icons
- **Clock icon** - Red accent (`#primary_color`) for print time
- **Leaf icon** - Red accent (`#primary_color`) for filament weight
- **Icon + text layout** - 4px gap between icon and text, 8px gap between metadata items
- **Color scheme** - Red accent icons with gray text (`#text_secondary`) per design spec

**Infrastructure Improvements:**
- **Command-line panel selection** - `./build/bin/guppy-ui-proto print-select` launches directly to Print Select panel
- **Updated screenshot.sh** - Added panel argument support: `./scripts/screenshot.sh guppy-ui-proto output print-select`
- **String lifetime fix** - Used `lv_strdup()` to create persistent copies of metadata strings
- **Card height adjustment** - Increased from 256px to 280px to accommodate metadata without scrollbars

**Documentation Updates:**
- **LVGL9_XML_GUIDE.md** - Added critical LV_SIZE_CONTENT warning with symptoms, root cause, and explicit dimension recommendations
- **print-select-panel-v1.md** - Updated color specifications to match implementation (red icons, gray text)

**Files Modified:**
- `ui_xml/print_file_card.xml` - Added metadata icon + text layout with explicit dimensions
- `ui_xml/globals.xml` - Increased `file_card_height` to 280px
- `src/main.cpp` - Added argc/argv parsing for panel selection, registered fa_icons_16 font
- `src/ui_panel_print_select.cpp` - Fixed string lifetime issue with lv_strdup()
- `scripts/screenshot.sh` - Added panel argument support
- `scripts/generate-icon-consts.py` - Regenerated icons including clock and leaf
- `docs/LVGL9_XML_GUIDE.md` - Added LV_SIZE_CONTENT troubleshooting section
- `docs/requirements/print-select-panel-v1.md` - Updated color specifications

**Result:**
✅ Print Select Panel fully functional with 8 test cards displaying:
- Thumbnails (180×180px centered)
- Filenames (truncated with ellipsis)
- Print times with clock icons (red + gray text)
- Filament weights with leaf icons (red + gray text)
- No scrollbars (280px card height fits all content)

### ✅ 4-Column Layout Optimization (2025-10-12 Afternoon)

**Layout Adjustment:**
- Changed from 3-column to **4-column grid** for 1024×800 display
- Updated card dimensions in globals.xml:
  - `file_card_width`: 260px → **204px**
  - `file_card_height`: 312px → **256px**
  - `file_card_thumbnail_size`: 236px → **180px**
- Grid math: 204×4 + 20×3 (gaps) = 876px (fits in 890px available width)
- Created new 180×180px centered placeholder thumbnail
- Added responsive design comments (future: 3-col for smaller displays)

**Benefits:**
- More cards visible per screen (4 per row vs 3)
- Better use of horizontal space on medium displays
- Maintains clean grid alignment with proper spacing
- Constants-based design allows easy adjustment for different screen sizes

**Documentation Updates:**
- Created comprehensive HANDOFF.md (complete project handoff guide)
- Updated ROADMAP.md with Phase 3 Print Select Panel progress
- Updated STATUS.md with 4-column layout details
- All documentation now reflects current architecture and next steps

**Files Modified:**
- `ui_xml/globals.xml` - Card dimension constants
- `assets/images/placeholder_thumb_centered.png` - Resized to 180×180px
- `docs/HANDOFF.md` - NEW comprehensive handoff document
- `docs/ROADMAP.md` - Updated Phase 3 status and recent work
- `STATUS.md` - Updated current state and achievements

## Recent Achievements (2025-10-11 Evening)

### ✅ Print Select Panel Phase 2 - Static Structure

**Panel Implementation:**
- Created `print_select_panel.xml` with full-height scrollable grid
- Decision: Skipped tab bar for v1 (single storage source)
- Registered in navigation system as 6th panel (folder icon)
- Uses row_wrap flex layout for responsive grid

**Build System Fixes:**
- Fixed Makefile to exclude test binaries from main app
- Resolved duplicate main() symbol linker error
- Cloned LVGL 9.3 locally (parent project uses incompatible LVGL 8.3)
- Clean build successful with expected warnings only

**Infrastructure:**
- Documented dev requirements for macOS and Debian/Ubuntu
- Installed coreutils (timeout command) on macOS
- Updated HANDOFF.md with Phase 2 completion

## Recent Achievements (2025-10-11 Afternoon)

### ✅ Vertical Accent Bar Pattern & Documentation Consolidation

**UI Enhancements:**
- Implemented vertical accent bar (leading edge indicator) pattern in home panel
- Repositioned status text to right of printer image with accent bar
- Added global constants for accent bar styling: `#accent_bar_width`, `#accent_bar_height`, `#accent_bar_gap`
- Uses flex layout for responsive positioning (bar fixed width, text flex-grows)

**Technical Implementation:**
```xml
<!-- Status message with accent bar (lines 10-16 in home_panel.xml) -->
<lv_obj flex_grow="1" flex_flow="row" style_flex_cross_place="center" style_pad_gap="#accent_bar_gap">
    <!-- Vertical accent bar -->
    <lv_obj width="#accent_bar_width" height="#accent_bar_height"
            style_bg_color="#primary_color" style_radius="2"/>

    <!-- Status text (flex-grows to fill remaining width) -->
    <lv_label flex_grow="1" bind_text="status_text" style_text_align="left"/>
</lv_obj>
```

**Documentation Improvements:**
- Consolidated three separate docs into comprehensive `LVGL9_XML_GUIDE.md` (1170 lines)
  - Merged: LVGL_XML_REFERENCE.md, XML_UI_SYSTEM.md, LVGL9_CENTERING_GUIDE.md
  - Added vertical accent bar pattern documentation
  - Improved navigation with detailed table of contents
  - Comprehensive troubleshooting and best practices sections
- Updated `CLAUDE.md` references to point to new consolidated guide

**UI Review System:**
- Created automated UI screenshot review script (`scripts/review-ui-screenshot.sh`)
- Added templates for requirements and changelogs (`docs/templates/`)
- Created example home panel requirements document (`docs/requirements/home-panel-v1.md`)
- Created example home panel changelog (`docs/changelogs/home-panel-2025-01-11.md`)
- Added ui-reviewer agent for automated LVGL v9 UI verification

**Agent Infrastructure:**
- Moved refractor and widget-maker agents to project root `.claude/agents/`
- Added ui-reviewer agent with detailed UI verification capabilities
- Updated agent descriptions with tool access information

**Files Modified:**
- `ui_xml/home_panel.xml` - Added accent bar with flex layout (lines 10-16)
- `ui_xml/globals.xml` - Added `accent_bar_width="4"`, `accent_bar_height="48"`, `accent_bar_gap="12"`
- `docs/LVGL9_XML_GUIDE.md` - New consolidated reference (1170 lines)
- `CLAUDE.md` - Updated documentation references and UI review system guide
- `scripts/review-ui-screenshot.sh` - New automated review script (459 lines)

**Deleted:**
- `docs/LVGL_XML_REFERENCE.md` (merged into LVGL9_XML_GUIDE.md)
- `docs/XML_UI_SYSTEM.md` (merged into LVGL9_XML_GUIDE.md)
- `docs/LVGL9_CENTERING_GUIDE.md` (merged into LVGL9_XML_GUIDE.md)

**Key Discovery:**
- Vertical accent bar pattern provides visual hierarchy and draws attention to status messages
- Uses Material Design principle of colored leading-edge indicators
- Flex layout makes it responsive: bar stays fixed width, text expands to fill space
- Pattern documented in LVGL9_XML_GUIDE.md for reuse across other panels

## Previous Achievements (2025-10-09)

### ✅ Home Panel XML Migration

**Implemented:**
- Migrated home panel from C++ mockup code to XML UI system
- Three display areas: temperature (top-left), network status (center), light control (right)
- Based on Bambu X1C home screen design pattern
- Temperature display with thermometer icon (fa_icons_32) + reactive text
- Network status with WiFi icon (fa_icons_48) + label
- Light control button with lightbulb icon (fa_icons_48) + click handler
- Event callback registration (`light_toggle_cb`) for button interactions

**Critical Discoveries:**
- **Subject registration conflict** - If `globals.xml` declares subjects, they're registered first with empty/default values, blocking C++ initialization
- **Solution:** Remove `<subjects>` section from `globals.xml`, let C++ handle all subject initialization
- **Icon constants work, subject bindings don't** - Using `text="#icon_wifi"` renders correctly, but `bind_text="network_icon"` fails to display FontAwesome UTF-8 glyphs on fa_icons_48
- **Icon generation required** - Must run `python3 scripts/generate-icon-consts.py` to populate UTF-8 bytes in `globals.xml` (invisible in editors but present)
- **Mixed positioning works** - Combining absolute (`x/y`), alignment (`align="center"`), and no flex on parent is valid when children don't conflict

**Reactivity Status:** ✅ **FULLY WORKING**
- Network icon/label dynamically update when calling `ui_panel_home_set_network()`
- Light icon color changes when calling `ui_panel_home_set_light()`
- Observer pattern successfully updates FontAwesome icon glyphs and colors
- Click handler on light button toggles state correctly

**Files Modified (2025-10-09):**
- `ui_xml/globals.xml` - Removed `<subjects>` section, kept icon constants
- `ui_xml/home_panel.xml` - Changed from subject bindings to constants (static icons)
- `src/ui_panel_home.cpp` - Already has reactive API functions ready
- `include/ui_panel_home.h` - Complete API with `network_type_t` enum

### ✅ Home Panel Reactivity Restored (2025-10-10)

**Implemented:**
- C++ observer callbacks for network and light state subjects
- Widget retrieval using `lv_obj_get_child()` (same approach as navigation panel)
- Observer functions update icon text, labels, and colors dynamically
- All three network states cycle correctly: WiFi → Ethernet → Disconnected
- Light state toggles correctly: OFF (gray) ↔ ON (gold)

**Technical Solution:**
- Added `ui_panel_home_setup_observers()` function called after XML panel creation
- Navigate widget hierarchy using child indices instead of deprecated ID system
- Observers watch subjects and update widgets via `lv_label_set_text()` and `lv_obj_set_style_text_color()`
- FontAwesome UTF-8 icon glyphs update correctly through subject changes

**Key Discovery:**
- LVGL 9 provides `lv_obj_find_by_name()` - The proper way to access XML widgets by name
- CRITICAL: Always use name-based lookup, never index-based `lv_obj_get_child(parent, index)`
- Enabled `LV_USE_OBJ_NAME` in `lv_conf.h` (required for name-based widget lookup)
- Disabled `LV_USE_OBJ_ID` (deprecated approach, using names instead)

**Files Modified:**
- `lv_conf.h` - Enabled `LV_USE_OBJ_ID` (line 384)
- `src/ui_panel_home.cpp` - Added observer setup and callbacks
- `include/ui_panel_home.h` - Added `ui_panel_home_setup_observers()` declaration
- `src/main.cpp` - Call observer setup after XML creation
- `ui_xml/home_panel.xml` - Added `name` attributes (though using child indices instead)

### ✅ Name-Based Widget Lookup Implementation (2025-10-10)

**Implemented:**
- Replaced all fragile index-based widget access with name-based lookup
- Updated `ui_panel_home.cpp` to use `lv_obj_find_by_name()` instead of child indices
- Updated `ui_nav.cpp` navigation system to use widget names from arrays
- Added semantic names to all navigation XML widgets (`nav_btn_home`, `nav_icon_home`, etc.)
- Enabled `LV_USE_OBJ_NAME=1` in `lv_conf.h` for name-based lookup functions
- Disabled `LV_USE_OBJ_ID=0` (deprecated approach, not needed with names)

**Technical Solution:**
```cpp
// Old approach (FRAGILE - breaks when layout changes):
lv_obj_t* widget = lv_obj_get_child(parent, 3);

// New approach (RESILIENT - survives layout changes):
lv_obj_t* widget = lv_obj_find_by_name(parent, "my_widget_name");
```

**Benefits:**
- Layout changes (reordering, adding/removing widgets) won't break widget access
- Self-documenting code - widget names show intent
- Consistent with XML-first architecture
- Eliminates brittle child index counting

**Files Modified:**
- `lv_conf.h` - Enabled `LV_USE_OBJ_NAME`, disabled `LV_USE_OBJ_ID`
- `src/ui_panel_home.cpp` - Replaced 6 index lookups with 3 name-based calls (line 80-82)
- `src/ui_nav.cpp` - Replaced loop index access with name arrays (line 121-122)
- `ui_xml/navigation_bar.xml` - Added names to all 10 navigation widgets
- `CLAUDE.md` - Documented name-based lookup as standard pattern
- `.claude/agents/widget-maker.md` - Updated widget-maker agent pattern

### ✅ Home Panel Polish & Consolidation (2025-10-10)

**Fixed:**
- **Removed border from light button** - Light control button had unwanted 2px border with opacity in `home_panel.xml`. Changed to `style_border_width="0"` and `style_pad_all="0"` for clean appearance
- **Consolidated file naming** - Moved XML-based home panel from `ui_panel_home_xml.*` to standard `ui_panel_home.*` files, removing the "_xml" suffix
- **Cleaned up implementation** - Removed obsolete `ui_panel_home_xml.{cpp,h}` files, updated `main.cpp` to use renamed functions

**Files Modified:**
- `ui_xml/home_panel.xml` - Removed border styling from light button (line 35)
- `src/ui_panel_home.cpp` - Now contains XML-based implementation (was pure C++ mockup)
- `include/ui_panel_home.h` - Added `ui_panel_home_init_subjects()` function
- `src/main.cpp` - Updated to call `ui_panel_home_init_subjects()` instead of `_xml` variant
- **Deleted:** `src/ui_panel_home_xml.cpp`, `include/ui_panel_home_xml.h`

**Result:**
- Light button appears as borderless clickable area with just the icon
- Cleaner file organization without redundant "_xml" suffixes
- Consistent naming convention across the project

### ✅ Testing Framework Setup (2025-10-10)

**Implemented:**
- Full testing infrastructure using Catch2 v2.x single-header framework
- Unit test suite for navigation system (5 test cases, 13 assertions)
- Integration test organization (moved `test-navigation.sh` to proper directory)
- Automated test builds via Makefile (`make test` target)
- Comprehensive testing documentation in `tests/README.md`

**Directory Structure:**
```
tests/
├── framework/
│   └── catch.hpp              # Catch2 v2.x (642KB, vendored)
├── unit/
│   ├── test_main.cpp          # Test runner entry point
│   └── test_navigation.cpp    # Navigation system tests
├── integration/
│   └── test-navigation.sh     # Manual UI testing script
└── README.md                  # Complete testing guide
```

**Technical Solution:**
- Vendored Catch2 v2.x single-header (standard practice, no submodule overhead)
- Headless LVGL testing with minimal display buffers (no SDL window needed)
- Test fixtures for proper LVGL initialization and cleanup
- Name-based widget lookup in tests matches production patterns

**Test Coverage:**
```cpp
✅ Default initialization (HOME panel active)
✅ Switching to all 5 panels individually
✅ Invalid panel ID rejection
✅ Repeated panel selection safety
✅ Full panel enumeration
```

**Build Integration:**
```makefile
test: $(TEST_BIN)              # Build and run all tests
$(TEST_BIN): $(TEST_OBJS) $(LVGL_OBJS) $(OBJ_DIR)/ui_nav.o
```

**Files Created:**
- `tests/framework/catch.hpp` - Catch2 v2.13.10 single-header
- `tests/unit/test_main.cpp` - Test runner with `CATCH_CONFIG_MAIN`
- `tests/unit/test_navigation.cpp` - Navigation system tests
- `tests/README.md` - 400+ line testing guide with examples
- `Makefile` - Added test targets and compilation rules

**Files Moved:**
- `test-navigation.sh` → `tests/integration/test-navigation.sh`

**Result:**
- All 13 assertions passing on first run
- Fast unit tests (no UI rendering overhead)
- Pattern established for testing LVGL components
- Auto-discovery of new test files in `tests/unit/*.cpp`
- Comprehensive documentation for writing new tests

## Previous Achievements (2025-10-08)

### ✅ Phase 2 Completed: Navigation & Blank Panels

**Implemented:**
- Clickable navigation with C++ event handlers (`ui_nav.cpp`)
- Active panel tracking via integer subject
- Panel show/hide management based on active state
- Reactive icon color updates (Subject-Observer pattern)
- All 5 panels registered (Home with content, others blank)

**Critical Discovery:**
- **Never call `SDL_PollEvent()` manually** - breaks click events and violates LVGL's display driver abstraction
- LVGL's internal timer (every 5ms) handles all SDL event polling
- Manual polling drains event queue before LVGL can process it
- This principle applies to ALL display drivers (SDL, framebuffer, DRM, etc.)

**Technical Implementation:**
```cpp
// Correct event loop pattern
while (lv_display_get_next(NULL)) {
    lv_timer_handler();  // Internally polls SDL events
    SDL_Delay(5);
}

// Required setup
lv_display_t* display = lv_sdl_window_create(1024, 800);
lv_indev_t* mouse = lv_sdl_mouse_create();  // ESSENTIAL for clicks
```

**Code Architecture:**
- `ui_nav.cpp` - Navigation system with Subject-Observer pattern
- `navigation_bar.xml` - 5 icon buttons with transparent styling
- `app_layout.xml` - Horizontal layout (navbar + content area)
- Panel XML files - 5 panels, content area switches visibility
- `main.cpp` - Initialization, subject registration, event loop

## Completed Phases

### ✅ Phase 1: Foundation
- LVGL 9.3 with XML support
- SDL2 display backend
- Reactive data binding
- Theme system
- FontAwesome integration
- Documentation

### ✅ Phase 2: Navigation & Blank Panels
- Clickable navigation
- Reactive icon highlighting
- Panel switching
- All 5 panel placeholders

## Upcoming Work

### Phase 3: Panel Content (IN PROGRESS)
Priority: High - Build out actual functionality for each panel

### Phase 4: Enhanced UI Components
- Integer subjects for progress bars/sliders
- Custom reusable widgets
- Modal dialogs
- Scrolling lists

### Phase 5: Transitions & Polish
- Panel fade/slide animations
- Button press feedback
- Visual polish

### Phase 6: Backend Integration
- Moonraker WebSocket client
- Real printer state updates
- File operations
- Live temperature/progress monitoring

## Build & Run

```bash
# Build
make

# Run
./build/bin/guppy-ui-proto

# Screenshot
./scripts/screenshot.sh
```

## Documentation

- **[README.md](README.md)** - Overview and quick start
- **[ROADMAP.md](docs/ROADMAP.md)** - Detailed development roadmap
- **[XML_UI_SYSTEM.md](docs/XML_UI_SYSTEM.md)** - Complete XML guide
- **[QUICK_REFERENCE.md](docs/QUICK_REFERENCE.md)** - Common patterns

## Key Metrics

- **Lines of XML:** ~550 (entire UI layout)
- **Lines of C++:** ~700 (initialization + reactive logic)
- **Lines of Test Code:** ~100 (unit tests)
- **Panels:** 5 (1 with content, 4 blank)
- **Subjects:** 10 (nav colors, active panel, home panel data)
- **FontAwesome Sizes:** 3 (64px nav, 48px status cards, 32px inline)
- **Test Coverage:** 5 test cases, 13 assertions (navigation system)
- **Build Time:** ~3 seconds (incremental), ~45 seconds (clean)
- **Test Time:** <1 second (unit tests)
- **Binary Size:** ~2.5MB (debug build)
- **Memory Usage:** ~15MB runtime (SDL backend)

## Architecture Highlights

**Separation of Concerns:**
- XML = UI structure, layout, styling
- C++ = Initialization, business logic, reactive updates
- Subjects = Data flow (one-way: C++ → XML)

**Zero Manual Widget Management:**
- No `lv_obj_t*` stored in application code
- No manual `lv_label_set_text()` calls
- Subject updates trigger all bound widgets automatically

**Display Driver Abstraction:**
- Never call display-specific APIs (SDL, DRM, etc.) directly
- LVGL handles all input/event processing internally
- Portable across all LVGL display backends

---

**Repository:** `prototype-ui9/` (LVGL 9 XML-based implementation)

**Status:** ✅ Navigation complete, home panel content reactive, testing framework operational, remaining panels pending
