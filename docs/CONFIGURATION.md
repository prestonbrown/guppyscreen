# Configuration Guide

GuppyScreen is configured through the `guppyconfig.json` file and command-line arguments.

## Table of Contents

- [Configuration File](#configuration-file)
- [Command Line Options](#command-line-options)
- [Printer Configuration](#printer-configuration)
- [Display Settings](#display-settings)
- [Network Configuration](#network-configuration)
- [Advanced Options](#advanced-options)

---

## Configuration File

### Location
- **Simulator:** `build/bin/guppyconfig.json`
- **Production:** `/usr/data/guppyscreen/guppyconfig.json` (K1/Max)
- **Raspberry Pi:** `/home/pi/.config/guppyscreen/guppyconfig.json`

### Basic Structure

```json
{
  "default_printer": "my_printer",
  "log_path": "/var/log/guppyscreen.log",
  "thumbnail_path": "/tmp/guppyscreen/thumbnails",
  "printers": {
    "my_printer": {
      "display_sleep_sec": 300,
      "moonraker_api_key": false,
      "moonraker_host": "127.0.0.1",
      "moonraker_port": 7125
    }
  },
  "wpa_supplicant": "/var/run/wpa_supplicant/wlan0"
}
```

---

## Command Line Options

### Usage
```bash
guppyscreen [OPTIONS]
```

### Available Options
- `-c, --config <path>` - Specify custom config file path
- `-h, --help` - Display help information

### Examples
```bash
# Use custom config file
./guppyscreen -c /path/to/my-config.json

# Display help
./guppyscreen --help
```

---

## Printer Configuration

### Single Printer Setup

```json
{
  "default_printer": "k1_max",
  "printers": {
    "k1_max": {
      "moonraker_host": "192.168.1.100",
      "moonraker_port": 7125,
      "display_sleep_sec": 600
    }
  }
}
```

### Multi-Printer Setup

```json
{
  "default_printer": "primary",
  "printers": {
    "primary": {
      "moonraker_host": "192.168.1.100",
      "moonraker_port": 7125,
      "display_sleep_sec": 300
    },
    "backup": {
      "moonraker_host": "192.168.1.101",
      "moonraker_port": 7125,
      "display_sleep_sec": 300
    }
  }
}
```

### Printer Options

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `moonraker_host` | string | `"127.0.0.1"` | Moonraker IP address or hostname |
| `moonraker_port` | integer | `7125` | Moonraker port number |
| `moonraker_api_key` | boolean/string | `false` | Moonraker API key (if required) |
| `display_sleep_sec` | integer | `300` | Screen timeout in seconds (0 = disabled) |

---

## Display Settings

### Resolution Configuration

GuppyScreen automatically detects framebuffer resolution. For custom configurations:

**Supported Resolutions:**
- 480x320 (small screens)
- 800x480 (K1/Max, common touch displays)
- 1024x600 (larger displays)
- 1280x720 (HD displays)

### Rotation

Set via environment variable:
```bash
export GUPPY_ROTATE=1
./guppyscreen
```

Rotation values:
- `0` - Normal (0°)
- `1` - 90° clockwise
- `2` - 180° (upside down)
- `3` - 270° clockwise (90° counter-clockwise)

### Small Screen Mode

For 480x320 displays:
```bash
export GUPPY_SMALL_SCREEN=1
./guppyscreen
```

---

## Network Configuration

### Wi-Fi Management

GuppyScreen can manage Wi-Fi connections through wpa_supplicant.

**Configure wpa_supplicant path:**
```json
{
  "wpa_supplicant": "/var/run/wpa_supplicant/wlan0"
}
```

**Requirements:**
- GuppyScreen must run as root (required for wpa_supplicant control)
- wpa_supplicant must be running
- Control socket must be accessible

### Moonraker API Authentication

If Moonraker requires API key authentication:

```json
{
  "printers": {
    "secure_printer": {
      "moonraker_host": "192.168.1.100",
      "moonraker_port": 7125,
      "moonraker_api_key": "your-api-key-here"
    }
  }
}
```

### CORS Configuration (Moonraker)

Ensure Moonraker allows connections from GuppyScreen:

**moonraker.conf:**
```ini
[authorization]
cors_domains:
    *

[server]
enable_debug_logging: False
```

---

## Advanced Options

### Logging

Configure log path and level:
```json
{
  "log_path": "/var/log/guppyscreen.log"
}
```

**Log Levels:**
- Controlled by spdlog at compile time
- Default: INFO level
- For debug builds: DEBUG level

### Thumbnail Cache

Configure thumbnail storage:
```json
{
  "thumbnail_path": "/tmp/guppyscreen/thumbnails"
}
```

**Notes:**
- Directory must exist and be writable
- Thumbnails are extracted from G-code files
- Cached to improve performance

### Theme Selection

Set via environment variable at compile time:
```bash
export GUPPY_THEME=zbolt
make build
```

Available themes:
- `material` (default) - Material Design icons
- `zbolt` - Z-Bolt icon set

### Platform-Specific Settings

#### K1/K1 Max
```json
{
  "log_path": "/usr/data/guppyscreen/guppyscreen.log",
  "thumbnail_path": "/usr/data/guppyscreen/thumbnails",
  "wpa_supplicant": "/var/run/wpa_supplicant/wlan0"
}
```

#### Raspberry Pi
```json
{
  "log_path": "/home/pi/.local/guppyscreen/guppyscreen.log",
  "thumbnail_path": "/home/pi/.local/guppyscreen/thumbnails",
  "wpa_supplicant": "/var/run/wpa_supplicant/wlan0"
}
```

#### FlashForge
```json
{
  "log_path": "/tmp/guppyscreen.log",
  "thumbnail_path": "/tmp/guppyscreen/thumbnails"
}
```

*Note: FlashForge uses custom input device at `/dev/input/guppy` (configured at build time)*

---

## Example Configurations

### Development/Simulator

```json
{
  "default_printer": "dev",
  "log_path": "/tmp/guppyscreen.log",
  "thumbnail_path": "/tmp/guppyscreen/thumbnails",
  "printers": {
    "dev": {
      "moonraker_host": "localhost",
      "moonraker_port": 7125,
      "display_sleep_sec": 0
    }
  }
}
```

### Production K1

```json
{
  "default_printer": "k1",
  "log_path": "/usr/data/guppyscreen/guppyscreen.log",
  "thumbnail_path": "/usr/data/guppyscreen/thumbnails",
  "printers": {
    "k1": {
      "moonraker_host": "127.0.0.1",
      "moonraker_port": 7125,
      "display_sleep_sec": 300,
      "moonraker_api_key": false
    }
  },
  "wpa_supplicant": "/var/run/wpa_supplicant/wlan0"
}
```

### Remote Development

```json
{
  "default_printer": "remote_k1",
  "log_path": "./guppyscreen.log",
  "thumbnail_path": "./thumbnails",
  "printers": {
    "remote_k1": {
      "moonraker_host": "192.168.1.100",
      "moonraker_port": 7125,
      "display_sleep_sec": 0
    }
  }
}
```

---

## Troubleshooting

### Common Issues

**Config file not found:**
- Specify path with `-c` flag
- Check file permissions
- Verify JSON syntax

**Cannot connect to Moonraker:**
- Verify IP address and port
- Check network connectivity
- Review Moonraker CORS settings
- Confirm Moonraker is running

**Wi-Fi control not working:**
- Ensure running as root
- Verify wpa_supplicant socket path
- Check wpa_supplicant is running

**Thumbnails not displaying:**
- Verify thumbnail_path exists and is writable
- Ensure G-code files contain embedded thumbnails
- Check log for thumbnail extraction errors

For more help, see the [Installation Guide](INSTALLATION.md) or open an issue on [GitHub](https://github.com/prestonbrown/guppyscreen/issues).
