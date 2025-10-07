# Claude Code Context for GuppyScreen

This document provides context for Claude Code when working on the GuppyScreen project.

## Project Overview

**GuppyScreen** is a lightweight LVGL-based touch UI for Klipper 3D printers that runs directly on framebuffer without X11/Wayland. It communicates with Moonraker via WebSocket and HTTP APIs.

- **Language:** C++17
- **Build System:** GNU Make with multi-target configuration
- **Key Dependencies:** LVGL 8.3, libhv, spdlog, SDL2 (simulator only)
- **Current Version:** 0.0.30-beta

## Build System Architecture

### Target Selection

The build system uses a configuration-driven approach:

```bash
make config          # Interactive menu to select target
make                 # Incremental build
make build           # Clean build with all dependencies
```

Supported targets (stored in `.config`):
- `simulator` - macOS/Linux development (SDL2, virtual display)
- `pi` - Raspberry Pi / BTT Pad (ARM, framebuffer)
- `k1` - Creality K1/K1 Max (MIPS, static linking)
- `flashforge` - FlashForge AD5M (ARM, custom config)

### Key Build Details

**Makefile Structure:**
- Lines 87-134: Target-specific configuration (LDFLAGS, DEFINES, toolchain)
- Lines 103-104: Linux simulator uses **dynamic linking with RPATH**
- Line 104: `RPATH='$ORIGIN/../../libhv/lib'` allows binary to find libhv.so relative to executable location

**Important:** The simulator build on Linux uses RPATH to avoid needing `LD_LIBRARY_PATH`. The binary in `build/bin/` can locate shared libraries in `libhv/lib/` via `$ORIGIN` relative path.

**Binary Location:** `build/bin/guppyscreen`

### Common Commands

```bash
# Build incrementally (parallel, uses all cores)
make

# Full clean rebuild
make build

# Check library dependencies
ldd build/bin/guppyscreen

# Verify RPATH is set
readelf -d build/bin/guppyscreen | grep RPATH

# Run simulator (from any directory)
./build/bin/guppyscreen

# Clean build artifacts
make clean
```

## Code Structure

### Source Organization

```
src/
├── main.cpp                    # Entry point
├── guppyscreen.cpp             # Main application class
├── websocket_client.cpp        # Moonraker WebSocket connection
├── state.cpp                   # Application state management
├── *_panel.cpp                 # UI panels (print_status, bedmesh, etc.)
├── config.cpp                  # Configuration management
└── utils.cpp                   # Utility functions

lvgl/                           # LVGL library (submodule)
lv_drivers/                     # LVGL display/input drivers (submodule)
assets/                         # Icons, fonts, images
  ├── material/                 # Material Design theme assets
  └── zbolt/                    # Z-Bolt theme assets
```

### Key Components

**WebSocket Communication:**
- `websocket_client.cpp` - Handles Moonraker connection
- Uses libhv for async HTTP/WebSocket operations
- JSON-RPC protocol for Klipper API calls

**UI Framework:**
- LVGL 8.3 for all UI rendering
- Panel-based architecture (each feature is a separate panel class)
- Custom widgets in `*_container.cpp` files

**Display Abstraction:**
- Framebuffer (`lv_drivers/display/fbdev.c`) for embedded targets
- SDL2 (`lv_drivers/sdl/`) for simulator builds
- Touch input via evdev or SDL2 events

## Development Workflow

### Typical Development Pattern

1. **Simulator Build** - Primary development target
   ```bash
   make config   # Select "simulator"
   make          # Build with all cores
   ./build/bin/guppyscreen
   ```

2. **Testing Changes** - Quick iteration
   ```bash
   make          # Rebuild only changed files
   ```

3. **Target Hardware** - Cross-compile when ready
   ```bash
   make config   # Select target (pi, k1, flashforge)
   make build    # Full rebuild for target
   ```

### Configuration Files

- **Simulator:** `guppyconfig-simulator.json` → copied to `build/bin/guppyconfig.json`
- **Production:** `debian/guppyconfig.json` → copied for embedded targets
- **Runtime:** Binary looks for `guppyconfig.json` in same directory

## Communication Style & Preferences

Based on our interactions, here's what works well:

### Responses
- **Concise and direct** - Answer questions briefly, provide complete information
- **Minimal preamble** - Skip "Here's what I'll do" unless asked
- **Show commands** - Prefer showing actual commands over explaining them
- **Professional tone** - Objective, technical, no superlatives

### Tool Usage
- **Parallel tool calls** - Run independent operations simultaneously
- **Specialized tools first** - Use Read/Edit/Write over bash for file ops
- **Proactive research** - Use Grep/Glob to find information before asking

### Code Changes
- **Read before edit** - Always read files before modifying
- **Verify changes** - Run builds/tests after modifications
- **Update incrementally** - Make focused, single-purpose changes

## Project-Specific Permissions

The following permissions are pre-approved in `.claude/settings.local.json`:

```json
"Bash(make:*)"          # Build commands
"Bash(ldd:*)"           # Library dependency checks
"Bash(readelf:*)"       # Binary inspection
"Bash(./guppyscreen)"   # Run the binary
"Read(//home/pbrown/Code/Printing/guppyscreen-pgb/**)"  # All project files
```

Additional git, Python, and web fetch permissions are also configured.

## Important Technical Notes

### RPATH Configuration
- **Linux simulator builds use RPATH** to locate `libhv.so` at runtime
- Set in `Makefile:104` as `-Wl,-rpath,'$$ORIGIN/../../libhv/lib'`
- Eliminates need for `LD_LIBRARY_PATH` environment variable
- Verify with: `readelf -d build/bin/guppyscreen | grep RPATH`

### macOS vs Linux Builds
- **macOS:** Static linking for all deps (avoids dyld issues)
- **Linux:** Dynamic linking with RPATH for flexibility
- **Both:** Use SDL2 for virtual display in simulator mode

### Cross-Compilation
- K1 target uses `mips-linux-gnu-*` toolchain
- FlashForge uses `arm-unknown-linux-gnueabihf-*`
- All cross-compiled builds use static linking for portability

### Parallel Builds
- Makefile auto-detects CPU cores via `$(NPROC)`
- Default target runs with `-j$(NPROC)` automatically
- Significantly speeds up compilation

## Common Issues & Solutions

**Problem:** `libhv.so: cannot open shared object file`
**Solution:** Ensure RPATH is set in Makefile (line 104) and rebuild

**Problem:** SDL2 not found on macOS
**Solution:** Install via Homebrew, Makefile auto-detects SDL2 prefix

**Problem:** Cross-compilation fails
**Solution:** Ensure appropriate toolchain is installed and in PATH

**Problem:** Configuration file not found at runtime
**Solution:** Binary looks for `guppyconfig.json` in same directory as executable

## Related Documentation

- `README.md` - Project overview, features, hardware support
- `DEVELOPMENT.md` - Detailed build instructions, toolchain setup
- `CHANGELOG.md` - Version history and release notes
- `.claude/settings.local.json` - Project-specific permissions

## Useful Git Context

**Current Branch:** main
**Remote:** GitHub (prestonbrown/guppyscreen fork)
**Submodules:** lvgl, lv_drivers, spdlog, libhv, wpa_supplicant

Always verify submodule status before major builds:
```bash
git submodule status
git submodule update --init --recursive
```

---

*This document is maintained to help Claude Code provide better assistance on this project. Update as the codebase evolves.*
