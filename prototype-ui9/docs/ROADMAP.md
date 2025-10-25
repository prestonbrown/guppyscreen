# LVGL 9 XML UI Prototype - Development Roadmap

## ✅ Phase 1: Foundation (COMPLETED)

- [x] LVGL 9.3 setup with XML support
- [x] Navigation bar with proper flex layout
- [x] Home panel with declarative XML
- [x] Reactive data binding (Subject-Observer pattern)
- [x] Theme system with global constants
- [x] FontAwesome icon integration with auto-generation
- [x] Screenshot utility supporting multiple binaries
- [x] Comprehensive documentation
- [x] C++ wrappers for XML panels with clean API
- [x] LVGL logging integration

## ✅ Phase 2: Navigation & Blank Panels (COMPLETED)

**Priority: High** - Enable panel switching with reactive navbar highlighting

- [x] **Navigation State Management**
  - Active panel tracking via integer subject
  - Navbar button click handlers (C++ event handlers)
  - Panel visibility toggling (show/hide based on active state)
  - Reactive navbar icon highlighting (red #ff4444 for active, white #ffffff for inactive)

- [x] **Blank Placeholder Panels**
  - Home panel (with printer status content)
  - Controls panel (blank placeholder)
  - Filament panel (blank placeholder)
  - Settings panel (blank placeholder)
  - Advanced panel (blank placeholder)

- [x] **Navigation Implementation**
  - Updated navigation_bar.xml with FontAwesome icons
  - Created C++ wrapper (ui_nav.cpp) with Subject-Observer pattern
  - Panel switching via clickable navigation icons
  - Icon color updates based on active panel

**Key Learnings:**
- Never call `SDL_PollEvent()` manually - violates LVGL display driver abstraction
- Must create mouse input device with `lv_sdl_mouse_create()` for clicks
- Event handlers in C++ with `lv_obj_add_event_cb()` for `LV_EVENT_CLICKED`
- Labels must have `LV_OBJ_FLAG_EVENT_BUBBLE` and not be clickable for clicks to reach buttons

## 📋 Phase 3: Print Select Panel Core (COMPLETED)

**Status: COMPLETE** - Card view with detail overlay working

- [x] **Print Select Panel** - File browser with dynamic cards
  - ✅ Dynamic XML card component system (print_file_card.xml)
  - ✅ 4-column grid layout (204×280px cards optimized for 1024×800)
  - ✅ Thumbnail display with centering (180×180px images)
  - ✅ Filename truncation with ellipsis
  - ✅ Mock print file data (30 test files)
  - ✅ Utility functions for time/filament formatting
  - ✅ Metadata labels with icons (clock + time, leaf + weight)
  - ✅ Clickable cards opening detail overlay
  - ✅ Full-screen detail view with large thumbnail, options, action buttons
  - ✅ Filament type dropdown (11 filament types)
  - ✅ Automatic bed leveling checkbox
  - ✅ Delete + Print action buttons

**Completed:** 2025-10-12 Evening

---

## ✅ Phase 4: Print Select Panel Polish (COMPLETED)

**Priority: High** - List view, sorting, empty state, confirmations

- [x] **View Modes**
  - [x] View toggle button (icon-only: fa-list ↔ fa-th-large)
  - [x] List view with sortable table layout
  - [x] Instant toggle between card and list views (show/hide containers)

- [x] **Sortable List View**
  - [x] Column headers simplified to 4 columns: Filename, Size, Modified, Print Time
  - [x] Click header to sort ascending/descending
  - [x] Visual sort indicators (▲/▼ arrows in header labels)
  - [x] Default sort: Filename, ascending
  - [x] Toggle direction on same column, switch to ascending on new column
  - [x] Reactive re-rendering with std::sort() and update functions

- [x] **Empty State**
  - [x] Display when no files available
  - [x] "No files available for printing" message (montserrat_20)
  - [x] "Upload gcode files to get started" secondary text (montserrat_16)
  - [x] Centered in content area (400×200px container)
  - [x] Shared between card and list views

- [x] **Confirmation Dialogs**
  - [x] Reusable confirmation dialog XML component (confirmation_dialog.xml)
  - [x] Semi-transparent backdrop overlay (#000000 opacity 180)
  - [x] 400×200px centered dialog card
  - [x] API properties for dynamic title and message
  - [x] Cancel + Confirm buttons (160px each)
  - [x] Click backdrop or Cancel to dismiss
  - [x] Confirm triggers callback function

- [x] **Icon System**
  - [x] Added ICON_LIST and ICON_TH_LARGE to ui_fonts.h
  - [x] Generated #icon_list and #icon_th_large in globals.xml
  - [x] Updated generate-icon-consts.py script

- [x] **Utility Functions**
  - [x] format_file_size() - Converts bytes to "1.2 MB" format
  - [x] format_modified_date() - Formats timestamps to "Jan 15 14:30"

**Completed:** 2025-10-12 Evening

**Key Decisions:**
- Simplified list view from 7 columns to 4 (removed Thumbnail, Filament, Slicer for clarity)
- View toggle shows opposite mode icon (intuitive: shows where you'll go, not where you are)
- Default view: Card mode (visual preference)
- Default sort: Filename ascending (alphabetical)
- Per-session view preference (remembered until restart)

---

## 🎮 Phase 5: Controls Panel (IN PROGRESS)

**Priority: High** - Manual printer control with Bambu X1C-style sub-screens

**Status:** Launcher Complete (Phase 1), Sub-screens Pending

### Phase 1: Launcher Panel ✅ COMPLETE (2025-10-12 Night)

- [x] **6-Card Launcher Menu**
  - [x] 2×3 grid layout (400×200px cards)
  - [x] Proper flex row_wrap for card wrapping
  - [x] Vertical scrolling for overflow
  - [x] Card icons + titles + subtitles
  - [x] Click handlers for all 6 cards
  - [x] C++ integration (ui_panel_controls.cpp/h)
  - [x] Fan Control card dimmed as "Coming soon" placeholder

- [x] **Design Specification**
  - [x] 70-page comprehensive UI design document (controls-panel-v1.md)
  - [x] Complete sub-screen mockups and specifications
  - [x] Component file structure defined
  - [x] API patterns documented

- [x] **Icon System Updates**
  - [x] Added 10 new icon constants to ui_fonts.h
  - [x] Regenerated globals.xml with 27 total icons
  - [x] Used existing fa_icons_64 glyphs as placeholders

**Completed:** 2025-10-12 Night

### Phase 2: Reusable Components ✅ COMPLETE (2025-10-12 Late Night)

- [x] **Numeric Keypad Modal Component**
  - [x] XML component (numeric_keypad_modal.xml) with backdrop + modal
  - [x] 700px width, right-docked design
  - [x] Header bar: back button + dynamic title + OK button
  - [x] Input display with large font (montserrat_48) + unit label
  - [x] 3×4 button grid: [7][8][9], [4][5][6], [1][2][3], [EMPTY][0][BACKSPACE]
  - [x] C++ wrapper (ui_component_keypad.cpp/h) with callback API
  - [x] Event handlers for all buttons (digits, backspace, OK, cancel)
  - [x] Input validation (min/max clamping)
  - [x] Backdrop click to dismiss
  - [x] Single reusable instance pattern
  - [x] FontAwesome backspace icon (U+F55A) in fa_icons_32
  - [x] Font regeneration workflow with lv_font_conv

**Files Created:**
- `ui_xml/numeric_keypad_modal.xml` - Full modal component
- `src/ui_component_keypad.cpp` - Implementation with state management
- `include/ui_component_keypad.h` - Public API with config struct
- `package.json` - Updated font ranges for backspace icon

**Files Modified:**
- `ui_xml/globals.xml` - Added keypad dimension and color constants
- `assets/fonts/fa_icons_32.c` - Regenerated with backspace icon
- `src/main.cpp` - Keypad initialization and test demo

**Completed:** 2025-10-12 Late Night

---

### Phase 3: Motion Sub-Screen (NEXT)

- [ ] **Movement Controls**
  - [ ] Button grid jog pad (3×3 grid with center home)
  - [ ] Z-axis buttons (↑10, ↑1, ↓1, ↓10)
  - [ ] Distance selector (0.1mm, 1mm, 10mm, 100mm radio buttons)
  - [ ] Real-time position display (X/Y/Z with reactive subjects)
  - [ ] Home buttons (All, X, Y, Z)
  - [ ] Back button navigation

### Phase 4: Temperature Sub-Screens (PENDING)

- [ ] **Nozzle Temperature Screen**
  - [ ] Current/Target temperature display
  - [ ] Preset buttons (Off, PLA 210°, PETG 240°, ABS 250°)
  - [ ] Custom button opens numeric keypad
  - [ ] Nozzle visualization graphic
  - [ ] Confirm button with green styling
  - [ ] Back button navigation

- [ ] **Heatbed Temperature Screen**
  - [ ] Current/Target temperature display
  - [ ] Preset buttons (Off, PLA 60°, PETG 80°, ABS 100°)
  - [ ] Custom button opens numeric keypad
  - [ ] Bed visualization graphic
  - [ ] Confirm button with green styling
  - [ ] Back button navigation

### Phase 5: Extrusion Sub-Screen (PENDING)

- [ ] **Extrude/Retract Controls**
  - [ ] Amount selector (5mm, 10mm, 25mm, 50mm radio buttons)
  - [ ] Extrude button (green, icon + text)
  - [ ] Retract button (orange, icon + text)
  - [ ] Temperature status card (nozzle temp with checkmark/warning)
  - [ ] Safety warning when nozzle < 170°C
  - [ ] Extruder visualization graphic
  - [ ] Back button navigation

### Phase 6: Advanced Features (FUTURE)

- [ ] **Fan Control Sub-Screen**
  - [ ] Part cooling fan slider (0-100%)
  - [ ] Preset buttons (Off, 50%, 100%)
  - [ ] Current speed display

- [ ] **Motors Disable**
  - [ ] Confirmation dialog
  - [ ] Send M84 command
  - [ ] Visual feedback

**Design Reference:** Based on Bambu Lab X1C controls system with sub-screens

---

## 📱 Phase 6: Additional Panel Content (FUTURE)

**Priority: Medium** - Build out remaining panel functionality

- [ ] **Storage Source Tabs** (Print Select Panel)
  - Internal/SD card tab navigation
  - Tab bar in panel header
  - Reactive tab switching
  - File list updates per storage source

- [~] **Print Status Panel** - Active print monitoring ✅ Logic / ⚠️ UI needs redesign
  - ✅ Progress bar with mock simulation (0-100%)
  - ✅ Print time/remaining time with reactive updates
  - ✅ Bed/nozzle temperatures with reactive updates
  - ✅ Pause/Resume/Cancel/Tune buttons (functional)
  - ✅ Wired to Print button on file detail view
  - ⚠️ **UI layout broken** - overlapping elements, poor spacing (Priority 1)

- [ ] **Filament Panel** - Filament management
  - Load/unload controls
  - Filament profiles
  - Color/material selection

- [ ] **Settings Panel** - Configuration
  - Network settings
  - Display settings (brightness, theme)
  - Printer settings
  - System info

- [ ] **Advanced/Tools Panel** - Advanced features
  - Bed mesh visualization
  - Console/logs
  - File manager
  - System controls

## 🎨 Phase 6: Enhanced UI Components (FUTURE)

**Priority: Medium** - Richer interactions and reusable widgets

- [ ] **Integer Subjects for Numeric Displays**
  - Progress bars with `bind_value`
  - Sliders with bi-directional binding
  - Arc/gauge widgets

- [ ] **Custom Widgets as XML Components**
  - Temperature display widget
  - Print progress widget
  - Status badge component

- [ ] **Additional Modal Patterns**
  - Error message dialogs
  - Loading indicators
  - Toast/snackbar notifications

- [ ] **Lists and Scrolling**
  - Virtual scrolling for large lists (100+ items)
  - Settings menu items
  - Log viewer with auto-scroll

## 🔗 Phase 7: Panel Transitions & Polish (FUTURE)

**Priority: Low** - Visual refinement and animations

- [ ] **Panel Transitions**
  - Fade in/out animations
  - Slide animations
  - Proper cleanup when switching panels

- [ ] **Button Feedback**
  - Navbar button press effects
  - Hover states (if applicable)
  - Ripple effects

- [ ] **View Transitions**
  - Smooth card ↔ list view transitions
  - Detail overlay fade-in/fade-out
  - Dialog animations

## 🔌 Phase 8: Backend Integration (FUTURE)

**Priority: Medium** - Connect to Klipper/Moonraker

- [ ] **WebSocket Client for Moonraker**
  - Connection management
  - JSON-RPC protocol
  - Reconnection logic

- [ ] **Printer State Management**
  - Poll printer status
  - Update subjects from printer data
  - Handle state changes

- [ ] **File Operations**
  - List print files
  - Upload files
  - Delete files
  - Start prints

- [ ] **Real-time Updates**
  - Temperature monitoring
  - Print progress
  - Status changes

## 🎭 Phase 9: Theming & Accessibility (FUTURE)

**Priority: Low** - Visual refinement

- [ ] **Theme Variants**
  - Light mode
  - High contrast mode
  - Custom color schemes

- [ ] **Responsive Layouts**
  - Support multiple resolutions
  - Orientation changes
  - Scaling for different DPI

- [ ] **Animations**
  - Button press effects
  - Panel transitions
  - Loading animations
  - Success/error feedback

- [ ] **Accessibility**
  - Larger touch targets
  - High contrast options
  - Status announcements

## 🧪 Phase 10: Testing & Optimization (ONGOING)

**Priority: Ongoing** - Quality assurance

- [ ] **Memory Profiling**
  - Check for leaks
  - Optimize subject usage
  - Profile panel switching

- [ ] **Performance Testing**
  - Frame rate monitoring
  - UI responsiveness
  - Touch latency

- [ ] **Cross-platform Testing**
  - Raspberry Pi target
  - BTT Pad target
  - Different screen sizes

- [ ] **Edge Case Handling**
  - Connection loss
  - Print errors
  - File system errors

## 🚀 Phase 11: Production Readiness (FUTURE)

**Priority: Future** - Deployment prep

- [ ] **Configuration System**
  - Runtime configuration file
  - Printer profiles
  - User preferences

- [ ] **Error Handling**
  - Graceful degradation
  - Error recovery
  - User notifications

- [ ] **Logging System**
  - Structured logging
  - Log levels
  - Log rotation

- [ ] **Build System Improvements**
  - Cross-compilation setup
  - Packaging for targets
  - Install scripts

---

## Current Status

**Active Phase:** Phase 4 - Print Select Panel Polish **COMPLETE**

**Recent Work (2025-10-12 Evening):**

**Phase 4 Completion:**
- ✅ Icon-only view toggle button (40×40px, fa-list ↔ fa-th-large)
- ✅ Dual view modes: Card view (grid) + List view (sortable table)
- ✅ List view with 4-column layout: Filename | Size | Modified | Time
- ✅ Column sorting with visual indicators (▲/▼ arrows)
- ✅ Empty state message ("No files available for printing")
- ✅ Reusable confirmation dialog component (confirmation_dialog.xml)
- ✅ Utility functions: format_file_size(), format_modified_date()
- ✅ Updated all documentation (requirements v2.0, HANDOFF.md, STATUS.md)
- ✅ Added ICON_LIST and ICON_TH_LARGE to icon system

**Phase 3 Completion:**
- ✅ Print Select Panel with dynamic card system fully working
- ✅ 4-column grid layout (204×280px cards) optimized for 1024×800
- ✅ Metadata icons and labels (clock + time, leaf + weight) rendering correctly
- ✅ Full-screen detail view overlay with large thumbnail
- ✅ Options panel (filament type dropdown, bed leveling checkbox)
- ✅ Delete + Print action buttons
- ✅ Click card to open detail, click back/outside to close

**Design Decisions (Phase 4):**
- Simplified list view from 7 columns to 4 (removed Thumbnail, Filament, Slicer)
- View toggle shows opposite mode icon (intuitive UX)
- Default view: Card mode (remembered per session)
- Default sort: Filename ascending

**Next Phase Options:**
1. **Phase 5:** Build out remaining panels (Controls, Filament, Settings, Advanced)
2. **Phase 7:** Moonraker integration (wire Print/Delete buttons to API)
3. **Phase 8:** Backend integration (file operations, real-time updates)

**Completed Phases:** 1, 2, 3, 4

**Phase 4 Completion Date:** 2025-10-12 Evening

---

## Notes

- **Reactive Pattern:** All UI updates should use Subject-Observer pattern
- **XML First:** Prefer XML layout over C++ when possible
- **Clean Separation:** Keep business logic in C++, layout in XML
- **Documentation:** Update guides as patterns emerge
- **UI Patterns:** Document reusable patterns in LVGL9_XML_GUIDE.md (e.g., vertical accent bars, dynamic card components)
- **Responsive Design:** Use constants in globals.xml for easy layout adjustments (4-col vs 3-col grids)
- **Testing:** Test each phase before moving to next

---

**Last Updated:** 2025-10-12
