# GuppyScreen Release History

## Upstream Release History (ballaswag/guppyscreen)

The original GuppyScreen project by ballaswag was actively developed through April 2024. The following releases were made before the project became inactive:

### 0.0.26-beta (April 28, 2024)
Last release from the original upstream repository.

### 0.0.25-beta (April 23, 2024)
### 0.0.24-beta (April 16, 2024)
### 0.0.23-beta (April 15, 2024)
### 0.0.22-beta (April 14, 2024)
### 0.0.21-beta (April 11, 2024)
### 0.0.20-beta (April 10, 2024)
### 0.0.19-beta (April 8, 2024)
### 0.0.18-beta (April 7, 2024)
### 0.0.17-beta (April 3, 2024)
### 0.0.16-beta (March 29, 2024)
### 0.0.15-beta (March 27, 2024)
### 0.0.14-beta (March 25, 2024)
### 0.0.13-beta (March 21, 2024)
### 0.0.12-beta (March 19, 2024)
### 0.0.11-beta (March 17, 2024)

---

## Fork Development (probielodan/guppyscreen)

After the upstream project became inactive in April 2024, development continued in this fork with significant architectural improvements and multi-platform support.

### 0.0.30-beta (Unreleased)

**Major Features:**
- **Multi-Target Build System**: Introduced menu-driven configuration system (`make config`) supporting four hardware platforms:
  - Simulator (macOS/Linux) - SDL2-based development environment with static linking on macOS
  - Raspberry Pi / BTT Pad - Native ARM build with framebuffer display
  - Creality K1/K1 Max - MIPS cross-compilation for embedded systems
  - FlashForge Adventurer 5M/Pro - ARM cross-compilation with custom macros

- **FlashForge Support**: Integrated changes from DrA1ex's FlashForge fork with conditional compilation:
  - Custom `AUTO_FULL_BED_LEVEL` bed mesh calibration macro
  - Universal input device support (`/dev/input/guppy`)
  - TMC stepper panels conditionally disabled for hardware compatibility

- **macOS Platform Support**:
  - Fixed runtime crashes on macOS by implementing static linking for libhv, spdlog, and wpa_client
  - Added Security and CoreFoundation frameworks for libhv SSL support
  - Cross-platform executable path detection (`_NSGetExecutablePath` on macOS, `/proc/self/exe` on Linux)
  - Command line argument parsing (`-c/--config`, `-h/--help`)

- **Code Modernization**:
  - Migrated from C-style `NULL` to C++11 `nullptr`
  - Replaced `.size() > 0` and `.length() > 0` with idiomatic `.empty()` checks

- **Version Management**:
  - Automated version injection from `VERSION` file into build system
  - Standardized version display across all build targets

- **Build System Improvements**:
  - Configuration persistence via `.config` file
  - Target-specific compilation flags and linking strategies
  - Improved cross-compilation toolchain management
  - Comprehensive documentation in `DEVELOPMENT.md`

**Attribution:**
- FlashForge adaptations based on work by Alexander (DrA1ex) - https://github.com/DrA1ex/guppyscreen
- macOS support and build system refactoring by Preston Brown

**Breaking Changes:**
- Build process now requires `make config` step before `make build` for first-time builds
- `.config` file must exist (can be created manually or via `make config`)
- Default target changed from K1 to simulator for easier development

**Migration Guide:**
For existing users, run `make config` to select your target hardware before building:
```bash
make config  # Select your target (1-4)
make build   # Build for selected target
```

The configuration is saved to `.config` and persists across builds. You can reconfigure anytime by running `make config` again.
