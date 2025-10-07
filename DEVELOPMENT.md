## Development

This repository contains the Guppy Screen source code and all its external dependencies.

Dependencies:
 - [lvgl](https://github.com/lvgl/lvgl)
   An embedded graphics library
 - [libhv](https://github.com/ithewei/libhv)
   A network library
 - [spdlog](https://github.com/gabime/spdlog)
   A logging library
 - [wpa_supplicant](https://w1.fi/wpa_supplicant/)
   Handles wireless connections

## Build System

GuppyScreen uses a menu-driven configuration system to select your target hardware platform before building.

### Quick Start

1. Configure your build target:
   ```bash
   make config
   ```

2. Build for your selected target:
   ```bash
   make build
   ```

### Build Targets

The build system supports four target platforms:

- **simulator** (default) - macOS/Linux development build with SDL2
  - Uses SDL2-based virtual display
  - Can connect to remote or local virtual Moonraker
  - Dynamic linking (static on macOS)
  - Standard Klipper bed mesh calibration
  - Includes TMC stepper tuning panels

- **pi** - Raspberry Pi / BTT Pad / Debian ARM
  - Native build on ARM device (no cross-compilation)
  - Framebuffer display (no X11/Wayland)
  - Dynamic linking
  - Standard Klipper bed mesh calibration
  - Includes TMC stepper tuning panels

- **k1** - Creality K1 / K1 Max printers
  - MIPS cross-compilation (mips-linux-gnu-)
  - Static linking for embedded system
  - Framebuffer display (no X11/Wayland)
  - Standard Klipper bed mesh calibration
  - Includes TMC stepper tuning panels

- **flashforge** - FlashForge Adventurer 5M / 5M Pro
  - ARM cross-compilation (arm-unknown-linux-gnueabihf-)
  - Dynamic linking
  - Framebuffer display (no X11/Wayland)
  - Custom AUTO_FULL_BED_LEVEL macro
  - Universal input device (/dev/input/guppy)
  - TMC panels DISABLED (hardware limitation)

The selected target is saved to `.config` (gitignored) and persists across builds.

### Configuration File

Your build configuration is stored in `.config` at the project root. You can:
- View the template with documentation: `.config.example`
- Reconfigure anytime: `make config`
- Manually edit `.config` to change targets

### Legacy Environment Variables

The following environment variables are still supported for advanced use:
- `GUPPYSCREEN_VERSION` - Version string displayed in the System Panel
- `GUPPY_THEME=zbolt` - Use Z-Bolt icon set instead of Material Design Icons
- `GUPPY_SMALL_SCREEN` - Enable small screen support (480x320)
- `GUPPY_ROTATE` - Enable screen rotation

## Toolchains

GuppyScreen uses C++17 features (filesystem), so gcc/g++ version 7.2+ with C++17 support is required.

### Build Environment

#### Ubuntu and Debian
For Ubuntu/Debian install build essentials and libsdl2-dev packages.

`sudo apt-get install -y build-essential cmake libsdl2-dev`

#### Arch and Derivatives

For Arch and derivatives install 'base-devel' and 'sdl2' packages.

`sudo pacman -S base-devel cmake sdl2`

#### Mipsel Tool chain

To build guppyscreen for Mipsel (Ingenic X2000E) - specific to the K1 SoC, you will need the mips-gcc720 tool chain.

1. Download the toolchain [here](https://github.com/ballaswag/k1-discovery/releases/download/1.0.0/mips-gcc720-glibc229.tar.gz)
2. `tar xf mips-gcc720-glibc229.tar.gz && export PATH=<path-to-mips-toolchain/bin>:$PATH`

### The Code

Clone the guppyscreen repo (and submodules) and apply a couple of patches locally.

1. `git clone --recursive https://github.com/probielodan/guppyscreen && cd guppyscreen`
2. `(cd lv_drivers/ && git apply ../patches/0001-lv_driver_fb_ioctls.patch)`
3. `(cd spdlog/ && git apply ../patches/0002-spdlog_fmt_initializer_list.patch)`

### Building for Different Targets

The new build system uses `make config` to select your target, then `make build` to compile.

#### Simulator (macOS/Linux Development)

For local development and testing without physical hardware:

**Prerequisites (macOS):**
1. Install Xcode Command Line Tools: `xcode-select --install`
2. Install Homebrew: https://brew.sh
3. Install dependencies: `brew install sdl2 cmake`

**Prerequisites (Linux):**
- Ubuntu/Debian: `sudo apt-get install -y build-essential cmake libsdl2-dev`
- Arch: `sudo pacman -S base-devel cmake sdl2`

**Build Steps:**
```bash
make config  # Select "1) simulator"
make build
```

The build process automatically:
- Copies `guppyconfig-simulator.json` to `build/bin/guppyconfig.json`
- Creates `build/logs/` and `build/thumbnails/` directories
- Uses static linking on macOS, dynamic on Linux

The executable is `./build/bin/guppyscreen`

**macOS Notes:**
- Automatically detects macOS and uses appropriate linking
- Includes platform-specific code for filesystem, networking, and executable path detection
- Compatible with both Apple Silicon and Intel Macs

#### Raspberry Pi / BTT Pad (Native ARM Build)

Build directly on the target device:

```bash
make config  # Select "2) pi"
make build
```

This performs a native ARM build with framebuffer support.

#### Creality K1/K1 Max (MIPS Cross-Compilation)

**Prerequisites:**
1. Download mips-gcc720 toolchain: [here](https://github.com/ballaswag/k1-discovery/releases/download/1.0.0/mips-gcc720-glibc229.tar.gz)
2. Extract and add to PATH:
   ```bash
   tar xf mips-gcc720-glibc229.tar.gz
   export PATH=<path-to-mips-toolchain/bin>:$PATH
   ```

**Build Steps:**
```bash
make config  # Select "3) k1"
make build
```

Uses static linking for the embedded environment.

#### FlashForge Adventurer 5M/Pro (ARM Cross-Compilation)

**Prerequisites:**
- ARM cross-compilation toolchain: `arm-unknown-linux-gnueabihf-`

**Build Steps:**
```bash
make config  # Select "4) flashforge"
make build
```

This target includes FlashForge-specific adaptations:
- Custom `AUTO_FULL_BED_LEVEL` bed mesh command
- Universal input device (`/dev/input/guppy`)
- TMC panels disabled (hardware doesn't have TMC drivers)

The executable is `./build/bin/guppyscreen` for all targets.

### Incremental Builds

After an initial `make build`, you can make changes to source files and use `make` to compile only the files that need recompiling.

### Simulation
Guppy Screen default configurations (guppyconfig.json) is configured for the K1/Max. In order to run it remotely as a simulator build, a few thing needs to be setup.
The following attributes need to be configured in `build/bin/guppyconfig.json`

1. `log_path` - Absolute path to `guppyscreen.log`. Directory must exist locally.
2. `thumbnail_path` - Absolute path to a local directory for storing gcode thumbnails.
3. `moonraker_host` - Moonraker IP address
4. `moonraker_port` - Moonraker Port
5. `wpa_supplicant` - Path to the wpa_supplicant socket (usually under /var/run/wpa_supplicant/)

```
{
  "default_printer": "k1",
  "log_path": "<local_path_to_guppyscreen.log>",
  "printers": {
    "k1": {
      "display_sleep_sec": 300,
      "moonraker_api_key": false,
      "moonraker_host": "<remote_ip_to_moonraker>",
      "moonraker_port": <moonraker_port_if_not_7125>
    }
  },
  "thumbnail_path": "<local_path_to_thumbnail_directory_for_storing_gcode_thumbs>",
  "wpa_supplicant": "<path_to_the_wireless_interface_wpa_supplicant_socket-e.g. /var/run/wpa_supplicant/wlo1>"
}

```

Note: Guppy Screen currently requires running as `root` because it directly interacts with wpa_supplicant.

### Virtual Klipper

For local testing and development without a physical printer, you can use a virtual Klipper printer. There are two options:

1. **virtual-3dprinter** (Recommended): https://github.com/prestonbrown/virtual-3dprinter
   - Easy to set up and use
   - Good for testing Guppy Screen features

2. **Mainsail Virtual Klipper**: https://github.com/mainsail-crew/virtual-klipper-printer
   - More comprehensive setup
   - Requires docker-ce and docker-compose

### Install Docker and Docker Compose

#### Ubuntu and Debian

You can follow the instructions to get docker and docker-compose setup on Ubuntu:
https://docs.docker.com/engine/install/ubuntu/#install-using-the-repository

1. `sudo apt-get update && sudo apt-get install ca-certificates curl gnupg`
2. `sudo install -m 0755 -d /etc/apt/keyrings`
3. `curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg`
4. `sudo chmod a+r /etc/apt/keyrings/docker.gpg`
3. `echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null`
4. `sudo apt-get update`
5. `sudo apt-get install docker-ce docker-compose docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin`

#### For Arch and Derivatives

1. `sudo pacman -S docker docker-compose`
2. `sudo systemctl start docker`

#### Build and Start

1. `git clone https://github.com/mainsail-crew/virtual-klipper-printer.git && cd virtual-klipper-printer`
2. `sudo docker-compose up -d`

You can now configure the guppyconfig.json `moonraker_host` to be `127.0.0.1` and `moonraker_port` to be 7125
