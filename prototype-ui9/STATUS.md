# Project Status - LVGL 9 UI Prototype

**Last Updated:** 2025-10-25 (Responsive Headers & Vertical Padding)

## Recent Updates (2025-10-25)

### Responsive Header Heights & Vertical Padding ✅ COMPLETE

**Objective:** Reduce wasted vertical space on tiny/small screens by making header heights and content padding responsive

**Problems Identified:**
1. Fixed 60px header height consumed 18.75% of tiny screen's 320px vertical space
2. Fixed 20px vertical padding at top/bottom of content areas wasted additional space
3. Horizontal padding could stay consistent, but vertical needed to be responsive

**Solution Implemented:**

1. **Header Bar Component System**
   - Created `ui_component_header_bar.cpp/h` for centralized header management
   - Moved `ui_header_bar_*()` functions from ui_utils to dedicated component file
   - Added `ui_component_header_bar_setup()` for responsive height application
   - Integrated with global resize handler system for dynamic updates

2. **Responsive Helper Functions**
   ```cpp
   ui_get_responsive_header_height(screen_height):
     - Tiny (≤479px): 40px
     - Small (480-599px): 48px
     - Medium/Large (≥600px): 60px

   ui_get_header_content_padding(screen_height):
     - Tiny: 6px
     - Small: 10px
     - Medium/Large: 20px
   ```

3. **Split Padding Pattern**
   - Vertical padding (top/bottom): Responsive (6/10/20px)
   - Horizontal padding (left/right): Fixed at UI_PADDING_MEDIUM (12px)
   - Rationale: Vertical space is scarce; horizontal can stay consistent

**Vertical Space Savings:**
- **Tiny (320px height):** Header 60→40px (-20px), padding 20→6px (-14px) = **34px saved** (10.6% more usable space)
- **Small (480px height):** Header 60→48px (-12px), padding 20→10px (-10px) = **22px saved** (4.6% more usable space)
- **Large (720px height):** No change (60px header, 20px padding maintained)

**Files Modified:**
- `include/ui_component_header_bar.h` - New component header (43 lines)
- `src/ui_component_header_bar.cpp` - Component implementation (160 lines)
- `include/ui_utils.h` - Added `ui_get_responsive_header_height()`, removed old header functions
- `src/ui_utils.cpp` - Added responsive height helper, removed moved functions
- `src/main.cpp` - Added `ui_component_header_bar_init()` call before XML registration
- `src/ui_panel_motion.cpp` - Added header setup + split padding
- `src/ui_panel_controls_extrusion.cpp` - Added header setup + split padding
- `src/ui_panel_controls_temp.cpp` - Added header setup to both nozzle/bed panels
- `src/ui_panel_print_status.cpp` - Added header setup
- `Makefile` - Auto-picks up new ui_component_header_bar.cpp

**Testing:**
```bash
# Verified responsive behavior on all screen sizes
./build/bin/guppy-ui-proto -s tiny -p motion    # 40px header, 6px padding
./build/bin/guppy-ui-proto -s small -p motion   # 48px header, 10px padding
./build/bin/guppy-ui-proto -s large -p motion   # 60px header, 20px padding
```

**Pattern Established:**
- Component wrappers for dynamic responsiveness when XML constants aren't sufficient
- Single setup call per panel: `ui_component_header_bar_setup(header, screen)`
- Global resize system handles all instances automatically
- Follows LVGL patterns (explicit setup, no automatic constructors)

**Documentation:**
- Updated `docs/RESPONSIVE_DESIGN_PATTERN.md` with C++ component pattern examples

**Benefits:**
- ✅ 10%+ more usable vertical space on tiny screens
- ✅ Clean component encapsulation (header manages its own behavior)
- ✅ Minimal panel code (one setup line each)
- ✅ Automatic resize handling
- ✅ Consistent pattern for future responsive components

---

### Controls Panel Responsive Launcher Cards ✅ COMPLETE

**Objective:** Fix oversized launcher cards on small screens and eliminate uneven card sizing across rows

**Problems Identified:**
1. Hardcoded 400×200px card dimensions made cards too large on tiny/small screens (480×320 only fit 1 card)
2. `flex_grow` property caused uneven card sizing when rows had different numbers of cards
3. Font sizes and padding too large for compact screens

**Solution Implemented:**

1. **Percentage-Based Width (No flex_grow)**
   - Changed from flexbox constraints to simple percentage width
   - `width="47%"` ensures exactly 2 cards per row on all screens
   - Removed `flex_grow`, `style_flex_basis`, `min_width`, `max_width` complexity
   - Result: All cards identical size across all rows

2. **Semantic Constants for Compact Styling**
   - `launcher_card_width: 47%` - Uniform width (2 cards per row + 20px gap)
   - `launcher_card_padding: 16px` - Reduced from 24px
   - `launcher_card_icon_padding: 12px` - Reduced from 16px
   - `launcher_card_title_padding: 6px` - Reduced from 8px

3. **Smaller Fonts for Better Density**
   - Title font: `montserrat_20` → `montserrat_16`
   - Subtitle font: `montserrat_16` → `montserrat_14`
   - Direct font references (not `#font_heading` constant) for precise control

**Results by Screen Size:**
- **Tiny (480×320):** 2 cards per row - much more usable (was 1)
- **Small (800×480):** 2 cards per row - perfect fit
- **Medium (1024×600):** 2 cards per row - clean layout
- **Large (1280×720):** 2 cards per row - consistent sizing

**Files Modified:**
- `ui_xml/globals.xml` - Added launcher card semantic constants (lines 107-118)
- `ui_xml/controls_panel.xml` - Updated all 6 cards with responsive sizing

**Testing:**
```bash
make clean && make
./scripts/screenshot.sh guppy-ui-proto controls-tiny-fixed controls -s tiny
./scripts/screenshot.sh guppy-ui-proto controls-small-fixed controls -s small
./scripts/screenshot.sh guppy-ui-proto controls-medium-fixed controls
./scripts/screenshot.sh guppy-ui-proto controls-large-fixed controls -s large
```

**Screenshots:**
- `/tmp/ui-screenshot-controls-tiny-fixed.png` - 2 cards per row
- `/tmp/ui-screenshot-controls-small-fixed.png` - 2 cards per row
- `/tmp/ui-screenshot-controls-medium-fixed.png` - 2 cards per row
- `/tmp/ui-screenshot-controls-large-fixed.png` - 2 cards per row

**Pattern Established:**
Percentage-based widths for uniform card sizing across rows (similar to print file cards pattern, but simpler - no dynamic C++ calculation needed for launcher cards).

**Benefits:**
- ✅ All cards identical size (no uneven rows)
- ✅ Tiny screens now usable with 2 cards per row
- ✅ Compact fonts/padding improve information density
- ✅ Simple, maintainable approach (just percentage width)

**Next:** Apply responsive design pattern to other panels as needed.

---

### Command-Q Quit Shortcut & Makefile Parallel Build Fix ✅ COMPLETE

**Objective:** Add keyboard shortcut to quit application and ensure default make uses parallel building

**Implementation:**

1. **Command-Q Quit Shortcut**
   - Added Cmd+Q (macOS) / Win+Q (Windows) keyboard shortcut to quit application
   - Implementation in `src/main.cpp` lines 555-561 (main event loop)
   - Uses SDL state query functions: `SDL_GetModState()` and `SDL_GetKeyboardState()`
   - Detects `KMOD_GUI` modifier (Cmd/Win key) + `SDL_SCANCODE_Q` simultaneously
   - Breaks main loop cleanly, allowing proper cleanup code to run
   - Logs "Cmd+Q/Win+Q pressed - exiting..." when triggered

2. **Makefile Parallel Building by Default**
   - Added `MAKEFLAGS += -j$(NPROC)` to Makefile line 57
   - Default `make` command now automatically uses all CPU cores
   - NPROC auto-detects cores: `sysctl -n hw.ncpu` (macOS) / `nproc` (Linux) / fallback 4
   - Significantly speeds up compilation without requiring `make build`

**Technical Details:**
- **State queries don't interfere with LVGL:** `SDL_GetModState()` and `SDL_GetKeyboardState()` query keyboard state without draining SDL event queue
- **Respects LVGL architecture:** Project constraint prohibits calling `SDL_PollEvent()` manually - state queries are safe
- **Cross-platform:** `KMOD_GUI` maps to Cmd (macOS), Windows key (Windows/Linux)
- **Clean exit:** Breaking loop proceeds to `lv_deinit()` cleanup (line 576) and returns 0

**Files Modified:**
- `src/main.cpp` - Added Cmd+Q detection in main loop (lines 555-561)
- `Makefile` - Added `MAKEFLAGS += -j$(NPROC)` for auto-parallel builds (line 57)

**Testing:**
```bash
make                          # Now uses all cores automatically
./build/bin/guppy-ui-proto    # Press Cmd+Q to quit
```

**Benefits:**
- Faster default builds (no need to remember `make build` vs `make`)
- Consistent with HANDOFF.md documentation ("make # Incremental build (auto-parallel)")
- Convenient quit method for macOS users (standard Cmd+Q behavior)

---

### Documentation Cleanup - Keypad Event Handlers Clarification ✅ COMPLETE

**Objective:** Clarify keypad implementation status and remove stale documentation

**Status Clarification:**
- **Event handlers are COMPLETE** - All numeric input, backspace, OK, cancel, and backdrop click handlers were implemented in Phase 5.2 (2025-10-12 Late Night)
- Implementation includes: digit buttons (0-9), backspace, OK button with value parsing/clamping/callback, back button cancellation, backdrop click dismissal
- See `src/ui_component_keypad.cpp` lines 176-356 for full implementation

**Documentation Updates:**
- Updated HANDOFF.md to remove stale "Wire Keypad Event Handlers" task from immediate next steps
- Updated HANDOFF.md to mark keypad testing as complete
- This entry added to STATUS.md to clarify implementation timeline

**Timeline:**
- 2025-10-12 Late Night: Numeric keypad component created with complete event handling (Phase 5.2)
- 2025-10-25: Responsive design improvements and sizing fixes
- 2025-10-25: FontAwesome backspace icon integration
- 2025-10-25: Documentation cleanup (this entry)

**Next Priority:** Apply responsive design pattern to other panels (motion, temp, extrusion)

---

### Keypad Button Sizing & FontAwesome Icon Integration ✅ COMPLETE

**Objective:** Remove max button height constraint and replace placeholder backspace character with FontAwesome icon

**Implementation:**

1. **Removed Button Height Constraint**
   - Removed `style_max_height="#button_max_height"` (70px cap) from all 11 keypad buttons
   - Removed unused `button_max_height` constant from numeric_keypad_modal.xml
   - Buttons now use `height="100%"` with `flex_grow="1"` to fill available space
   - Result: Taller buttons on larger screens, better use of vertical space

2. **FontAwesome Backspace Icon Integration**
   - Fixed `scripts/generate-icon-consts.py` pattern matching for globals.xml
   - Generated icon constants with proper UTF-8 encoding (U+F55A for backspace)
   - Added 33 icon constants to `ui_xml/globals.xml` using script
   - Updated backspace button to use `text="#icon_backspace" style_text_font="fa_icons_24"`
   - Size consistency: fa_icons_24 matches montserrat_20 button labels better than fa_icons_32

3. **Icon Generation Script Updates**
   - Updated regex pattern to handle tab/space variations in globals.xml
   - Script now properly replaces existing icon section instead of failing silently
   - UTF-8 characters correctly embedded (appear empty in terminal but render with font)

**Files Modified:**
- `ui_xml/numeric_keypad_modal.xml` - Removed max height, added FA backspace icon
- `ui_xml/globals.xml` - Added 33 icon constants with UTF-8 characters
- `scripts/generate-icon-consts.py` - Fixed pattern matching for reliable updates

**Technical Details:**
- Icon constants use Private Use Area Unicode (U+F000-U+F8FF)
- Characters appear empty in terminal/grep but contain UTF-8 bytes (e.g., ef959a for U+F55A)
- FontAwesome fonts (fa_icons_16/24/32/48/64) already declared and registered in main.cpp
- Icon constant validation: `python3 -c "print(repr(chr(0xF55A)))"` → `'\uf55a'`

**Verification:**
```bash
# Check UTF-8 encoding
python3 << 'EOF'
with open('ui_xml/globals.xml', 'r') as f:
    for line in f:
        if 'icon_backspace' in line:
            import re
            match = re.search(r'value="([^"]*)"', line)
            if match:
                print(f"Bytes: {match.group(1).encode('utf-8').hex()}")  # ef959a
EOF

# Rebuild and test
make clean && make
./build/bin/guppy-ui-proto -k  # Auto-open keypad for testing
```

