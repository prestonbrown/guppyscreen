# Installation Guide

This guide covers installing pre-built GuppyScreen binaries on supported hardware platforms.

## Table of Contents

- [Creality K1/K1 Max](#creality-k1k1-max)
- [FlashForge Adventurer 5M/Pro](#flashforge-adventurer-5mpro)
- [Raspberry Pi / BTT Pad](#raspberry-pi--btt-pad)
- [Development Simulator](#development-simulator)

---

## Creality K1/K1 Max

### Prerequisites
- Klipper installed and configured
- Moonraker running and accessible
- SSH access to the printer

### Material Design Theme (Recommended)
```bash
sh -c "$(wget --no-check-certificate -qO - https://raw.githubusercontent.com/prestonbrown/guppyscreen/main/scripts/installer.sh)"
```

### Z-Bolt Theme (Alternative)
```bash
sh -c "$(wget --no-check-certificate -qO - https://raw.githubusercontent.com/prestonbrown/guppyscreen/main/scripts/installer.sh)" -s zbolt
```

### Uninstall
To revert to the stock Creality UI:
```bash
/usr/data/guppyscreen/reinstall-creality.sh
```

---

## FlashForge Adventurer 5M/Pro

### Prerequisites
- Custom firmware with Klipper support
- Moonraker configured for FlashForge
- SSH access

### Installation
*Installation scripts for FlashForge are under development.*

For manual installation, see the [Development Guide](../DEVELOPMENT.md) for building from source.

### Configuration Notes
- FlashForge uses a custom `AUTO_FULL_BED_LEVEL` macro instead of standard `BED_MESH_CALIBRATE`
- Input device is mapped to `/dev/input/guppy`
- TMC stepper diagnostics are disabled (hardware limitation)

---

## Raspberry Pi / BTT Pad

### Prerequisites
- Debian/Raspbian-based OS
- Klipper and Moonraker installed
- Framebuffer access
- SSH or terminal access

### Installation
```bash
wget -O - https://raw.githubusercontent.com/prestonbrown/guppyscreen/main/scripts/installer-deb.sh | bash
```

### Tested Hardware
- BTT Pad 7
- Raspberry Pi 4 with touchscreen
- Raspberry Pi 3B+ with touchscreen

---

## Development Simulator

The simulator allows you to develop and test GuppyScreen without physical printer hardware.

### macOS

**Prerequisites:**
1. Install Homebrew: https://brew.sh
2. Install dependencies:
   ```bash
   brew install sdl2 cmake
   xcode-select --install
   ```

**Build and Run:**
```bash
git clone --recursive https://github.com/prestonbrown/guppyscreen
cd guppyscreen
(cd lv_drivers/ && git apply ../patches/0001-lv_driver_fb_ioctls.patch)
(cd spdlog/ && git apply ../patches/0002-spdlog_fmt_initializer_list.patch)
make config  # Select option 1: simulator
make build
./build/bin/guppyscreen
```

**Configuration:**
Edit `build/bin/guppyconfig.json` to point to your Moonraker instance:
```json
{
  "default_printer": "dev",
  "log_path": "/tmp/guppyscreen.log",
  "printers": {
    "dev": {
      "moonraker_host": "192.168.1.100",
      "moonraker_port": 7125
    }
  }
}
```

### Linux

**Prerequisites (Ubuntu/Debian):**
```bash
sudo apt-get install -y build-essential cmake libsdl2-dev
```

**Prerequisites (Arch):**
```bash
sudo pacman -S base-devel cmake sdl2
```

**Build and Run:**
Same as macOS instructions above.

---

## Virtual Klipper for Testing

For testing without physical hardware, use a virtual Klipper environment:

### Option 1: virtual-3dprinter (Recommended)
```bash
git clone https://github.com/prestonbrown/virtual-3dprinter
cd virtual-3dprinter
# Follow setup instructions in repository
```

### Option 2: Mainsail Virtual Klipper
```bash
git clone https://github.com/mainsail-crew/virtual-klipper-printer
cd virtual-klipper-printer
sudo docker-compose up -d
```

---

## Troubleshooting

### Display Issues
- **Black screen:** Check framebuffer permissions and `/dev/fb0` access
- **Wrong resolution:** Verify screen resolution in config
- **No touch input:** Check `/dev/input/event*` permissions

### Connection Issues
- **Cannot connect to Moonraker:** Verify IP address and port in config
- **WebSocket errors:** Ensure Moonraker CORS is configured correctly
- **API key errors:** Check if Moonraker requires API key authentication

### Performance Issues
- **Slow UI:** May indicate high CPU usage; check Klipper/Moonraker logs
- **Choppy animations:** Framebuffer performance varies by hardware

For additional help, open an issue on [GitHub](https://github.com/prestonbrown/guppyscreen/issues).
