# GuppyScreen

**A Modern Touch UI for Klipper 3D Printers**

GuppyScreen is a lightweight, framebuffer-based touch interface for Klipper that communicates directly with Moonraker. Built on LVGL (Light and Versatile Graphics Library), it runs as a standalone executable without requiring X11, Wayland, or any display server.

<p align="center">
    <a aria-label="Version" href="https://github.com/prestonbrown/guppyscreen/releases">
      <img src="https://img.shields.io/badge/version-0.0.30--beta-blue?style=flat-square">
  </a>
    <a aria-label="Stars" href="https://github.com/prestonbrown/guppyscreen/stargazers">
      <img src="https://img.shields.io/github/stars/prestonbrown/guppyscreen?style=flat-square">
  </a>
    <a aria-label="License" href="https://github.com/prestonbrown/guppyscreen/blob/main/LICENSE">
      <img src="https://img.shields.io/github/license/prestonbrown/guppyscreen?style=flat-square">
  </a>
</p>

---

## 🚀 Project Status

This is an **active fork** of the original [GuppyScreen by ballaswag](https://github.com/ballaswag/guppyscreen), which was last updated in April 2024. We've continued development with:

- **Multi-platform build system** supporting 4 hardware targets
- **macOS development support** with SDL2 simulator
- **FlashForge Adventurer 5M integration** from DrA1ex's fork
- **Improved build tooling** with automatic parallelization
- **Active maintenance** and bug fixes

**Current Version:** 0.0.30-beta

---

## ✨ What is GuppyScreen?

GuppyScreen transforms your Klipper 3D printer into a fully-featured touchscreen experience. It provides:

### Core Capabilities
- **Real-time print monitoring** with live status, progress, and thumbnails
- **Complete printer control** - temperatures, fans, movement, extrusion
- **Advanced tuning** - Input Shaper, bed mesh visualization, TMC diagnostics
- **Macro execution** with custom console/shell interface
- **Multi-printer support** - manage multiple Klipper instances
- **Spoolman integration** for filament tracking

### Key Features
- ✅ **Bed Mesh Visualization** - 3D visualization of bed leveling data
- ✅ **Input Shaper Graphs** - PSD frequency analysis charts
- ✅ **TMC Stepper Diagnostics** - Real-time motor driver metrics
- ✅ **Belt Calibration** - Excitation and analysis tools
- ✅ **Fine Tuning** - Live adjustment of speed, flow, Z-offset, Pressure Advance
- ✅ **Power Device Control** - Smart plug/relay integration via Moonraker
- ✅ **Wi-Fi Management** - Configure wireless directly from the interface
- ✅ **File Browser** - Browse and select G-code files with thumbnails

---

## 🖥️ Supported Hardware

### Printer Hardware
| Platform | Architecture | Linking | Notes |
|----------|-------------|---------|-------|
| **Creality K1/K1 Max** | MIPS (Ingenic X2000E) | Static | Primary target hardware |
| **FlashForge AD5M/AD5M Pro** | ARM | Dynamic | Custom bed mesh macro |
| **Raspberry Pi / BTT Pad** | ARM (native) | Dynamic | Framebuffer display |
| **Development Simulator** | x86_64/ARM64 | Static (macOS)<br>Dynamic (Linux) | SDL2-based virtual display |

### Display Support
- Direct framebuffer rendering (no X11/Wayland required)
- Multiple resolution support (480x320, 800x480, 1024x600, etc.)
- Touch input via evdev
- Rotation support for various mounting orientations

---

## 🛠️ Quick Start

### For Users (Pre-built Binaries)
See [Installation Guide](docs/INSTALLATION.md) for platform-specific instructions.

### For Developers
Building from source is simple with our menu-driven configuration:

```bash
# Clone the repository
git clone --recursive https://github.com/prestonbrown/guppyscreen
cd guppyscreen

# Apply required patches
(cd lv_drivers/ && git apply ../patches/0001-lv_driver_fb_ioctls.patch)
(cd spdlog/ && git apply ../patches/0002-spdlog_fmt_initializer_list.patch)

# Configure your target hardware
make config

# Build (automatically uses all CPU cores)
make build
```

The build process will:
1. Detect your CPU cores and build in parallel
2. Compile all dependencies (libhv, spdlog, wpa_supplicant)
3. Generate the final binary in `build/bin/guppyscreen`

**See [DEVELOPMENT.md](DEVELOPMENT.md) for detailed build instructions.**

---

## 📚 Documentation

- **[Development Guide](DEVELOPMENT.md)** - Building from source, toolchains, testing
- **[Installation Guide](docs/INSTALLATION.md)** - Platform-specific installation
- **[Configuration](docs/CONFIGURATION.md)** - Customizing GuppyScreen
- **[Changelog](CHANGELOG.md)** - Release history and version notes

---

## 🎨 Screenshots

### Material Design Theme
![Material Theme](https://github.com/prestonbrown/guppyscreen/blob/main/screenshots/material/material_screenshot.png)

*More screenshots available in the [screenshots](screenshots/) directory*

---

## 🔧 Technology Stack

GuppyScreen is built with modern C++17 and leverages:

- **[LVGL v8.3](https://lvgl.io/)** - Embedded graphics library
- **[libhv](https://github.com/ithewei/libhv)** - High-performance network library
- **[spdlog](https://github.com/gabime/spdlog)** - Fast C++ logging
- **[SDL2](https://www.libsdl.org/)** - Cross-platform development (simulator only)
- **[Moonraker API](https://moonraker.readthedocs.io/)** - Klipper HTTP/WebSocket API
- **[wpa_supplicant](https://w1.fi/wpa_supplicant/)** - Wi-Fi configuration

---

## 🗺️ Roadmap

**Upcoming Features:**
- [ ] Exclude Object support
- [ ] Firmware Retraction control
- [ ] Enhanced thumbnail support
- [ ] Custom theme engine

**Contributions welcome!** Open an issue to discuss new features or report bugs.

---

## 🙏 Credits & Attribution

### Original Project
- **[GuppyScreen](https://github.com/ballaswag/guppyscreen/)** by ballaswag - Original project (2023-2024)

### Continued Development
- **[probielodan](https://github.com/probielodan/guppyscreen)** - Initial fork and continued development (2024)
- **Preston Brown** - Bed mesh visualization, macOS support, build system refactoring, current maintenance
- **[Alexander (DrA1ex)](https://github.com/DrA1ex/guppyscreen)** - FlashForge integration

### Inspiration & Resources
- **[Moonraker](https://github.com/Arksine/moonraker)** - Klipper API server
- **[KlipperScreen](https://github.com/KlipperScreen/KlipperScreen)** - Alternative Klipper UI
- **[Fluidd](https://github.com/fluidd-core/fluidd)** - Web-based Klipper interface
- **[Klippain ShakeTune](https://github.com/Frix-x/klippain-shaketune)** - Input shaper analysis

### Assets
- **[Material Design Icons](https://pictogrammers.com/library/mdi/)** - Material theme icons
- **[Z-Bolt Icons](https://github.com/Z-Bolt/OctoScreen)** - Alternative icon set

---

## 📄 License

This project is licensed under the GNU General Public License v3.0 (GPLv3). See [LICENSE](LICENSE) for details.

---

## 🔗 Links

- **Report Issues:** [GitHub Issues](https://github.com/prestonbrown/guppyscreen/issues)
- **Discussions:** [GitHub Discussions](https://github.com/prestonbrown/guppyscreen/discussions)
- **Original Project:** [ballaswag/guppyscreen](https://github.com/ballaswag/guppyscreen)
- **probielodan Fork:** [probielodan/guppyscreen](https://github.com/probielodan/guppyscreen)