---

## Recent Updates (2025-10-25)

### Responsive Numeric Keypad - Mobile-First Design ✅ COMPLETE

**Objective:** Make numeric keypad work across all screen sizes from tiny (480×320) to large (1280×720)

**Approach:** Mobile-first XML-based design using semantic constants

**Implementation:**

1. **Removed All Hardcoded Sizing**
   - Eliminated keypad-specific constants (keypad_padding, keypad_section_gap, etc.)
   - Replaced with semantic constants: `#padding_small`, `#gap_normal`, `#gap_small`
   - All sizing declarative in XML, zero C++ runtime adjustments

2. **Responsive Flex Layout**
   - Grid container: `flex_grow="1"` fills available vertical space
   - Button rows: `flex_grow="1"` with `space_evenly` distribution
   - Individual buttons: `flex_grow="1"` and `height="100%"`
   - Input display/header: `height="content"` for natural sizing

3. **Mobile-First Font Sizes**
   - Header title: `montserrat_20` (was 28)
   - Button labels: `montserrat_20` (was 28)
   - Input display: `montserrat_28` (was 48)
   - Icon size: `sm` instead of `md`

4. **Testing Infrastructure**
   - Added `-k`/`--keypad` flag to auto-open keypad for testing
   - Created `scripts/test_keypad_sizes.sh` for automated multi-size testing

**Results:**
- ✅ Tiny (480×320): All buttons visible, compact but usable
- ✅ Small (800×480): Comfortable spacing
- ✅ Medium (1024×600): Perfect proportions
- ✅ Large (1280×720): Spacious and comfortable

**Files Modified:**
- `ui_xml/numeric_keypad_modal.xml` - Responsive layout
- `ui_xml/globals.xml` - Removed redundant constants
- `src/main.cpp` - Added `-k` test flag
- `scripts/test_keypad_sizes.sh` - New test script

**Key Insight:** XML-first approach with semantic constants eliminates need for C++ runtime adjustments. This pattern should be applied to other panels (motion, temps, extrusion).

---

### Interactive Testing & Test Automation ✅ COMPLETE

**Objective:** Verify navigation flows, UI polish, and panel rendering across all configurations.

**Testing Approach:**

**1. Automated Test Script** (`scripts/test_navigation.sh`)
- Created comprehensive automated test suite
- Tests all command-line panel flags (-p home, controls, motion, etc.)
- Tests all screen size flags (-s tiny, small, medium, large)
- Verifies all panels render without errors
- **Results:** ✅ ALL TESTS PASS (26/26)
  - 8 panel flags working correctly
  - 4 screen sizes supported
  - 9 panels render without errors
  - Zero initialization failures

**2. UI Polish Verification** (Screenshot Review)
- Print select card view: ✅ Rounded corners visible, thumbnails scaled properly
- Print file detail view: ✅ Rounded corners, object-fit:contain working
- Print status panel: ✅ Perfect 2:1 layout, gradient backgrounds
- Motion panel: ✅ Clean overlay, back chevron visible
- Nozzle/Bed temp panels: ✅ Preset buttons, custom button, back chevron
- Extrusion panel: ✅ Proper layout, all controls visible

**3. Navigation Architecture Clarification**
- **Panel Stack:** Motion, Nozzle Temp, Bed Temp, Extrusion, Print Status
  - Use `ui_nav_push_overlay()` for stack-based navigation
  - Back buttons use `ui_nav_go_back()` to pop stack
- **Modal Dialogs:** Numeric keypad
  - Simple show/hide (`lv_obj_remove_flag`/`lv_obj_add_flag`)
  - Not part of navigation stack (appears on top)

**Testing Summary:**
```
✓ All command line flags working
✓ All screen sizes supported (480x320 to 1280x720)
✓ All panels render without errors
✓ Rounded corners visible on cards and detail views
✓ Thumbnails scale properly (cover for cards, contain for detail)
✓ Gradient backgrounds working correctly
✓ Back chevrons visible on all overlay panels
✓ Navigation bar highlights correct active panel
```

**Screenshots Generated:**
- `/tmp/ui-screenshot-nav-test-home.png`
- `/tmp/ui-screenshot-nav-test-controls.png`
- `/tmp/ui-screenshot-nav-test-motion.png`
- `/tmp/ui-screenshot-nav-test-nozzle-temp.png`
- `/tmp/ui-screenshot-nav-test-bed-temp.png`
- `/tmp/ui-screenshot-nav-test-extrusion.png`
- `/tmp/ui-screenshot-nav-test-print-select.png`
- `/tmp/ui-screenshot-nav-test-file-detail.png`
- `/tmp/ui-screenshot-nav-test-print-status.png`

**Manual Testing Still Required:**
- Interactive navigation (click motion card → back button behavior)
- Multi-level navigation (nozzle temp → keypad modal → ESC → back)
- State preservation (set temp values, navigate away, navigate back)
- Rapid button clicking (stress test event handlers)

**Files Created:**
- `scripts/test_navigation.sh` - Automated test suite

**Status:** ✅ **Ready for Moonraker Integration** - All UI components functional, navigation system robust, visual polish complete.

---

## Previous Updates (2025-10-25)

### Navigation System Refactoring & Bug Fixes ✅ COMPLETE

**Objective:** Fix navigation system issues, UI polish bugs, and resolve critical blank screen bug.

**Problems Addressed:**
1. Print file cards lacked rounded corners (image overflow)
2. Print detail view thumbnails not scaling when shown
3. Navigation history not clearing when clicking nav bar icons
4. Back button showing blank screen when history empty
5. Nav buttons not properly showing clicked panel
6. **CRITICAL:** Clicking nav buttons caused total blank screen (navbar and panels disappeared)

**Solutions Implemented:**

**1. UI Polish - Rounded Corners**
- Added `style_clip_corner="true"` to `print_file_card.xml` card root (line 63)
- Added same to `print_file_detail.xml` thumbnail section (line 62)
- Prevents thumbnail images from overflowing rounded card backgrounds

**2. Detail View Image Scaling** (`ui_panel_print_select.cpp`)
- Created `scale_detail_images()` function (lines 633-662)
- Calls `lv_obj_update_layout()` before scaling (CRITICAL for flex layouts)
- Uses `ui_image_scale_to_cover()` for gradient background
- Uses `ui_image_scale_to_contain()` for thumbnail (top-mid alignment)
- Called when detail view is shown (line 671), not during creation
- Follows print_status panel pattern for proper timing

**3. Navigation System Refactoring** (`ui_nav.cpp`)
- **Unified panel stack approach** - replaced dual system (panel_widgets + nav_history)
- Changed `nav_history` to `panel_stack` (lines 43-48) - tracks ALL visible panels in z-order
- Refactored `nav_button_clicked_cb()` (lines 82-136):
  - Hides all visible overlay panels (not in panel_widgets array)
  - Hides all main panels
  - Clears stack completely
  - Shows clicked panel and pushes to stack
  - Updates active panel subject
- Made `ui_nav_go_back()` defensive (lines 333-408):
  - ALWAYS hides any visible overlay panels first
  - Pops stack
  - Fallback to HOME panel if stack empty
  - Hides other main panels when showing previous main panel
  - Returns true if navigation occurred, false if no previous panel

**4. App Layout Protection - CRITICAL FIX** (`ui_nav.cpp`, `ui_nav.h`, `main.cpp`)
- **Root cause of blank screen:** Navigation code was hiding `app_layout` container (which holds navbar + all panels)
- Added `ui_nav_set_app_layout()` function (ui_nav.h:54-55)
- Store `app_layout_widget` static reference (ui_nav.cpp:39-40, 183-186)
- Modified `nav_button_clicked_cb()` to skip app_layout when hiding overlays (lines 91-94)
- Modified `ui_nav_go_back()` to skip app_layout when hiding overlays (line 346)
- Called `ui_nav_set_app_layout(app_layout)` in main.cpp after XML creation (line 346)

**Architecture Context:**
```
screen
└── app_layout (navbar + content_area container)
    ├── navigation_bar
    └── content_area
        ├── home_panel
        ├── controls_panel
        ├── print_select_panel
        └── ... other main panels
```

**Key Insight:** Navbar is inside `app_layout`, not a direct screen child. Comparing against `navbar_widget` didn't work because navigation code iterates screen children. Solution: Track and skip `app_layout` entirely.

**Files Modified:**
- `ui_xml/print_file_card.xml` - Added rounded corner clipping (line 63)
- `ui_xml/print_file_detail.xml` - Added rounded corner clipping (line 62)
- `src/ui_panel_print_select.cpp` - Detail view image scaling (lines 633-662, 671)
- `include/ui_nav.h` - Added `ui_nav_set_app_layout()` declaration (lines 54-55)
- `src/ui_nav.cpp` - Refactored to panel_stack, defensive navigation, app_layout protection (lines 39-40, 43-48, 82-136, 183-186, 333-408)
- `src/main.cpp` - Register app_layout reference (line 346)

**Testing:** ✅ All navigation flows working correctly. No blank screens. Back button defaults to home. Nav buttons clear history and show selected panel.

---

## Previous Updates (2025-10-25)

### Navigation History Stack Implementation ✅ COMPLETE

**Objective:** Implement back button navigation history for overlay panels with state preservation.

**Problem:**
- Back buttons on overlay panels (motion, temps, extrusion) manually show/hide specific panels
- No navigation stack - can't track "previous panel" for multi-level navigation
- User requested: clicking nav bar icons should clear history; back buttons should use history

**Solution Implemented:**

**1. Navigation History Stack** (`ui_nav.cpp`/`ui_nav.h`)
- Added `std::vector<lv_obj_t*> nav_history` to track panel navigation
- Created `ui_nav_push_overlay(lv_obj_t*)` - pushes current panel to history and shows overlay
- Created `ui_nav_go_back()` - pops history and returns to previous panel
- Updated nav bar button handler to **clear history** when clicking any nav icon

**2. Updated All Overlay Show Functions** (use `ui_nav_push_overlay()`)
- `ui_panel_controls.cpp`: motion, nozzle temp, bed temp, extrusion card click handlers
- Each handler now calls `ui_nav_push_overlay()` instead of manual show/hide

**3. Updated All Back Button Callbacks** (use `ui_nav_go_back()`)
- `ui_panel_motion.cpp` - motion panel back button
- `ui_panel_controls_temp.cpp` - nozzle & bed temp back buttons
- `ui_panel_controls_extrusion.cpp` - extrusion back button
- `ui_panel_print_status.cpp` - print status back button
- Each includes fallback to manual show/hide if history is empty

**How It Works:**
1. **Opening overlay:** Current visible panel pushed to history, overlay shown
2. **Back button:** Pops history, hides current overlay, shows previous panel
3. **Nav bar click:** Clears history, shows selected main panel

**Files Modified:**
- `include/ui_nav.h` - Added function declarations
- `src/ui_nav.cpp` - Implemented history stack (lines 44, 250-327)
- `src/ui_panel_controls.cpp` - Updated card click handlers (lines 26, 128-130, 157-159, 185-187, 212-215)
- `src/ui_panel_motion.cpp` - Updated back button (lines 22, 86-99)
- `src/ui_panel_controls_temp.cpp` - Updated temp back buttons (lines 24, 112-125, 242-255)
- `src/ui_panel_controls_extrusion.cpp` - Updated back button (lines 22, 176-189)
- `src/ui_panel_print_status.cpp` - Updated back button (lines 23, 179-192)

**Testing:** ✅ Build successful, navigation system ready for interactive testing

---

## Previous Updates (2025-10-24)

### Image Scaling Refactoring & Pattern Documentation ✅ COMPLETE

**Objective:** Fix gradient/thumbnail scaling issues in print status panel and extract reusable image scaling utilities.

**Problem Identified:**
- Print status panel gradient and thumbnail not displaying (appeared as black/empty)
- Scaling code attempted to query layout dimensions before LVGL calculated them
- Result: Container reported 0x0 size, scaling functions did nothing

**Root Cause:**
LVGL uses **deferred layout calculation**. After setting dimensions with `lv_obj_set_width()`, flex/grid layouts don't immediately recalculate child sizes - they mark layouts "dirty" and process during next render cycle. Without forcing synchronous update, widgets report incorrect dimensions.

**Solution Implemented:**

**1. Reusable Image Scaling Utilities** (ui_utils.h/cpp)
- Added `ui_image_scale_to_cover()` - Like CSS object-fit: cover (fills area, may crop)
- Added `ui_image_scale_to_contain()` - Like CSS object-fit: contain (fits within, no crop)
- Both handle LVGL zoom calculation (256 = 1.0x scale)
- Both include debug logging for troubleshooting

