# GuppyScreen UI Redesign Project

## Project Overview

**Goal:** Modernize GuppyScreen's user interface by taking inspiration from leading 3D printer UIs while maintaining its unique identity and Klipper-specific functionality.

**Status:** Research Phase

**Approach:**
1. Research and analyze reference UIs
2. Document design patterns and best practices
3. Plan screen/panel mapping (current vs. new)
4. Create UI mockups
5. Implement incrementally without breaking existing functionality
6. Integrate with existing Moonraker/Klipper communication layer

---

## Reference UIs

### Primary Inspiration
- **Bambu Lab X1C** - Industry-leading modern UI (primary reference)

### Secondary References
- **Creality K1** - Good UI with useful patterns
- **FlashForge Adventurer 5M Pro** - Stock screen reference

---

## Research Notes

### Bambu Lab X1C UI

**Hardware Specs:**
- 5-inch 1280×720 high-resolution touch screen
- Current firmware: 01.10.00.00 (latest as of Oct 2025)
- Major UI redesign introduced in firmware 01.09.00.00 (April 2025)
- UI technology: QML-based interface

**Screenshots & Resources:**
- Official Wiki: https://wiki.bambulab.com/en/x1/manual/screen-operation
- Forum discussion on 01.09 UI: https://forum.bambulab.com/t/absolutely-love-the-look-of-the-new-ui-01-09-00-00/165327
- Firmware downloads: https://bambulab.com/en/support/firmware-download/x1
- Custom firmware (X1Plus): https://github.com/X1Plus/X1Plus

**Key Features Observed:**
- Real-time print monitoring with live camera feed
- Model preview with thumbnails
- Print history viewing
- Temperature controls and status
- Filament management (AMS integration)
- Settings and calibration tools
- Remote control capability via app/PC

**Design Patterns:**
- **Layout approach:** Major redesign in 01.09 matched UI to H2D printer design
- **Typography:** Bolder fonts (reported by users as significantly improved)
- **Visual elements:** Larger thumbnails, more prominent information display
- **Navigation:** Improved menu navigation, "snappier" performance
- **Information density:** Better use of screen real estate with bigger UI elements

**User Feedback on Redesign:**
- Positive: "Everything feels snappier, thumbnails bigger, fonts bolder, nicer menu navigation"
- Mixed reception: Some users loved it, others reported issues/preferred old UI
- Notable changes: Runtime counter moved from main display to Settings menu
- Developer mode appears to have been removed in 01.09

**Functionality:**
- Complete printer control from touchscreen
- Live print monitoring and adjustment
- File selection and management
- Calibration workflows
- Settings configuration
- LAN-only mode available (privacy option)

**Notes for GuppyScreen:**
- This is the PRIMARY reference for our redesign
- Focus on the improved visual hierarchy (bigger thumbnails, bolder fonts)
- Study the navigation improvements and "snappier" interface feel
- Consider the balance between information density and clarity

---

### Creality K1 UI

**Hardware Specs:**
- 4.3-inch color touchscreen
- Resolution: 480×400 pixels
- Stock Creality display-server software

**Screenshots & Resources:**
- Detailed review with photos: https://3dwithus.com/creality-k1-review-3d-printer-testing-settings-and-tips
- Additional review: https://clevercreations.org/creality-k1-3d-printer-review-specs/
- K1 Max review: https://3dwithus.com/creality-k1-max-review-3d-printer-testing
- Official store page: https://store.creality.com/products/k1-max-screen-kit

**Key Features Observed:**
- Model preview with thumbnail icons
- Print history viewing
- Basic calibration options (can be enabled/disabled per print)
- File browsing from printer storage or USB
- Model preview icons make file selection straightforward
- Very limited live print adjustment capabilities

**Design Patterns:**
- **Layout approach:** Simple, clear, easy-to-navigate interface following Creality's heritage
- **Design philosophy:** Prioritizes simplicity and ease of use for beginners/intermediate users
- **Information density:** Straightforward presentation, no clutter
- **Navigation:** Everything needed for setup, calibration, and printing accessible from menus
- **User guidance:** Interface guides users through entire printing process

**User Feedback:**
- "Very well thought out, intuitive, and user-friendly"
- "Simple, clear, and easy to navigate"
- "Great addition for new users" (guided workflow)
- Criticism: "Basic and occasionally unresponsive"
- Limitation: Cannot adjust deeper print parameters on the fly

