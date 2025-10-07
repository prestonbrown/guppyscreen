# UI Research Screenshots

This directory contains reference screenshots from leading 3D printer UIs used for GuppyScreen redesign research.

## Directory Structure

```
ui-research/
├── bambu-x1c/           # Bambu Lab X1C (firmware 01.09+) - PRIMARY REFERENCE
│   ├── main-screen/     # Home/dashboard screens
│   ├── print-status/    # Active print monitoring
│   ├── file-browser/    # File selection and preview
│   ├── settings/        # Configuration menus
│   ├── temperature/     # Temperature control screens
│   ├── calibration/     # Calibration workflows
│   └── misc/            # Other UI elements
├── creality-k1/         # Creality K1 - SECONDARY REFERENCE
│   ├── main-screen/
│   ├── print-status/
│   ├── file-browser/
│   ├── settings/
│   ├── temperature/
│   ├── calibration/
│   └── misc/
└── flashforge-ad5m-pro/ # FlashForge Adventurer 5M Pro - TERTIARY REFERENCE
    ├── main-screen/
    ├── print-status/
    ├── file-browser/
    ├── settings/
    ├── temperature/
    ├── calibration/
    └── misc/
```

## Screenshot Guidelines

When adding screenshots:

1. **Naming Convention:** `[screen-type]-[number].jpg` or `[screen-type]-[description].jpg`
   - Examples: `main-01.jpg`, `print-status-with-camera.jpg`

2. **Source Documentation:** Create a `SOURCES.md` file in each printer directory with:
   - URL/source of each screenshot
   - Date captured
   - Firmware version (if known)
   - Any relevant notes

3. **Resolution:** Keep original resolution when possible

4. **Format:** JPG or PNG preferred

## Research Focus

**Bambu Lab X1C (Primary)**
- Focus on firmware 01.09+ redesign
- Look for: Bigger thumbnails, bolder fonts, improved navigation
- Key screens: Main dashboard, print monitoring, file selection

**Creality K1 (Secondary)**
- Focus on: Simple, clear navigation patterns
- Look for: Beginner-friendly workflows, guided processes

**FlashForge AD5M Pro (Tertiary)**
- Focus on: Any unique interaction patterns
- Look for: Screen layout alternatives

## Analysis Goals

For each UI, document:
- Color schemes and themes
- Typography choices (font sizes, weights, hierarchy)
- Icon styles and sizes
- Layout patterns (grid, list, card-based)
- Navigation paradigms (tabs, menus, gestures)
- Information density
- Visual hierarchy
- Interactive elements (buttons, sliders, toggles)

See [UI_REDESIGN.md](../UI_REDESIGN.md) for full research documentation.

---

*Last Updated: 2025-10-07*