**2. Fixed Print Status Panel** (ui_panel_print_status.cpp:297-314)
- Added `lv_obj_update_layout(panel)` call before scaling images
- Forces synchronous layout calculation so dimensions are available
- Refactored scaling code to use new utility functions
- Created `scale_thumbnail_images()` helper called on setup and resize

**3. Documented Pattern** (CLAUDE.md Critical Pattern #5)
- Brief explanation of why `lv_obj_update_layout()` is required
- References to implementation locations for detailed examples
- Keeps CLAUDE.md concise with pointers to supporting documentation

**Testing:**
- ✅ Print status panel: Gradient and thumbnail now display correctly
- ✅ Print select panel: Continues to work (no changes needed, scales during dynamic creation)
- Screenshot: `/tmp/ui-screenshot-print-status-fixed.png`

**Key Learning:**
Print select panel worked without explicit layout update because it scales images dynamically during `populate_card_view()`, called after setup. By then, LVGL's event loop naturally calculated layouts. Print status panel scales images immediately during `ui_panel_print_status_setup()`, requiring explicit `lv_obj_update_layout()`.

**Files Changed:**
- `include/ui_utils.h` - Added function declarations
- `src/ui_utils.cpp` - Implemented scaling utilities (lines 213-276)
- `src/ui_panel_print_status.cpp` - Added layout update and refactored scaling (lines 249-314)
- `CLAUDE.md` - Added Critical Pattern #5, streamlined documentation

---

### Print Status Panel Layout Fix - Incremental Rebuild ✅ COMPLETE

**Objective:** Fix the broken print status panel layout using an incremental approach, building up from a minimal working state.

**Problem:** Previous layout used hardcoded percentage widths (`width="66%"` and `width="34%"`) which caused layout collapse and overflow. Multiple attempts to fix with various flex approaches failed. Root cause was trying to add all content at once without verifying each step.

**Approach Taken:** **Incremental build with testing at each step**
1. Strip down to minimal empty containers with `flex_grow` only
2. Verify 2:1 ratio works
3. Add content one piece at a time
4. Test after each addition
5. Fix immediately if something breaks

**Changes Made:**

**Phase 1: Minimal Empty Layout** ✅
- Removed all content from `thumbnail_section` and `controls_section`
- Used only `flex_grow="2"` and `flex_grow="1"` (no width attributes)
- Kept `height="100%"` on both sections
- Screenshot: `/tmp/ui-screenshot-print-status-empty.png`
- **Result:** Perfect 2:1 ratio with empty containers

**Phase 2: Thumbnail Images with Resize Callback** ✅
- **XML Changes** (ui_xml/print_status_panel.xml):
  - Added `gradient_background` image (thumbnail-gradient-bg.png)
  - Added `print_thumbnail` image (placeholder with `bind_src="selected_thumbnail"`)
  - Both images: `width="100%"`, `height="100%"`, centered alignment

- **C++ Changes** (src/ui_panel_print_status.cpp:249-300):
  - Added `#include "ui_utils.h"` for resize handler
  - Created `on_resize()` callback function
  - Gradient background: object-fit cover (use larger scale)
  - Print thumbnail: object-fit contain (use smaller scale)
  - Registered resize callback in setup: `ui_resize_handler_register(on_resize)`

- **Why Resize Callback:** Initial attempt ran scaling during setup when dimensions were 0. Resize callback runs after layout is calculated, just like print_select panel's detail view.

- Screenshot: `/tmp/ui-screenshot-print-status-resize-test.png`
- **Result:** Thumbnail renders properly, 2:1 ratio maintained

**Phase 3: Metadata Overlay** ✅
- Added metadata overlay at bottom of thumbnail section
- Includes progress bar (lv_bar bound to `print_progress` subject)
- Filename label (bound to `print_filename`)
- Time and percentage labels in horizontal row
- Semi-transparent black background (opa=180) overlays thumbnail
- Height: 120px, positioned with `align="bottom_mid"`
- Screenshot: `/tmp/ui-screenshot-print-status-complete.png`

**Phase 4: Controls Section - Temperature Cards & Buttons** ✅
- Made controls_section background transparent (`style_bg_opa="0"`)
- Added column flex layout with `#padding_normal` gap
- **Temperature Cards:**
  - Nozzle temp card: 100% width × 80px height
  - Bed temp card: 100% width × 80px height
  - Both display current/target temps (bound to `nozzle_temp_display`, `bed_temp_display`)
  - Card background: `#card_bg`, radius: 8px
- **Spacer:** `flex_grow="1"` transparent container pushes buttons to bottom
- **Control Buttons (2×2 Grid):**
  - Row 1: Light, Pause (120×54px each)
  - Row 2: Tune, Cancel (120×54px each)
  - Cancel button styled in red (0xE74C3C) for visual emphasis
  - All others use `#jog_button_bg`
- Screenshot: `/tmp/ui-screenshot-print-status-final.png`

**Final Result:**
- ✅ Perfect 2:1 layout ratio maintained (thumbnail_section flex_grow="2", controls_section flex_grow="1")
- ✅ All content displays correctly with proper spacing
- ✅ Temperature cards clickable (event handlers wired in C++)
- ✅ No XML parsing errors, no layout collapse
- ✅ Ready for integration with live printer data

**Key Learning:** Incremental approach with testing prevents cascading failures. Build from known-good state, add one thing at a time.

**Files Modified:**
- `ui_xml/print_status_panel.xml` - Complete 2:1 layout with thumbnail, metadata overlay, temp cards, and control buttons
- `src/ui_panel_print_status.cpp` - Added resize callback for thumbnail scaling

**Status:** ✅ Foundation solid, 🔨 Content additions in progress

---

## Previous Updates (2025-10-24)

### Print Button → Print Status Panel Integration ✅ COMPLETE

**Objective:** Wire the Print button on file detail view to launch the print status panel with a mock print.

**Problem:** Print status panel existed but had no way to access it from the UI. Print button on detail view was non-functional.

**Changes Made:**

**1. Print Select Panel Integration** (src/ui_panel_print_select.cpp)
- Added `#include "ui_panel_print_status.h"`
- Added static reference to print status panel widget
- Added Print button event handler in `create_detail_view()`:
  - Hides detail view and print select panel
  - Shows print status panel overlay
  - Starts mock print with selected filename (250 layers, 10800 seconds)
  - Logs print start for debugging
- Added `ui_panel_print_select_set_print_status_panel()` function to receive panel reference

**2. Public API** (include/ui_panel_print_select.h)
- Added function declaration for `ui_panel_print_select_set_print_status_panel(lv_obj_t* panel)`

**3. Main Setup** (src/main.cpp:379-391)
- Creates print status panel at startup as screen overlay
- Hides it by default with `LV_OBJ_FLAG_HIDDEN`
- Wires it to print select panel via setter function
- Mock print tick already exists in main loop (line 540)

**Test Flow:**
1. Navigate to Print Select panel
2. Click any file card → detail view opens
3. Click green "Print" button
4. Print status panel appears with mock print running
5. Progress updates every second (0-100%)
6. Temperatures, layers, times update realistically
7. Back button returns to Home panel

**Verification Logs:**
```
[User] Print status panel reference set
[User] Print status panel created and wired to print select
[User] Started mock print for: awesome_benchy.gcode
```

**Known Issues:**
- ⚠️ **Print Status Panel UI needs complete redesign** - Current layout has:
  - Overlapping elements
  - Misaligned/missized components
  - Poor visual hierarchy
  - Not production-ready visually
- **Logic and integration work correctly** - Data flow, mock simulation, and panel transitions all functional
- **Next session should focus on UI refinement** - Interactive design iteration needed

**Files Modified:**
- `src/ui_panel_print_select.cpp` - Print button handler, panel reference
- `include/ui_panel_print_select.h` - Public API for panel setter
- `src/main.cpp` - Print status panel creation and wiring

**Status:** ✅ Integration complete, ⚠️ UI design needs work

---

## Previous Updates (2025-10-24)

### Header Bar Responsive Refactoring ✅ COMPLETE

**Objective:** Remove hardcoded widths from header_bar component and use semantic constants for full responsiveness.

**Problem:** Header bar used hardcoded pixel widths (400px for left container, 320px for title) that didn't adapt to different screen sizes or content lengths.

**Changes Made:**

**1. Added Semantic Constants** (ui_xml/globals.xml:126-133)
- `header_back_button_size`: 60px (back button touch target)
- `header_right_button_width`: 120px (action button width)
- `header_right_button_height`: 40px (action button height)

**2. Refactored Header Bar Layout** (ui_xml/header_bar.xml)
- Root height: `40` → `#header_height` (60px from globals)
- Left container: Fixed `width="400"` → removed width, added `flex_grow="1"` (responsive)
- Title label: Fixed `width="320"` → removed width (auto-sizes to content)
- Back button: `width="60" height="60"` → `width="#header_back_button_size" height="#header_back_button_size"`
- Right button: `width="120" height="40"` → `width="#header_right_button_width" height="#header_right_button_height"`
- Right button radius: `8` → `#card_radius` (consistency with global styling)
- Added `style_pad_right="#padding_normal"` to root (20px spacing from right edge)

**Benefits:**
- Header bar now fully responsive - adapts to any screen size
- Title text auto-sizes to content length
- Left container grows to fill available space
- All dimensions use semantic constants (easier to maintain/theme)
- Proper visual spacing on right edge

**Files Modified:**
- `ui_xml/globals.xml` - Added 3 new header-specific constants
- `ui_xml/header_bar.xml` - Removed all hardcoded values, added responsive layout

**Verification:**
- Tested with nozzle-temp panel (shows "Confirm" button)
- Tested with motion panel (no right button)
- Both render correctly with proper spacing

---

## Previous Updates (2025-10-24)

### LVGL 9 XML Flag Syntax Cleanup ✅ COMPLETE

**Objective:** Fix all incorrect `flag_*` attribute usage across entire codebase.

**Problem:** LVGL 9 XML uses simplified attribute syntax without `flag_` prefix. Using `flag_hidden="true"`, `flag_clickable="true"`, etc. causes attributes to be silently ignored by the parser.

**Scope:** 12 XML files with 80+ incorrect attribute usages

**Files Fixed:**
1. `print_file_detail.xml` - 5 occurrences (clickable, scrollable)
2. `motion_panel.xml` - 16 occurrences (all scrollable in jog pad/controls)
3. `numeric_keypad_modal.xml` - 9 occurrences (clickable, scrollable)
4. `controls_panel.xml` - 1 occurrence (scrollable on main view)
5. `nozzle_temp_panel.xml` - 10 occurrences (clickable, scrollable)
6. `print_file_card.xml` - 3 occurrences (clickable, scrollable)
7. `print_select_panel.xml` - 11 occurrences (scrollable, hidden)
8. `print_status_panel.xml` - 1 occurrence (scrollable)
9. `confirmation_dialog.xml` - 3 occurrences (clickable)
10. `extrusion_panel.xml` - 9 occurrences (clickable, scrollable, hidden)
11. `print_file_list_row.xml` - 1 occurrence (clickable)
12. `bed_temp_panel.xml` - 10 occurrences (clickable, scrollable)

**Replacements Made:**
- `flag_clickable="true"` → `clickable="true"`
- `flag_scrollable="false"` → `scrollable="false"`
- `flag_scrollable="true"` → `scrollable="true"`
- `flag_hidden="true"` → `hidden="true"`

**Verification:** `grep -r 'flag_\w+="' ui_xml/` returns no matches - all incorrect syntax removed.

**Impact:**
- Backdrop click handlers now properly functional
- Scrollable/non-scrollable containers behave as designed
- Hidden elements properly hidden without C++ workarounds
- List view sort indicators visibility now controlled declaratively

**Technical Reference:**
- Source: `lvgl/src/widgets/property/lv_obj_properties.c:34`
- Documentation: `docs/LVGL9_XML_ATTRIBUTES_REFERENCE.md:91-123`

---

### Detail View Final Fixes + LVGL 9 XML Attribute Syntax Discovery ✅ COMPLETE

**Objective:** Fix remaining issues with print file detail view and resolve header button visibility.

**Issues Fixed:**
1. **Gradient background too small** - Background image centered instead of filling entire thumbnail section
2. **Header button still visible** - Button appeared in detail view header despite `flag_hidden="true"` in XML

**Root Cause Discovery:**
LVGL 9 XML uses **simplified attribute syntax** without `flag_` prefix:
- ❌ Wrong: `flag_hidden="true"`, `flag_clickable="true"`, `flag_scrollable="false"`
- ✅ Correct: `hidden="true"`, `clickable="true"`, `scrollable="false"`

This is documented in `docs/LVGL9_XML_ATTRIBUTES_REFERENCE.md:97` but we were using the wrong syntax throughout the codebase.

**Solutions Implemented:**

**1. Fixed Gradient Background Scaling** (src/ui_panel_print_select.cpp:664-735)
- Added C++ code to scale gradient background images to fill containers
- Uses same "cover" fit algorithm as thumbnails: `scale = (scale_w > scale_h) ? scale_w : scale_h`
- Applied to both card view gradients and detail view gradient
- Gradient now fills entire thumbnail area (like CSS `object-fit: cover`)

**2. Fixed Header Button Visibility** (ui_xml/header_bar.xml:41, 49, 60, 82-83)
- Changed all `flag_scrollable` → `scrollable` (4 occurrences)
- Changed `flag_hidden="true"` → `hidden="true"` (1 occurrence)
- Button now properly hidden in detail view using declarative XML
- No C++ workarounds needed

**3. Fixed Button Color** (ui_xml/header_bar.xml:78)
- Changed `style_bg_color="#success_color"` → `style_bg_color="#primary_color"`
- Now uses global constant for consistency

**Files Modified:**
- `ui_xml/header_bar.xml` - Fixed all flag attribute syntax, updated button color
- `src/ui_panel_print_select.cpp` - Added gradient background scaling code

**Technical Lessons:**
- LVGL 9 XML property system auto-generates simplified attribute names from enum values
- See `lvgl/src/widgets/property/lv_obj_properties.c:34` - property is named `flag_hidden` in C but `hidden` in XML
- Many files in codebase use incorrect `flag_*` syntax and may not work as expected
- Future work: Audit all XML files for incorrect `flag_*` usage

**Visual Results:**
- Gradient background fills entire thumbnail section (no centering issues)
- Header button properly hidden in detail view
- Button uses correct primary color constant

---

## Previous Updates (2025-10-24)

### Print File Detail View Redesign ✅ COMPLETE

**Objective:** Fix major issues with print file detail overlay panel to match card view design.

**Issues Fixed:**
1. **Thumbnail not displaying** - Incorrect image path (`placeholder_thumb_centered.png` doesn't exist)
2. **Wrong styling** - Detail view didn't match card appearance (no gradient, different metadata layout)
3. **Positioning bug** - Detail view appeared below print select header instead of overlaying entire screen
4. **Thumbnail cropping** - Image zoomed too much, cutting off top/bottom
5. **Empty green button** - Right button visible in header despite not being needed
6. **Wrong back icon** - Curved arrow instead of left chevron

**Solutions Implemented:**

**1. Fixed Thumbnail Path** (ui_xml/print_file_detail.xml:78)
- Changed `placeholder_thumb_centered.png` → `thumbnail-placeholder.png`
- Added `bind_src="selected_thumbnail"` for reactive updates

**2. Redesigned Layout to Match Card View**
- Changed from 3-section vertical layout to proper 2-column design
- Header bar now full-width at top (60px, no excess padding)
- Content area below with left (2/3) and right (1/3) sections
- Left section styled exactly like large card:
  - Gradient background (`thumbnail-gradient-bg.png`)
  - Semi-transparent metadata overlay at bottom
  - Filename + icon rows (mat_clock + time, mat_layers + filament)
  - Uses same icon component pattern as cards

**3. Fixed Positioning** (src/ui_panel_print_select.cpp:640)
- Changed parent from `panel_root_widget` → `parent_screen_widget`
- Detail view now created as direct child of screen, not panel
- Properly overlays entire screen from top to bottom

**4. Fixed Thumbnail Zoom Calculation** (src/ui_panel_print_select.cpp:670)
- Changed from "cover" fit to "contain" fit
- Before: `scale = (scale_w > scale_h) ? scale_w : scale_h;` (larger scale, crops)
- After: `scale = (scale_w < scale_h) ? scale_w : scale_h;` (smaller scale, fits)
- Full thumbnail now visible without cropping

**5. Fixed Back Icon** (ui_xml/header_bar.xml:62)
- Changed `mat_back` (curved arrow) → `mat_arrow_left` (left chevron)
- Consistent with standard navigation patterns

**6. Empty Button Investigation**
- header_bar component correctly defaults to `flag_hidden="true"` for right button
- Panels explicitly call `ui_header_bar_show_right_button()` to enable it
- Detail view doesn't call this, so button stays hidden as designed
- No component fix needed

**7. Added Test Support** (src/main.cpp:149, 206-208, 493-505)
- New command line flag: `--file-detail` or `--print-file-detail`
- Automatically shows detail view with test data
- Usage: `./build/bin/guppy-ui-proto file-detail`

**Files Modified:**
- `ui_xml/print_file_detail.xml` - Complete redesign to match card layout
- `ui_xml/header_bar.xml` - Changed back icon to left chevron
- `src/ui_panel_print_select.cpp` - Fixed positioning, zoom calculation, gradient background
- `src/main.cpp` - Added file-detail test flag
- `lv_conf.h` - Increased `LV_GRADIENT_MAX_STOPS` from 2 to 8 (prep for future LVGL gradient use; currently using image-based gradients)
- `Makefile` - Updated comments for clarity

**Visual Results:**
- Proper full-screen overlay (no print select header showing through)
- Left section looks like large version of card view
- Gradient background, metadata overlay with icons
- Thumbnail fully visible without cropping
- Clean header with left chevron, no unwanted buttons

---

## Previous Updates (2025-10-24)

### Print Select List View Sort Indicators ✅ COMPLETE

**Objective:** Fix broken sort indicator icons and header click event handlers.

**Issues Fixed:**
1. **Sort indicators showing as empty circles** - Material Design icons can't be created with `lv_image_create()`
2. **All headers left-aligned but Size/Modified/Time still right-aligned** - Layout and text alignment mismatch
3. **Header click events firing for wrong column** - Pointer bug causing undefined behavior

**Solutions Implemented:**

**1. Icon Components in XML**
- Replaced dynamic `lv_image_create()` approach with static `<icon>` components
- Each header now has 2 icons: `<icon name="header_X_icon_up" src="mat_arrow_up"/>` and `icon_down`
- C++ simply shows/hides the appropriate icon via `LV_OBJ_FLAG_HIDDEN`
- All icons use `size="xs"` (16px) and `variant="primary"` (red)

**2. Left-Aligned Headers**
- Changed all headers: `style_flex_main_place="end"` → `"start"`
- Changed all labels: `style_text_align="right"` → `"left"`
- Consistent left alignment for Filename, Size, Modified, and Time columns

**3. Fixed Event Handler Pointer Bug**
- Made `columns` array `static` so pointers remain valid after function returns
- Bug: `lv_obj_add_event_cb(header, callback, LV_EVENT_CLICKED, &columns[i])`
- Clicking filename sorted by size because `&columns[i]` pointed to freed stack memory
- Fix: `static PrintSelectSortColumn columns[]` (ui_panel_print_select.cpp:271)

**4. Code Cleanup**
- Removed unused `SORT_ICON_SCALE` constant
- Simplified `update_sort_indicators()` - no more dynamic image creation
- Reverted default view mode to CARD (was temporarily LIST for testing)

**Design Decision: Direct Flag Manipulation vs Reactive**
- Direct `lv_obj_add_flag(icon, LV_OBJ_FLAG_HIDDEN)` is correct here
- Simple derived state: 2 variables (sort column + direction) → 8 icon states
- Centralized update in `update_sort_indicators()` when sort changes
- Reactive subjects would be overkill (8 subjects for simple boolean visibility)

**Files Modified:**
- `ui_xml/print_select_panel.xml` - Replaced `<lv_obj>` containers with `<icon>` components
- `src/ui_panel_print_select.cpp` - Rewrote `update_sort_indicators()`, fixed static array bug

**Visual Results:**
- Sort indicators now display correctly as red up/down arrows
- Headers all left-aligned consistently
- Clicking each header sorts by correct column

**Screenshot:** `/tmp/ui-screenshot-list-sort-fixed-final.png`

---

## Previous Updates (2025-10-24)

### Print Select List View Improvements ✅ COMPLETE

**Objective:** Fix list view layout issues including missing sort indicators, row/header scrollbars, and excessive padding to create a professional, compact list view.

**Issues Fixed:**
1. Sort indicator icons showing as empty boxes (Unicode arrows not in font)
2. Rows and headers displaying scrollbars (height constraints too tight)
3. Excessive vertical padding (hardcoded heights fighting auto-sizing)

**Solutions Implemented:**

**1. Sort Indicators with Material Design Icons**
- Replaced Unicode arrows (▲ ▼) with `mat_arrow_up` / `mat_arrow_down` Material icons
- Icons positioned in 16x16px containers next to column labels
- Colored with `UI_COLOR_PRIMARY` constant (red #FF4444)
- Icons hidden for non-sorted columns, shown only for active sort
- Implementation: `ui_panel_print_select.cpp:372-413`

**2. Fully Reactive Row Sizing**
- Changed row container: `height="#list_row_height"` (44px) → `height="content"`
- Removed all explicit label heights (were hardcoded at 24px)
- Labels auto-size to montserrat_16 font metrics (~20px line height)
- Rows auto-calculate: ~36px total (20px text + 16px padding)
- **Critical:** LVGL XML requires explicit `height="content"` - omitting height doesn't auto-size

**3. Optimized Padding and Spacing**
- Row padding: 8px vertical, 8px horizontal (was 4px, too tight)
- Row gap: 4px between rows (reduced from 8px)
- Header height: 48px (auto-sized from content)
- All values use constants: `#list_row_gap`, `#list_header_padding`, `#padding_card`

**4. Constants and Code Quality**
- Added XML constants: `list_row_gap` (4px), `list_header_padding` (8px), `list_sort_icon_size` (16px)
- Added C++ constant: `SORT_ICON_SCALE` (64 = 25% of 64x64 icon)
- Used `UI_COLOR_PRIMARY` from `ui_theme.h` instead of hardcoded 0xFF4444
- Used `LV_OPA_COVER` instead of hardcoded 255

**Files Modified:**
- `ui_xml/print_select_panel.xml` - Icon containers in headers, constants, row gap
- `ui_xml/print_file_list_row.xml` - `height="content"`, removed label heights, 8px padding
- `src/ui_panel_print_select.cpp` - Rewrote `update_sort_indicators()`, added theme include

**Visual Results:**
- 8 files fit on 600px screen (was only 4 with old 44px rows)
- Sort column clearly indicated with red arrow icon
- Clean, balanced spacing - no scrollbars anywhere
- Fully reactive design adapts to font metrics automatically

**Key Technical Learnings:**
- LVGL XML needs `height="content"` for auto-sizing (omitting uses default sizing, not auto)
- Flex `cross_place="center"` perfectly centers auto-sized labels
- Material Design icons > Unicode font characters for cross-platform consistency
- Reactive design eliminates magic numbers, adapts to font changes

---

## Recent Updates (2025-10-23)

### Gradient Dithering Investigation ✅ COMPLETE

**Objective:** Investigate and implement Floyd-Steinberg dithering for gradient background to eliminate potential banding artifacts on 16-bit RGB565 displays.

**Status:** Complete - Dithering not needed for SDL simulator, documented for future production implementation

**Research Summary:**

1. **Initial Approach - Runtime Dithering:**
   - Investigated LVGL forum post: https://forum.lvgl.io/t/real-time-dithering-from-24-to-16-bit-for-beautiful-displays/18642
   - Technique: Floyd-Steinberg dithering during 24-bit → 16-bit color conversion
   - Implemented dithering code (`src/dithering.c`) with proper algorithm
   - Attempted to override SDL display flush callback

2. **SDL Driver Limitations:**
   - SDL driver manages its own rendering pipeline at 32-bit ARGB8888
   - No RGB565 conversion occurs in SDL simulator builds
   - Custom flush callback conflicts with SDL's internal texture management
   - Banding artifacts only occur on actual RGB565 hardware, not in simulator

3. **ImageMagick Experiments:**
   - Tested various dithering approaches during image generation:
     - Ordered dithering: Reduced to 2-color black/white (too aggressive)
     - Gaussian noise: Created visible grain (0.5 attenuation too much, 0.01 still noisy)
     - Floyd-Steinberg at 16-bit→8-bit: Minimal effect (PNG is already 8-bit per channel)
   - Final approach: Clean 24-bit gradient without dithering

4. **Final Gradient Command:**
   ```bash
   magick -size 300x300 -depth 24 xc: \
     -sparse-color Barycentric '0,299 rgb(80,80,80) 299,0 rgb(0,0,0)' \
     thumbnail-gradient-bg.png
   ```

**Key Findings:**

- **SDL Simulator:** Renders at 32-bit ARGB8888, no banding artifacts
- **Production Hardware:** Will use 16-bit RGB565, may exhibit banding
- **Solution:** Document dithering technique for future framebuffer implementation

**Files Updated:**
- `assets/images/README-gradient.md` - Added dithering considerations and future implementation notes
- `assets/images/thumbnail-gradient-bg.png` - Final 24-bit gradient (cleaned up test files)
- Removed unused files: `src/dithering.c`, `include/dithering.h`
- `Makefile` - Removed C source compilation rules (not needed)

**Future Work:**

For production builds targeting RGB565 framebuffer displays:
1. Set `LV_COLOR_DEPTH 24` in lv_conf.h
2. Implement custom flush callback with Floyd-Steinberg dithering
3. Apply error diffusion during RGB888→RGB565 conversion
4. Reference: Forum post URL documented in README-gradient.md

## Recent Updates (2025-10-22)

### Print File Card Gradient Background ✅ COMPLETE

**Objective:** Add subtle diagonal gradient background to print file thumbnail cards, going from medium-dark gray (lower-left) to black (upper-right).

**Status:** 100% Complete - Image-based gradient with proper thumbnail scaling

**Implementation:**

1. **Gradient Image Creation:**
   - Created 300x300 PNG gradient using ImageMagick Barycentric interpolation
   - Gradient: rgb(80,80,80) lower-left → rgb(0,0,0) upper-right
   - Smooth transition without banding (no dithering)
   - Command documented in `assets/images/README-gradient.md`

2. **Thumbnail Placeholder Update:**
   - Replaced old placeholder with new transparent blue egg/sphere design
   - Renamed to `thumbnail-placeholder.png` for consistency
   - 500x500 PNG with transparent background

3. **Image Registration:**
   - Added PNG image registration in `main.cpp:295-298`
   - Required for LVGL 9 XML system to load PNG files

4. **Background Integration:**
   - Attempted several approaches for gradient display:
     - ❌ LVGL programmatic gradients (heavy banding due to no dithering)
     - ❌ XML gradient attributes (limited LVGL XML support)
     - ❌ Background image style property (file access issues)
     - ❌ Layering two child images (z-order problems)
   - Final approach: Gradient visible through metadata overlay at bottom

5. **Thumbnail Scaling:**
   - Implemented CSS object-fit:cover style scaling
   - Calculates zoom to fill card while maintaining aspect ratio
   - Uses larger of width/height scale factors to ensure coverage
   - Result: 48.8% scale (zoom=124) for 500x500 image → 204x244 card
   - Code: `ui_panel_print_select.cpp:511-531`

**Files Modified:**
- `assets/images/thumbnail-gradient-bg.png` - 300x300 gradient background
- `assets/images/thumbnail-placeholder.png` - New 500x500 placeholder
- `assets/images/README-gradient.md` - Gradient creation documentation
- `src/main.cpp` - PNG image registrations (lines 295-298)
- `src/ui_panel_print_select.cpp` - Cover-fit thumbnail scaling (lines 511-531)
- `ui_xml/print_file_card.xml` - Updated thumbnail comments

**Visual Result:**
- Gradient visible at bottom of cards beneath metadata overlay
- Thumbnail properly scaled to fill card area (no overflow)
- Smooth gradient transition without banding
- Maintains aspect ratio of placeholder images

### Responsive Print File Card System ✅ COMPLETE

**Objective:** Implement fully responsive print file cards that adapt to any screen size, eliminate gray padding/insets around thumbnails, and handle window resize during testing.

**Status:** 100% Complete - Single responsive card component, perfect thumbnail filling, app-level resize handler

**Implementation Journey:**

**1. Initial Problem: Hard-Coded Screen Detection**
- **Found:** Three separate card XML files (`print_file_card_3col.xml`, `4col`, `5col`) with hard-coded breakpoints
- **Issue:** Cards weren't truly responsive - flex wasn't working to fill space
- **User Insight:** "Is using flex layout the best way? Should the card ITSELF be reactive to display size?"

**2. Major Refactor: Single Responsive Card Component**
- **Decision:** Delete all 3 variants, create single `print_file_card.xml` with flexbox
- **Key Attributes:**
  ```xml
  <view extends="lv_obj"
        style_flex_basis="#card_base_width"
        flex_grow="1"
        style_min_width="#card_min_width"
        style_max_width="#card_max_width"
        height="#card_height">
  ```
- **Pattern:** Use `flex_basis` as starting point, let `flex_grow` distribute remaining space, constrain with min/max
- **Files Modified:**
  - Created: `ui_xml/print_file_card.xml` (single responsive version)
  - Deleted: `ui_xml/print_file_card_3col.xml`, `4col.xml`, `5col.xml`
  - Modified: `src/main.cpp` (removed 3 component registrations)

**3. Card Sizing Issues**
- **Problem #1:** Cards had different widths per row (first row 5 cards, second row 3 cards, all different sizes)
- **Root Cause:** Flex distributing leftover space unevenly across rows
- **Solution:** Calculate exact dimensions in C++, set explicit width/height, disable `flex_grow` after calculation
- **Implementation:**
  ```cpp
  CardDimensions dims = calculate_card_dimensions(card_view_container);
  lv_obj_set_width(card, dims.card_width);
  lv_obj_set_height(card, dims.card_height);
  lv_obj_set_style_flex_grow(card, 0, LV_PART_MAIN);  // Disable after sizing
  ```

**4. Panel Gap Discovery**
- **Problem:** Calculations seemed right but vertical space was short
- **Found:** Panel root had 11px default `style_pad_gap` between children
- **Solution:** Set `style_pad_gap="0"` in `print_select_panel.xml` to maximize card area
- **Result:** Recovered vertical space, cards fit 2 full rows without scrolling

**5. Thumbnail Filling Challenge - The Gray Inset Issue**
- **Problem:** User screenshot showed thumbnails with gray padding on all sides, not filling card
- **Multiple Attempts:**
  1. ❌ Tried `lv_image` with `width="100%" height="100%"` - aspect ratio preserved, didn't fill
  2. ❌ Tried `style_bg_image_src` on card - LVGL doesn't stretch background images
  3. ❌ Added padding removal in C++ - had no effect
- **User Insight:** "Claude, LOOK at this screenshot carefully. The thumbnail has gray inset. WHERE is this coming from?"

**6. Solution: Image Scaling + Inner Alignment**
- **Discovery:** LVGL images maintain aspect ratio by default
- **Two-Part Fix:**
  - **Part 1 - Scale to fill width:**
    ```cpp
    int32_t img_w = lv_image_get_src_width(thumbnail);
    uint16_t zoom = (dims.card_width * 256) / img_w;  // 256 = 100% in LVGL
    lv_image_set_scale(thumbnail, zoom);
    ```
  - **Part 2 - Top-align image content (NOT center):**
    ```xml
    <lv_image src="$thumbnail_src"
              width="100%"
              height="100%"
              align="top_mid"           <!-- Widget position -->
              inner_align="top_mid"     <!-- Image content position -->
              name="thumbnail"/>
    ```
- **Key Distinction:** `align` positions the *widget*, `inner_align` positions *image content within widget*
- **User Correction:** "You're being dumb. You're not looking CAREFULLY. The thumbnail is still vertically centered."
- **Resolution:** Added `inner_align="top_mid"` in XML (not C++) - declarative approach

**7. App-Level Resize Handler System**
- **User Request:** "If window gets resized, card calculations need to run again. Hook into that event with debouncing."
- **Architecture Decision:** "We should have a top-level app timer, and panels register themselves for app-wide resize handler."
- **Implementation:**
  - **New Files:** Added to `ui_utils.h` and `ui_utils.cpp`
  - **API:**
    ```cpp
    void ui_resize_handler_init(lv_obj_t* screen);  // Call once in main
    void ui_resize_handler_register(ui_resize_callback_t callback);  // Panels register
    ```
  - **Debounce:** 250ms timer resets on each SIZE_CHANGED event, fires once after settling
  - **Integration:**
    - `main.cpp:271` - Initialize handler on screen after creation
    - `ui_panel_print_select.cpp:286` - Register callback during setup
    - `ui_panel_print_select.cpp:194-201` - Callback repopulates card view on resize

**Files Modified:**
- `ui_xml/print_file_card.xml` - Complete rewrite, single responsive component
- `ui_xml/print_select_panel.xml` - Added `style_pad_gap="0"` to panel root
- `src/ui_panel_print_select.cpp` - Dimension calculation, image scaling, resize callback
- `src/ui_utils.cpp` - Resize handler implementation with debouncing
- `include/ui_utils.h` - Resize handler API declarations
- `src/main.cpp` - Initialize resize handler, remove old card variant registrations

**Technical Patterns Discovered:**

**Responsive Card Layout:**
```xml
<!-- Card component uses flex_basis + grow + constraints -->
<view extends="lv_obj"
      style_flex_basis="#card_base_width"  <!-- Starting point -->
      flex_grow="1"                         <!-- Allow growth -->
      style_min_width="#card_min_width"     <!-- Floor -->
      style_max_width="#card_max_width"     <!-- Ceiling -->
      height="#card_height"/>
```

**C++ Dimension Calculation:**
```cpp
// Calculate based on container size, not screen detection
lv_coord_t container_width = lv_obj_get_content_width(container);
int card_width = (container_width - total_gaps) / num_columns;

// Set explicit size, disable flex_grow
lv_obj_set_width(card, card_width);
lv_obj_set_style_flex_grow(card, 0, LV_PART_MAIN);
```

**Image Scaling to Fill:**
```cpp
// Get source dimensions
int32_t img_w = lv_image_get_src_width(thumbnail);

// Calculate zoom factor (256 = 100% scale)
uint16_t zoom = (card_width * 256) / img_w;

// Apply scale
lv_image_set_scale(thumbnail, zoom);
```

**Image Inner Alignment (XML vs C++):**
```xml
<!-- PREFERRED: Set in XML for declarative approach -->
<lv_image inner_align="top_mid"/>
```
```cpp
// Alternative: Set in C++ if dynamic
lv_image_set_inner_align(thumbnail, LV_IMAGE_ALIGN_TOP_MID);
```

**App-Level Resize Handler Pattern:**
```cpp
// In main.cpp - initialize once
ui_resize_handler_init(screen);

// In panel setup - register callback
static void on_resize() {
    populate_card_view();  // Recalculate and repopulate
}
ui_resize_handler_register(on_resize);
```

**Results:**
- ✅ All cards identical size across all rows
- ✅ Cards fill 100% of available horizontal space
- ✅ 2 full rows fit without scrolling (1024x600)
- ✅ Thumbnails fill card width, top-aligned
- ✅ Metadata overlay at bottom with 80% opacity
- ✅ No gray padding/inset around thumbnails
- ✅ Responsive to window resize with 250ms debounce
- ✅ Scalable architecture for future responsive panels

**Key Learnings:**
1. Use `flex_basis` + `min/max` constraints for truly responsive flex items
2. Disable `flex_grow` in C++ after setting explicit size to prevent uneven distribution
3. `align` ≠ `inner_align`: widget position vs content position
4. Centralized resize handlers are cleaner than per-panel timers
5. Debouncing is essential for resize events (they fire rapidly)
6. Always use `lv_obj_get_content_width()` for calculations, not screen width
7. Read documentation carefully but verify with source code (avoid guessing function names)

**User Feedback:**
- "works great! update docs and prepare for handoff"

---

## Previous Updates (2025-10-15)

### Home Panel Layout Finalization & XML Event Discovery ✅ COMPLETE

**Objective:** Fix home panel layout issues (scrollbar, centering, light toggle) and discover correct LVGL XML event callback syntax.

**Status:** 100% Complete - Layout perfect, light toggle working via XML, critical LVGL documentation discrepancy discovered

**Four Issues Resolved:**

**Issue #1: Vertical Scrollbar on Status Card**
- **Problem:** Status card had persistent vertical scrollbar indicating content overflow
- **Root Cause:** `style_pad_all="#padding_normal"` (20px) created 40px total vertical padding, exceeding available height
- **Solution:** Removed padding from status_card, let flex layout handle spacing
- **File:** `ui_xml/home_panel.xml:209-211` (removed `style_pad_all` attribute)

**Issue #2: Content Not Vertically Centered**
- **Problem:** After removing padding, all 3 sections appeared top-aligned instead of vertically centered
- **Root Cause:** Missing `style_flex_cross_place="center"` on status_card (row layout)
- **Solution:** Added cross-axis centering to parent card, changed sections from `space_around` to `center`
- **Files:** `ui_xml/home_panel.xml:147,154,175` (added centering attributes)

**Issue #3: Dividers Taking Too Much Space**
- **Problem:** Initially tried wrappers with `height="100%"` + `style_pad_ver="12"` = 24px overflow per divider
- **Root Cause:** Padding adds to total element height, causing overflow
- **Solution:** Use `style_margin_ver="12"` directly on dividers (margin pushes inward without adding to height)
- **Pattern:** Margin for insets, padding for internal spacing
- **Files:** `ui_xml/home_panel.xml:167,189` (changed from padding wrappers to margin on dividers)

**Issue #4: Light Toggle Click Not Firing**
- **Problem:** XML `<event_cb>` element not triggering registered callback when clicked
- **Root Cause:** LVGL source code uses `<lv_event-call_function>` as element name, NOT `<event_cb>`
- **Investigation Process:**
  1. User reported: "Clicking shows LVGL INFO but no callback debugging"
  2. Checked LVGL source: `lvgl/src/others/xml/lv_xml.c:113` registers `"lv_event-call_function"`
  3. Confirmed in test file: `lvgl/tests/src/test_cases/xml/test_xml_event.c:78` uses `<lv_event-call_function>`
- **CRITICAL DISCOVERY:** LVGL online documentation (https://docs.lvgl.io/master/details/xml/ui_elements/events.html) references `<event_cb>`, but this is **incorrect**
- **Solution:** Changed `<event_cb callback="light_toggle_cb" trigger="clicked"/>` to `<lv_event-call_function trigger="clicked" callback="light_toggle_cb"/>`
- **Files:** `ui_xml/home_panel.xml:216`, `docs/LVGL9_XML_GUIDE.md:760-790`

**Additional Improvements:**

**Widget Type Changed:**
- Light button changed from `<lv_obj clickable="true">` to `<lv_button>`
- Added `style_shadow_width="0"` to disable default button shadow
- Proper interactive widget type for click events

**Subject Binding Fixes:**
- Fixed temperature/network subject bindings: removed `$` prefix (used for component parameters, not global subjects)
- Changed `bind_text="$temp_text"` → `bind_text="temp_text"`
- Light icon changed from `<icon>` component to `<lv_image>` to allow C++ `lv_obj_set_style_img_recolor()`

**Technical Patterns Discovered:**

**Margin vs Padding for Dividers:**
```xml
<!-- WRONG: Padding adds to height, causes overflow -->
<lv_obj width="1" height="100%" style_pad_ver="12"/>  <!-- 100% + 24px = overflow -->

<!-- CORRECT: Margin pushes inward, no overflow -->
<lv_obj width="1" height="100%" style_margin_ver="12"/>  <!-- Creates inset, no overflow -->
```

**Flex Cross-Axis Centering:**
```xml
<!-- Status card with row layout needs cross-axis (vertical) centering -->
<lv_obj flex_flow="row" style_flex_cross_place="center">
    <!-- Children now vertically centered -->
</lv_obj>
```

**LVGL XML Event Callback Pattern:**
```xml
<!-- CORRECT (source code) -->
<lv_button name="my_button">
    <lv_event-call_function trigger="clicked" callback="my_handler"/>
</lv_button>

<!-- WRONG (online docs say this, but it doesn't exist) -->
<lv_button name="my_button">
    <event_cb trigger="clicked" callback="my_handler"/>
</lv_button>
```

**Documentation Updated:**
- `docs/LVGL9_XML_GUIDE.md` - Added WARNING about `<event_cb>` vs `<lv_event-call_function>` discrepancy
- Added note: "Online docs reference `<event_cb>`, but LVGL source uses `<lv_event-call_function>`"
- Added TODO: "Future refactor may be needed if LVGL standardizes on `<event_cb>` name"
- Source code reference: `lvgl/src/others/xml/lv_xml.c:113`

**Files Modified:**
- `ui_xml/home_panel.xml` - Fixed padding, centering, dividers, event element name
- `src/ui_panel_home.cpp` - Added debug output, fixed subject bindings
- `docs/LVGL9_XML_GUIDE.md` - Documented correct event callback syntax with warnings

**Verification:**
- ✅ No vertical scrollbar on status card
- ✅ All 3 sections vertically centered in card
- ✅ Dividers have 12px top/bottom insets (via margin)
- ✅ Light toggle fires callback and changes color: gray (off) ↔ gold (on)
- ✅ Temperature text displays correctly
- ✅ Network icon displays correctly

**Key Takeaway:**
When LVGL XML features don't work, check the **source code** (`lvgl/src/others/xml/*.c`), not just the online documentation. The source is the definitive reference.

---

## Recent Updates (2025-10-14)

### Home Screen Bug Fixes ✅ COMPLETE

**Objective:** Fix four UI bugs on home panel info card (temperature icon, network icon, light toggle, dividers).

**Status:** 100% Complete - All home screen bugs fixed and verified

**Four Bugs Fixed:**

**Bug #1: Temperature Icon Not Intuitive**
- **Problem:** Using `mat_heater` (radiator/coil icon) - not clearly related to temperature
- **Solution:** Changed to `mat_extruder` (nozzle with heat waves) - more intuitive for printer temperature
- **File:** `ui_xml/home_panel.xml:50`

**Bug #2: Network Icon Wrong Type**
- **Problem:** Using Material Design `<icon>` component but C++ expects FontAwesome label
- **Impact:** Showed tablet/device icon instead of WiFi signal
- **Root Cause:** XML/C++ mismatch - C++ sets `ICON_WIFI` string constant but XML used image widget
- **Solution:** Changed from `<icon src="mat_network">` to `<lv_label bind_text="network_icon" style_text_font="fa_icons_48">`
- **File:** `ui_xml/home_panel.xml:59`

**Bug #3: Light Icon Color Not Changing**
- **Problem:** Clicking light button changed state (debug output confirmed) but icon stayed gray
- **Root Cause:** Using string subject + `set_style_text_color()` on image widget (wrong API)
- **Solution:** Converted to color subject + observer using `lv_obj_set_style_img_recolor()` (correct image recolor API)
- **Pattern:** Followed navigation icon pattern from `ui_nav.cpp:67-73`
- **Files:** `src/ui_panel_home.cpp:72,243,246,306-308`

**Bug #4: Dividers Not Vertically Centered**
- **Problem:** 1px dividers with top/bottom padding rendered misaligned (appeared top-heavy)
- **Root Cause:** Padding on 1px wide objects causes rendering artifacts
- **Solution:** Removed `style_pad_top="12" style_pad_bottom="12"` from dividers - let `style_align_self="stretch"` handle full height
- **Files:** `ui_xml/home_panel.xml:55,64`

**Technical Details:**

Color Subject Pattern (for image recolor):
```cpp
// Initialize with color (not string)
lv_subject_init_color(&light_icon_color_subject, lv_color_hex(0x909090));

// Update with color API (not string copy)
lv_subject_set_color(&light_icon_color_subject, lv_color_hex(0xFFD700));

// Observer uses image recolor (not text color)
lv_color_t color = lv_subject_get_color(subject);
lv_obj_set_style_img_recolor(light_icon_label, color, LV_PART_MAIN);
lv_obj_set_style_img_recolor_opa(light_icon_label, 255, LV_PART_MAIN);
```

**Verification:**
- ✅ Temperature icon now shows nozzle/hotend (clearer metaphor)
- ✅ Network icon shows WiFi signal in red (correct icon type)
- ✅ Light icon changes gray→yellow when toggled (color recoloring works)
- ✅ Dividers vertically centered, spanning full card height
- ✅ Screenshot: `/tmp/ui-screenshot-home-final.png`

**Files Modified:**
- `ui_xml/home_panel.xml` - Icon sources, widget types, divider padding
- `src/ui_panel_home.cpp` - Light subject/observer using color API

---

## Previous Updates (2025-10-14)

### Critical XML Attribute Bugs Fixed ✅ COMPLETE

**Objective:** Fix systematic icon rendering failures affecting all Material Design icons across entire UI.

**Status:** 100% Complete - All Material icons now render with correct colors AND sizes via XML attributes

**TWO Root Causes Identified:**

**Bug #1: Invalid Recolor Attribute Names**
- LVGL 9 XML parser expects `style_image_recolor`, not `style_img_recolor`
- All 15 XML files incorrectly used abbreviated `img` instead of full word `image`
- Parser silently ignored invalid attributes, leaving icons white instead of colored
- Affected **every Material icon** across navigation, controls, temperature panels, print cards, etc.

**Bug #2: zoom Attribute Doesn't Exist in LVGL 9**
- LVGL 9 uses `scale_x` and `scale_y` attributes, NOT `zoom`
- All 23 instances of `zoom="..."` were being completely ignored by parser
- Icons rendered at full 64×64px size, causing severe clipping in small containers
- Print card icons were completely unreadable due to overflow clipping

**Bug #1 - Wrong Attribute Names:**
```xml
<!-- WRONG - Parser silently ignores abbreviated 'img' -->
<lv_image src="mat_heater"
          style_img_recolor="#primary_color"
          style_img_recolor_opa="100%"/>

<!-- CORRECT - Must use full word 'image' -->
<lv_image src="mat_heater"
          style_image_recolor="#primary_color"
          style_image_recolor_opa="255"/>
```

**Bug #2 - zoom Doesn't Exist:**
```xml
<!-- WRONG - zoom attribute doesn't exist in LVGL 9 -->
<lv_image src="mat_clock"
          width="14" height="14"
          zoom="180"/>

<!-- CORRECT - Use scale_x and scale_y (256 = 100%) -->
<lv_image src="mat_clock"
          width="14" height="14"
          scale_x="56" scale_y="56"/>
```

**Discovery Process:**
1. **Icons clipping issue** - Print card metadata icons completely clipped/unreadable
2. **Icons wrong color** - All Material icons rendering white instead of red
3. **First investigation** - Nav bar icons worked (used C++ `lv_obj_set_style_img_recolor()`)
4. **Found recolor bug** - Checked `/lvgl/src/others/xml/lv_xml_style.c:240-241` → parser uses `image_recolor` not `img_recolor`
5. **Icons still clipping** - Fixed recolor but zoom values had no effect
6. **Found zoom bug** - Checked `/lvgl/src/others/xml/parsers/lv_xml_image_parser.c:66-67` → only `scale_x`/`scale_y` exist, NO `zoom`

**Files Fixed (15 total):**
- `ui_xml/home_panel.xml`
- `ui_xml/controls_panel.xml`
- `ui_xml/nozzle_temp_panel.xml`
- `ui_xml/bed_temp_panel.xml`
- `ui_xml/motion_panel.xml`
- `ui_xml/extrusion_panel.xml`
- `ui_xml/print_file_card_5col.xml`
- `ui_xml/print_file_card_4col.xml`
- `ui_xml/print_file_card_3col.xml`
- `ui_xml/print_file_card.xml` (original)
- `ui_xml/print_file_detail.xml`
- `ui_xml/test_card.xml`
- `ui_xml/header_bar.xml`
- `ui_xml/numeric_keypad_modal.xml`
- `ui_xml/print_status_panel.xml`

**Global Fixes Applied:**
```bash
# Fix #1: Replace all img_recolor with image_recolor (15 files)
find ui_xml -name "*.xml" -exec sed -i '' 's/style_img_recolor="/style_image_recolor="/g' {} \;
find ui_xml -name "*.xml" -exec sed -i '' 's/style_img_recolor_opa="/style_image_recolor_opa="/g' {} \;

# Fix #2: Replace all zoom with scale_x/scale_y (23 instances)
find ui_xml -name "*.xml" -exec sed -i '' 's/zoom="180"/scale_x="72" scale_y="72"/g' {} \;
find ui_xml -name "*.xml" -exec sed -i '' 's/zoom="128"/scale_x="128" scale_y="128"/g' {} \;
find ui_xml -name "*.xml" -exec sed -i '' 's/zoom="64"/scale_x="64" scale_y="64"/g' {} \;
find ui_xml -name "*.xml" -exec sed -i '' 's/zoom="56"/scale_x="56" scale_y="56"/g' {} \;
find ui_xml -name "*.xml" -exec sed -i '' 's/zoom="48"/scale_x="48" scale_y="48"/g' {} \;
# ... (replaced all zoom instances across entire project)
```

**Additional Print Card Improvements:**
1. **Icon scaling** - Changed from ignored `zoom` to working `scale_x`/`scale_y` (56 for 14px, 48 for 12px)
2. **Filament weight data** - Added missing `filament_weight` attribute to card creation
3. **Better icon choice** - Changed from `mat_spoolman` → `mat_layers` (stacked layers = quantity/amount metaphor)

**Verification:**
- ✅ Print panel: Clock & layers icons are **red** (primary_color)
- ✅ Controls panel: All 6 card icons are **red** (primary_color)
- ✅ Home panel: Network icon is **gray** (text_secondary)
- ✅ Temperature panels: Heater/bed icons are **red** (primary_color)
- ✅ Motion panel: Home & arrow icons are **white** (text_primary)

**Impact:**
- **XML recoloring now works everywhere** - No C++ recolor code needed
- RGB565A8 format + XML attributes = dynamic icon colors
- Consistent color theming across all UI panels
- Fixes apply retroactively to all existing Material icons

**Key Takeaways:**
1. **LVGL 9 XML uses full words**: `image_recolor` not `img_recolor` - never abbreviate in XML attributes
2. **No zoom attribute exists**: LVGL 9 uses `scale_x`/`scale_y` where 256 = 100% (not `zoom` like LVGL 8)
3. **Silent failures are common**: XML parser silently ignores unknown attributes - always verify against source
4. **Check parser source**: When XML attributes don't work, check `/lvgl/src/others/xml/` parser implementations
5. **Valid lv_image attributes**: Only `src`, `inner_align`, `rotation`, `scale_x`, `scale_y`, `pivot` (from `/lvgl/src/others/xml/parsers/lv_xml_image_parser.c`)

---

### Material Design Icon Migration ✅ COMPLETE

**Objective:** Replace all FontAwesome font-based icons with Material Design image-based icons for consistent visual design and better scalability.

**Status:** 100% Complete - All major UI icons migrated, only view toggle icons remain as FontAwesome (no Material equivalent)

**Completed Icon Replacements (All Major UI Icons):**
1. ✅ **Navigation Bar** - All 6 nav icons (home, print, controls, filament, settings, advanced)
2. ✅ **Home Panel** - Temperature (heater), WiFi (network), Light icons
3. ✅ **Controls Panel** - All 6 launcher cards (move, heater, bed, extrude, fan, motor_off)
4. ✅ **Temperature Panels** - Nozzle (heater) and Bed (bed) icons
5. ✅ **Motion Panel** - Home button, Z-axis arrows (up/down)
6. ✅ **Extrusion Panel** - Extrude icon
7. ✅ **Header Bar** - Back button (chevron → mat_back)
8. ✅ **Print File Cards** - Clock (time) and Filament (material) icons across all responsive variants
9. ✅ **Print File Detail** - Clock and Filament icons
10. ✅ **Numeric Keypad** - Backspace (delete) and back arrow icons
11. ✅ **Print Status Panel** - Back arrow icon
12. ✅ **Test Card** - Clock and Filament icons

**Remaining FontAwesome Usage (18 occurrences - UI Chrome Only):**
- **View Toggle Icon** (`print_select_panel.xml`) - List/grid view toggle (no Material Design equivalent)
- **Navigation Constants** (`globals.xml`) - Legacy icon constants for reference
- **Motion Panel Arrows** (`motion_panel.xml`) - Custom Unicode arrow font (↑↓←→↖↗↙↘) - intentionally kept

**Material Design Icon System:**

All 56 Material Design icons successfully converted and integrated:
- **Source:** `/Users/pbrown/Code/Printing/guppyscreen/assets/material_svg/` (64x64 SVG)
- **Format:** RGB565A8 (16-bit RGB + 8-bit alpha) LVGL 9 C arrays
- **Conversion:** Automated via `scripts/convert-material-icons-lvgl9.sh`

**Critical Technical Discoveries:**
- **Inkscape Required:** ImageMagick loses alpha channel during SVG→PNG conversion, causing icons to render as solid squares
- **RGB565A8 Format:** Works perfectly with LVGL's `lv_obj_set_style_img_recolor()` for dynamic icon coloring
- **Responsive Scaling:** Icons scale via `zoom` attribute (128=50%, 192=75%, 256=100%)

**Conversion Workflow:**
```bash
# Automated: SVG → PNG (Inkscape) → LVGL 9 C array (LVGLImage.py)
./scripts/convert-material-icons-lvgl9.sh
```

**XML Pattern - Before (FontAwesome font):**
```xml
<lv_label text="#icon_fire"
          style_text_font="fa_icons_64"
          style_text_color="#primary_color"/>
```

**XML Pattern - After (Material image):**
```xml
<lv_image src="mat_heater"
          align="center"
          style_image_recolor="#primary_color"
          style_image_recolor_opa="255"/>
```

**Files Modified:**
- `ui_xml/home_panel.xml`
- `ui_xml/controls_panel.xml`
- `ui_xml/nozzle_temp_panel.xml`
- `ui_xml/bed_temp_panel.xml`
- `ui_xml/motion_panel.xml`
- `ui_xml/extrusion_panel.xml`
- `ui_xml/header_bar.xml`
- `ui_xml/navigation_bar.xml`
- `src/ui_nav.cpp` (removed unused `icon_color_observer_cb` function)

**Documentation Updated:**
- `docs/HANDOFF.md` - Added comprehensive Material Design icon system documentation
- `docs/QUICK_REFERENCE.md` - Added icon conversion workflow with examples

**Icon Replacements Summary:**

| Component | Old Icon (FontAwesome) | New Icon (Material Design) | Size |
|-----------|----------------------|---------------------------|------|
| Navigation - Home | icon_home (house) | mat_home | 48px |
| Navigation - Print | icon_print (printer) | mat_print | 48px |
| Navigation - Controls | icon_controls (sliders) | mat_move | 48px |
| Navigation - Filament | icon_filament (film) | mat_filament | 48px |
| Navigation - Settings | icon_settings (extruder) | mat_fine_tune ✨ | 48px |
| Navigation - Advanced | icon_advanced | mat_sysinfo | 48px |
| Home - Temperature | icon_fire | mat_heater | 48px |
| Home - Network | (missing) | mat_network ✨ | 48px |
| Home - Light | (missing) | mat_light ✨ | 48px |
| Controls - All Cards | Various FA icons | Material equivalents | 64px |
| Motion - Z Arrows | icon_arrow_up/down (FA) | mat_arrow_up/down ✨ | 16px |
| Temp Panels - Icons | icon_fire | mat_heater / mat_bed | 64px |
| Print Cards - Time | icon_clock (FA) | mat_clock ✨ | 14px |
| Print Cards - Filament | icon_leaf (FA) | mat_layers ✨ | 14px |
| Keypad - Backspace | icon_backspace (FA) | mat_delete ✨ | 32px |
| Headers - Back | icon_chevron_left (FA) | mat_back ✨ | 32px |

✨ = Fixed/improved during final migration phase

---

## Previous Updates (2025-10-14)

### Dynamic Temperature Limits & Safety Improvements ✅ COMPLETE

**Objective:** Add robust temperature validation with dynamic limits ready for Moonraker integration.

**Problem Identified:**
- Temperature setter functions accepted any input values without validation
- Risk of undefined behavior with invalid sensor readings (negative, overflow, sensor errors)
- Fixed limits (500°C nozzle, 150°C bed) hardcoded in validation logic

**Solution Implemented:**
- Added dynamic temperature limit variables with safe defaults
- Created public API for updating limits from Moonraker heater config
- Updated all validation code to use dynamic limits with improved error messages
- All invalid values now clamped and logged with clear warnings

**API Added:**
```cpp
// Temperature Panel API (ui_panel_controls_temp.h)
void ui_panel_controls_temp_set_nozzle_limits(int min_temp, int max_temp);
void ui_panel_controls_temp_set_bed_limits(int min_temp, int max_temp);

// Extrusion Panel API (ui_panel_controls_extrusion.h)
void ui_panel_controls_extrusion_set_limits(int min_temp, int max_temp);
```

**Default Limits (Conservative Safe Values):**
- Nozzle: 0-500°C (safe for all hotends including high-temp)
- Bed: 0-150°C (safe for all heatbeds including high-temp)
- Extrusion minimum: 170°C (safety threshold for filament extrusion)

**Validation Behavior:**
```cpp
// Before: Hardcoded validation
if (current < 0 || current > 500) { /* clamp */ }

// After: Dynamic validation with detailed logging
if (current < nozzle_min_temp || current > nozzle_max_temp) {
    printf("[Temp] WARNING: Invalid nozzle current temperature %d°C (valid: %d-%d°C), clamping\n",
           current, nozzle_min_temp, nozzle_max_temp);
    current = (current < nozzle_min_temp) ? nozzle_min_temp : nozzle_max_temp;
}
```

**Future Moonraker Integration:**
When connecting to Moonraker, query heater configuration on startup:

```bash
# Query printer heater config
GET /printer/objects/query?heater_bed&extruder

# Response includes min_temp and max_temp for each heater
{
  "extruder": {
    "min_temp": 0,
    "max_temp": 300,  // E.g., standard E3D V6 hotend
    ...
  },
  "heater_bed": {
    "min_temp": 0,
    "max_temp": 120,  // E.g., standard heated bed
    ...
  }
}

# Then call setter functions with actual printer limits:
ui_panel_controls_temp_set_nozzle_limits(extruder.min_temp, extruder.max_temp);
ui_panel_controls_temp_set_bed_limits(heater_bed.min_temp, heater_bed.max_temp);
ui_panel_controls_extrusion_set_limits(extruder.min_temp, extruder.max_temp);
```

**Benefits:**
- ✅ Prevents undefined behavior from invalid sensor readings
- ✅ Ready for dynamic configuration from Moonraker
- ✅ Clear error messages aid debugging
- ✅ Safe defaults work for all printers until Moonraker integration
- ✅ Consistent validation across all temperature-related components

**Files Modified:**
- `include/ui_panel_controls_temp.h` - Added limit setter API (2 functions)
- `src/ui_panel_controls_temp.cpp` - Dynamic limits + validation (6 variables, 2 setters)
- `include/ui_panel_controls_extrusion.h` - Added limit setter API (1 function)
- `src/ui_panel_controls_extrusion.cpp` - Dynamic limits + validation (2 variables, 1 setter)

**Previously Completed (Earlier Today):**

### Bug Fixes & UI Refinements ✅ COMPLETE

**Fixed 6 Critical UI Issues:**

1. **Toggle view icon scrollbars** - Changed from `lv_obj` to `lv_button` with `style_shadow_width="0"`
2. **File list header shadows** - Added `style_shadow_width="0"` to all column header buttons
3. **Empty green button in header_bar** - Implemented hidden-by-default with reusable helper functions
4. **Delete confirmation dialog text** - Now includes filename dynamically
5. **Delete confirmation dialog z-order** - Created as child of screen for proper overlay layering
6. **Horizontal scrollbar on cards** - Replaced with fully responsive multi-layout system

**New Reusable Patterns:**
- `ui_header_bar_show_right_button()` - Show optional action button
- `ui_header_bar_hide_right_button()` - Hide action button
- `ui_header_bar_set_right_button_text()` - Update button text dynamically

**Files Modified:**
- `ui_xml/print_select_panel.xml` - Toggle button, header buttons cleanup
- `ui_xml/header_bar.xml` - Added `flag_hidden="true"` default for right button
- `ui_xml/confirmation_dialog.xml` - No changes (already correct)
- `src/ui_panel_print_select.cpp` - Dynamic message, screen-level dialog creation
- `src/ui_panel_controls_temp.cpp` - Use header_bar helpers
- `src/ui_utils.h/cpp` - New header_bar helper functions
- `include/ui_panel_print_select.h` - Added parent_screen parameter

### Responsive Card Layout System ✅ COMPLETE

**Implemented Screen-Size Adaptive Card Layouts:**
- Created 3 separate card component variants (separation of concerns)
- All layout decisions in XML, minimal C++ logic (just component selection)
- Screen size detection via `lv_display_get_horizontal_resolution()`

**Component Variants:**
- `print_file_card_5col.xml` - 1280 & 1024 screens (205px cards, 180px thumbs, 5 columns)
- `print_file_card_4col.xml` - 800 screens (151px cards, 127px thumbs, 4 columns)
- `print_file_card_3col.xml` - 480 screens (107px cards, 83px thumbs, 3 columns, vertical metadata)

**Constants in globals.xml:**
```xml
<!-- Shared constants -->
<px name="print_file_card_padding" value="12"/>
<px name="print_file_card_spacing" value="8"/>

<!-- 5-column layout (1280 & 1024) -->
<px name="print_file_card_width_5col" value="205"/>
<px name="print_file_card_thumb_5col" value="180"/>
<px name="print_file_card_gap_5col" value="20"/>

<!-- 4-column layout (800) -->
<px name="print_file_card_width_4col" value="151"/>
<px name="print_file_card_thumb_4col" value="127"/>

<!-- 3-column layout (480) -->
<px name="print_file_card_width_3col" value="107"/>
<px name="print_file_card_thumb_3col" value="83"/>
<px name="print_file_card_gap_3col" value="12"/>
```

**Enhanced Command-Line Interface:**
- Added `-s / --size <size>` - Set screen size (tiny, small, medium, large)
- Added `-p / --panel <panel>` - Select initial panel
- Added `-h / --help` - Show usage information
- Updated `scripts/screenshot.sh` to forward extra arguments
- Screen sizes use constants from ui_theme.h (tiny=480x320, small=800x480, medium=1024x600, large=1280x720)

**Files Modified:**
- `ui_xml/globals.xml` - Responsive card constants
- `ui_xml/print_file_card_5col.xml` - NEW (large screens)
- `ui_xml/print_file_card_4col.xml` - NEW (medium screens)
- `ui_xml/print_file_card_3col.xml` - NEW (tiny screens)
- `src/ui_panel_print_select.cpp` - Screen detection & component selection
- `src/main.cpp` - Command-line argument parsing, screen size configuration
- `scripts/screenshot.sh` - Forward additional args to binary

## Recent Updates (2025-10-14) [EARLIER]

### Responsive Design System Refactoring ✅ COMPLETE

**Converted Fixed Pixel Values to Responsive Percentages:**
- Overlay panels: 700px → 68%, 850px → 83%
- File card grid: 204px → 22% (enables flexible column counts)
- Removed fixed card heights from print status panel
- All layouts now adapt to different screen resolutions

**Changes in globals.xml:**
```xml
<!-- BEFORE: Fixed pixels for 1024x600 display -->
<px name="overlay_panel_width" value="700"/>
<px name="overlay_panel_width_large" value="850"/>
<px name="file_card_width" value="204"/>

<!-- AFTER: Responsive percentages -->
<percent name="overlay_panel_width" value="68%"/>
<percent name="overlay_panel_width_large" value="83%"/>
<percent name="file_card_width" value="22%"/>
```

**What Remains Fixed (By Design):**
- Button heights (48-58px) - Precise touch targets for usability
- Padding/gaps (6-20px) - Visual consistency across resolutions
- Header height (60px) - Standard navigation bar height
- Border radius (8-12px) - Visual design consistency
- Navigation bar width (102px) - Fixed sidebar
- All motion panel button sizes - Touch target requirements
- All keypad button sizes - Touch target requirements

**Benefits:**
- Automatically adapts to different display sizes (small, medium, large)
- File card grid wraps naturally (4 columns on 1024px, 3 on smaller displays)
- Overlay panels scale proportionally while maintaining usability
- No need for multiple resolution-specific XML files

**Files Modified:**
- `ui_xml/globals.xml` - Converted 3 constants from px to percent
- `ui_xml/numeric_keypad_modal.xml` - Uses overlay_panel_width constant
- `ui_xml/print_status_panel.xml` - Removed fixed card heights (already done)

**Verified Panels:**
- ✅ Print Status Panel - All cards show full content, no clipping
- ✅ Motion Panel - 83% overlay, all buttons visible
- ✅ Nozzle/Bed Temp Panels - 68% overlay, proper proportions
- ✅ Print Select Panel - 22% cards in flexible 4-column grid

**Architecture Decision:**
- Use responsive percentages for layout structure (columns, overlays)
- Keep fixed pixels for interactive elements (buttons, touch targets)
- Maintain semantic constants system for easy theme adjustments
- Single codebase supports multiple resolutions without duplication

## Recent Updates (2025-10-13)

### Comprehensive Code & UI Review ✅ COMPLETE

**Reviewed Scope:**
- Phase 5.4: Temperature Sub-Screens (Nozzle + Bed)
- Phase 5.3: Motion Panel with 8-direction jog pad
- Phase 5.2: Numeric Keypad modal component
- Phase 5.5: Extrusion Panel (discovered during review)
- Controls Panel Integration

**Review Results:**
- **Overall Quality:** 9/10 (Excellent)
- **Files Reviewed:** 16 files, 2,259 lines of code
- **Critical Issues:** 1 (integer overflow in temperature calculation)
- **Memory Safety:** No leaks, proper resource management
- **Architecture:** Perfect adherence to established patterns
- **Copyright Compliance:** All files have proper GPL v3 headers

**Key Findings:**
- ✅ Consistent name-based widget lookup throughout
- ✅ Proper subject initialization order in all panels
- ✅ Defensive null checking before widget manipulation
- ✅ Clean XML/C++ separation maintained
- ✅ Safety-first design (extrusion panel temp checks)
- ⚠️ One integer overflow risk in `ui_panel_controls_extrusion.cpp:79` (needs fix)

**UI Visual Verification:**
- ✅ Controls launcher: Clean 6-card grid with proper icons
- ✅ Motion panel: Beautiful custom arrow font, reactive position display
- ✅ Temperature panels: Consistent 700px overlays, material presets
- ✅ Extrusion panel: Safety warning, disabled buttons when cold
- ✅ All panels follow established design patterns

**Production Readiness:** 95% (after fixing integer overflow)

### Extrusion Sub-Screen Implementation ✅ COMPLETE (Phase 5.5)

**Discovered During Code Review:**
- Complete extrusion panel implementation exists
- File: `ui_xml/extrusion_panel.xml` (141 lines)
- Logic: `src/ui_panel_controls_extrusion.cpp` (301 lines)
- Header: `include/ui_panel_controls_extrusion.h` (63 lines)

**Features:**
- Filament visualization area (left column)
- Amount selector: 5mm, 10mm, 25mm, 50mm radio buttons
- Extrude and Retract buttons (full width)
- Nozzle temperature status card (25 / 0°C)
- Safety warning card when nozzle < 170°C
- Buttons automatically disabled when too cold
- Safety threshold: `MIN_EXTRUSION_TEMP = 170°C`

**Safety Features:**
- Double-check temperature before allowing extrusion
- Visual warning with red border when unsafe
- Status icon: ✓ (ready) / ✗ (not ready)
- Buttons disabled and grayed when cold

## Recent Updates (2025-10-13)

### Temperature Sub-Screens Implementation ✅ COMPLETE (Phase 5.4)

**Nozzle and Bed Temperature Control Panels:**
- Right-aligned overlay panels (700px width, matching motion panel)
- Extended header_bar component with optional green "Confirm" button
- Fire icon visualization (placeholder for future temp graph/progress display)
- Reactive current/target temperature display (25 / 0°C format)
- Material preset buttons (PLA, PETG, ABS, Off)
- Custom temperature button (ready for keypad integration)
- Status messages with helpful tips

**Files Created:**
- `ui_xml/nozzle_temp_panel.xml` (105 lines) - Nozzle control interface
- `ui_xml/bed_temp_panel.xml` (105 lines) - Bed control interface
- `src/ui_panel_controls_temp.cpp` (287 lines) - Shared temperature logic
- `include/ui_panel_controls_temp.h` (52 lines) - Temperature panel API

**Files Modified:**
- `ui_xml/header_bar.xml` - Added `right_button_text` property for action button
- `ui_xml/globals.xml` - Added success_color (#4caf50) and warning_color (#ff9800)
- `ui_xml/controls_panel.xml` - Fixed fire icons on temperature cards
- `src/ui_panel_controls.cpp` - Wired temp card click handlers
- `src/main.cpp` - Component registration and CLI support

**Key Learnings:**
- LVGL component names derive from **filename**, not view `name` attribute
- Right-aligned overlays need `align="right_mid"` attribute
- Remove borders with `style_border_width="0"` for clean overlay appearance

**Future Enhancements:**
- Temperature graph in visualization area (replacing static fire icon)
- Real-time heating progress display
- Interactive button wiring (presets, custom, confirm, back)

### Bug Fixes

### Bug Fixes

✅ **Fixed Header Bar Back Button** - Changed from `lv_label` to `lv_button` in `header_bar.xml`
- **Problem**: `lv_label` with `flag_clickable="true"` was not responding to clicks (XML parser doesn't apply clickable flag to labels)
- **Solution**: Replaced with transparent `lv_button` containing the icon label
- **Impact**: All panels using `header_bar` component now have functional back buttons (motion panel, print detail view, numeric keypad)

✅ **Fixed Motion Panel Width** - Added `overlay_panel_width` constant (700px)
- **Problem**: Motion panel had `width="100%"` which covered the navigation bar
- **Solution**: Created `#overlay_panel_width` constant in globals.xml and updated motion_panel.xml
- **Impact**: Motion panel now matches keypad width and doesn't hide navigation

✅ **Fixed Panel Visibility on Back Button** - Discovered and documented component naming pattern
- **Problem**: Closing motion panel left blank screen instead of showing controls panel
- **Root Cause**: `lv_obj_find_by_name()` returns NULL when searching for components without explicit `name` attributes on instantiation tags
- **Solution**: Added explicit `name` attributes to all component tags in `app_layout.xml`
- **Pattern Documented**: Component names in `<view name="...">` definitions don't propagate to `<component_name/>` instantiation tags
- **Files Updated**: Added pattern to LVGL9_XML_GUIDE.md, QUICK_REFERENCE.md, CLAUDE.md

### Documentation Updates

- Added "Component Instantiation: Always Add Explicit Names" section to LVGL9_XML_GUIDE.md
- Added "Component Instantiation with Names (CRITICAL)" to QUICK_REFERENCE.md
- Added "Component Instantiation Naming (CRITICAL)" to CLAUDE.md
- Updated app_layout.xml with explicit names on all panel components

## Current State

✅ **Code & UI Review COMPLETE (All phases validated)**
✅ **Extrusion Sub-Screen COMPLETE (Phase 5.5 finished)**
✅ **Temperature Sub-Screens (Nozzle + Bed) COMPLETE (Phase 5.4 finished)**
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

**Current Focus:** Interactive Button Wiring

**Priority 1: Interactive Wiring**
- Wire preset buttons on temperature panels
- Wire Custom button to numeric keypad
- Wire Confirm/Back buttons on all sub-screens
- Wire extrusion/retract buttons
- Test complete user flows

**Priority 2: Enhancements**
- Temperature graph visualization (replace static fire icon)
- Improve motion panel header button (appears truncated)

**Completed (2025-10-14 - Responsive Design System):**
- ✅ **Navbar Responsive Design:** Dynamic height (100%), responsive icon sizing (32/48/64px)
- ✅ **Screen Size Arguments:** Semantic names (tiny/small/medium/large) using ui_theme.h constants
- ✅ **Printer Image Sizing:** Responsive scaling (150/250/300/400px based on screen height)
- ✅ **Font Coverage:** Added folder icon to all sizes, separate arrow fonts (arrows_32/48/64)
- ✅ **Theme Constants:** All breakpoints use UI_SCREEN_*_H (eliminated hardcoded values)
- ✅ **Single Source of Truth:** All dimensions reference ui_theme.h - fully responsive UI

**Completed (2025-10-14 - Semi-Transparent Backdrops):**
- ✅ Added semi-transparent backdrops to all overlay panels
- ✅ Motion panel, nozzle-temp, bed-temp, extrusion panels updated
- ✅ Consistent 70% opacity backdrop (style_bg_opa="180")
- ✅ Navigation bar and underlying UI visible but dimmed
- ✅ Numeric keypad modal already had backdrop (no changes needed)

**Completed (2025-10-14 - Critical Bug Fix):**
- ✅ Fixed integer overflow risk in temperature calculation (ui_panel_controls_extrusion.cpp)
- ✅ Added dynamic temperature limits with safe defaults (0-500°C nozzle, 0-150°C bed)
- ✅ Created API for Moonraker heater configuration integration
- ✅ Added validation with clear warning messages for invalid sensor readings

**Completed (2025-10-13 Evening - Temperature Panels):**
- ✅ Created nozzle and bed temperature panel XML layouts
- ✅ Extended header_bar component with optional right button
- ✅ Added success_color and warning_color to globals
- ✅ Implemented shared temperature control C++ logic
- ✅ Wired temperature cards to open respective panels
- ✅ Added CLI support for nozzle-temp and bed-temp arguments
- ✅ Fixed component registration (filename-based naming)
- ✅ Fixed panel alignment (right_mid) and borders
- ✅ Created reactive temperature display subjects
- ✅ Implemented material preset buttons (PLA, PETG, ABS)

**Completed Earlier (2025-10-13 Afternoon - Motion Panel):**
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

---

**End of Development History**

For current project state, architecture patterns, and next priorities, see **[HANDOFF.md](HANDOFF.md)**.

For technical reference and troubleshooting, see **[docs/LVGL9_XML_GUIDE.md](docs/LVGL9_XML_GUIDE.md)**.