**Functionality:**
- File selection and preview
- Print history
- Basic calibration workflows
- Temperature monitoring (limited control)
- Print progress monitoring
- Limited live adjustments (unlike GuppyScreen's capabilities)

**Limitations:**
- Cannot adjust print settings like speed directly on screen
- Recommended to use Creality Print software or app for detailed settings
- Less advanced than Bambu Lab's interface

**Notes for GuppyScreen:**
- Good example of simplified, beginner-friendly approach
- Shows importance of guided workflows for new users
- Demonstrates that "simple and clear" is valued even if less feature-rich
- GuppyScreen should maintain its advanced features while improving accessibility

---

### FlashForge AD5M Pro UI

**Hardware Specs:**
- 4.3-inch responsive touchscreen
- Full color LCD HD screen
- Stock FlashForge interface

**Screenshots & Resources:**
- Official product page: https://www.flashforge.com/products/adventurer-5m-pro-3d-printer
- User manual: https://www.flashforge.com/blogs/download-document/adventurer-5m-pro
- Wiki documentation: https://wiki.flashforge.com/en/adventurer-series/manual
- Custom firmware mod: https://github.com/DrA1ex/ff5m (Forge-X mod)

**Key Features Observed:**
- Model preview functionality
- Print history viewing
- Progress tracking during prints
- Auto-leveling integration
- Firmware updates via touchscreen
- Smart features built-in

**Design Patterns:**
- **Design philosophy:** Simplified touchscreen to guide users through all hurdles
- **User experience:** Guides users through process step-by-step
- **Approach:** Focus on ease of use and reducing complexity

**Functionality:**
- Model preview and selection
- Print progress monitoring
- Print history access
- Auto-leveling controls
- Firmware update management
- Basic printer controls

**Research Status:**
- ⚠️ **Limited detailed information available** compared to Bambu and Creality
- Few public screenshots or detailed UI reviews found
- Custom firmware (Forge-X) exists, suggesting stock UI has room for improvement
- Will need to search for video reviews or user-submitted photos for more details

**Notes for GuppyScreen:**
- Less critical as secondary reference
- Focus research efforts on Bambu X1C and Creality K1 for now
- May revisit if more visual resources become available

---

## Current GuppyScreen Panels/Screens

### Existing Panels (from src/)
- `bedmesh_panel.cpp` - 3D bed mesh visualization
- `console_panel.cpp` - Console/shell interface
- `finetune_panel.cpp` - Live print adjustments
- `files_panel.cpp` - G-code file browser
- `limits_panel.cpp` - Velocity/acceleration limits
- `macros_panel.cpp` - Macro execution
- `main_panel.cpp` - Main screen
- `numpad_panel.cpp` - Numeric input
- `print_status_panel.cpp` - Active print status
- `printer_select_panel.cpp` - Multi-printer selection
- `settings_panel.cpp` - System settings
- `spoolman_panel.cpp` - Filament management
- `sysinfo_panel.cpp` - System information
- `temp_panel.cpp` - Temperature control
- `tune_panel.cpp` - Print tuning
- `wifi_panel.cpp` - Wi-Fi configuration
- `zcalibrate_panel.cpp` - Z-offset calibration

### Additional Functionality (containers/widgets)
- `belt_calibration_container.cpp`
- `input_shaper_container.cpp`
- `shaper_graph_container.cpp`
- `tmc_status_container.cpp`
- `sensor_container.cpp`
- `power_device_container.cpp`

---

## Screen/Panel Mapping

### Keep As-Is
- Moonraker WebSocket/API communication layer
- [To be determined based on research]

### Enhance/Redesign
- [To be determined based on research]

### New Screens/Panels
- [To be determined based on research]

### Remove/Deprecate
- [To be determined based on research]

---

## Design Decisions Log

### 2025-10-07 - Initial Project Scoping
- Decided to research Bambu X1C, Creality K1, FlashForge AD5M Pro UIs
- Approach: Design-first, then implement incrementally
- Keep: Moonraker/Klipper communication layer
- Open for redesign: Everything else (UI, layouts, themes, panels)

### 2025-10-07 - Initial Research Findings
- Bambu X1C (firmware 01.09+) identified as PRIMARY reference
  - Key takeaways: Bigger thumbnails, bolder fonts, snappier navigation, improved visual hierarchy
- Creality K1 shows good beginner-friendly approach
  - Key takeaways: Simplicity, guided workflows, clear navigation
- FlashForge AD5M Pro has limited public information
  - Will need video reviews for more details
- Challenge: Difficult to find actual UI screenshots online, may need to look at YouTube walkthroughs

---

## Implementation Plan

*To be created after research phase*

### Phase 1: Research & Analysis
- [x] Initial web research for Bambu X1C UI (firmware 01.09/01.10)
- [x] Initial web research for Creality K1 UI
- [x] Initial web research for FlashForge AD5M Pro UI
- [ ] **NEXT:** Find YouTube video walkthroughs with actual UI screenshots
  - Search for: "Bambu Lab X1C screen menu walkthrough" on YouTube
  - Search for: "Creality K1 touchscreen tutorial" on YouTube
  - Search for: "FlashForge Adventurer 5M Pro screen interface" on YouTube
- [ ] Take screenshots from video walkthroughs of key screens:
  - Main/home screen
  - Print status/monitoring screen
  - File browser/selection screen
  - Settings/configuration screens
  - Temperature control screens
  - Any calibration/tuning screens
- [ ] Analyze design patterns across all three UIs
- [ ] Document functionality comparison table
- [ ] Create design recommendations document

### Phase 2: Design & Mockups
- [ ] Define new screen/panel structure
- [ ] Create UI mockups
- [ ] Define color schemes and icon styles
- [ ] Get feedback and approval

### Phase 3: Implementation
- [ ] Create new UI components
- [ ] Integrate with existing functionality
- [ ] Test on simulator
- [ ] Test on hardware

### Phase 4: Refinement
- [ ] User testing
- [ ] Polish and bug fixes
- [ ] Documentation updates

---

## Questions & Decisions Needed

### Open Questions
- **Screenshot gathering strategy:** Should we focus on YouTube videos for visual references?
- **Screen resolution target:** What resolution(s) should we optimize for? (480x320, 800x480, 1024x600?)
- **Color scheme:** Dark theme? Light theme? Both? Match current material/zbolt themes?
- **Icon style:** Stick with Material Design Icons or explore alternatives?
- **Navigation pattern:** Bottom nav? Side nav? Card-based? Tab-based?

### Screenshot Collection Strategy

**Directory Structure Created:**
- `docs/ui-research/` with subdirectories for each printer
- Organized by screen type: main-screen, print-status, file-browser, settings, temperature, calibration, misc
- See `docs/ui-research/README.md` for organization guidelines
- See `docs/ui-research/SOURCES.md` for all source links found

**Sources Identified:**

1. **Bambu Lab X1C:**
   - Official PDF: https://cdn1.bambulab.com/documentation/Quick Start Guide for X1-Carbon Combo.pdf
   - Wiki: https://wiki.bambulab.com/en/x1/manual/screen-operation
   - Reviews: Tom's Hardware, The Gadgeteer (2025)
   - Need: YouTube videos with screen walkthroughs

2. **Creality K1:**
   - ✅ 3DWithUs Review (confirmed screenshots): https://3dwithus.com/creality-k1-review-3d-printer-testing-settings-and-tips
   - Tom's Hardware K1 Review
   - HowToMechatronics K1 Max Review
   - Need: Official manual screenshots

3. **FlashForge AD5M Pro:**
   - Official manual (72 pages): https://www.manua.ls/flashforge/adventurer-5m-pro/manual
   - Anton Mansson video review: https://www.antonmansson.com/videos/flashforge-adventurer-5m-pro-review-video
   - Need: Extract screenshots from video reviews

**Next Manual Steps (requires downloading/viewing):**
1. Download PDF manuals and extract UI screenshots
2. Watch YouTube video reviews and capture key frames showing UI
3. Visit review sites and save embedded photos
4. Organize screenshots into directory structure
5. Document source and firmware version for each image

### Next Session Tasks
1. ~~Search YouTube for video walkthroughs~~ ✅ Sources documented
2. Download/capture screenshots from identified sources (manual step)
3. Begin comparative analysis document
4. Start sketching potential layouts for GuppyScreen redesign

---

## Resources

### Design Tools
- [To be added]

### Icon Sets
- Material Design Icons (current)
- Z-Bolt Icons (current)
- [New icon sets to consider]

### Color Palettes
- [To be added based on research]

---

---

## Research Summary (Current Session)

**What We Learned:**
1. **Bambu X1C** (PRIMARY reference):
   - Recent major UI overhaul in firmware 01.09 (April 2025)
   - Focus: Bigger, bolder, clearer - improved visual hierarchy
   - 5-inch 1280x720 display, QML-based interface
   - Users loved: Snappier feel, larger thumbnails, bolder fonts, better navigation

2. **Creality K1** (SECONDARY reference):
   - Simple, beginner-friendly approach
   - 4.3-inch 480x400 display
   - Key strength: Guided workflows, clear navigation
   - Limitation: Basic features, limited live adjustments

3. **FlashForge AD5M Pro** (TERTIARY reference):
   - 4.3-inch touchscreen
   - Limited public documentation
   - Needs more research via video reviews

**Key Design Principles Identified:**
- ✅ Bigger thumbnails improve file selection
- ✅ Bolder fonts improve readability
- ✅ Snappy/responsive UI is critical for user satisfaction
- ✅ Clear visual hierarchy helps navigation
- ✅ Guided workflows benefit new users
- ✅ Balance simplicity with advanced features (GuppyScreen's strength)

**Next Steps:**
- Find YouTube video walkthroughs for actual UI screenshots
- Create comparative screenshot collection
- Analyze navigation patterns
- Begin mockup planning

---

*Last Updated: 2025-10-07*
*Research Status: Phase 1 - Initial Web Research Complete*
