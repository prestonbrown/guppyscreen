#!/bin/bash

# GuppyScreen Build Configuration Script
# Interactive menu to configure build target and options

CONFIG_FILE=".config"
CONFIG_EXAMPLE=".config.example"

# Colors for better UX (used in fallback mode)
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if whiptail is available
HAS_WHIPTAIL=false
WHIPTAIL_ASKED_FILE=".config.whiptail_asked"

if command -v whiptail &> /dev/null; then
    HAS_WHIPTAIL=true
else
    # On macOS, offer to install whiptail (via newt package)
    if [[ "$OSTYPE" == "darwin"* ]] && ! [ -f "$WHIPTAIL_ASKED_FILE" ]; then
        echo -e "${YELLOW}whiptail is not installed. It provides a nicer menu interface.${NC}"
        echo ""
        read -p "Would you like to install it via Homebrew? (y/n): " install_choice

        # Remember that we asked, regardless of answer
        touch "$WHIPTAIL_ASKED_FILE"

        case $install_choice in
            y|Y)
                echo "Installing newt (provides whiptail)..."
                if command -v brew &> /dev/null; then
                    brew install newt
                    if command -v whiptail &> /dev/null; then
                        HAS_WHIPTAIL=true
                        echo -e "${GREEN}✓ whiptail installed successfully${NC}"
                    fi
                else
                    echo -e "${RED}Homebrew not found. Please install Homebrew first: https://brew.sh${NC}"
                fi
                ;;
            *)
                echo "Continuing with fallback menu..."
                ;;
        esac
        echo ""
    fi
fi

print_header() {
    echo -e "${BLUE}╔═════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║     GuppyScreen Build Configuration         ║${NC}"
    echo -e "${BLUE}╚═════════════════════════════════════════════╝${NC}"
    echo ""
}

print_current_config() {
    if [ -f "$CONFIG_FILE" ]; then
        echo -e "${YELLOW}Current configuration:${NC}"
        cat "$CONFIG_FILE" | sed 's/^/  /'
        echo ""
    fi
}

select_target_whiptail() {
    local current_target=""
    if [ -f "$CONFIG_FILE" ]; then
        current_target=$(grep "^TARGET=" "$CONFIG_FILE" | cut -d'=' -f2)
    fi

    local default_item="simulator"
    if [ -n "$current_target" ]; then
        default_item="$current_target"
    fi

    TARGET=$(whiptail --title "GuppyScreen Build Configuration" \
        --menu "Select target hardware platform:" 20 78 4 \
        --default-item "$default_item" \
        "simulator" "macOS/Linux development (SDL2, remote/local testing)" \
        "pi" "Raspberry Pi / BTT Pad (ARM native, framebuffer)" \
        "k1" "Creality K1/K1 Max (MIPS embedded, static linking)" \
        "flashforge" "FlashForge AD5M/Pro (ARM, custom bed mesh)" \
        3>&1 1>&2 2>&3)

    # Check if user cancelled
    if [ $? -ne 0 ]; then
        echo "Configuration cancelled."
        exit 1
    fi
}

select_target_fallback() {
    echo -e "${GREEN}Select target hardware:${NC}"
    echo ""
    echo "  1) simulator     - macOS/Linux development (default)"
    echo "                     • SDL2-based virtual display"
    echo "                     • Connect to remote or local virtual Moonraker"
    echo "                     • Standard Klipper commands"
    echo ""
    echo "  2) pi            - Raspberry Pi / BTT Pad / Debian ARM"
    echo "                     • Native build on ARM device"
    echo "                     • Framebuffer display (no X11)"
    echo "                     • Standard Klipper commands"
    echo "                     • Includes TMC stepper motor tuning"
    echo ""
    echo "  3) k1            - Creality K1/K1 Max printers"
    echo "                     • MIPS cross-compilation"
    echo "                     • Static linking for embedded system"
    echo "                     • Includes TMC stepper motor tuning"
    echo ""
    echo "  4) flashforge    - FlashForge Adventurer 5M/Pro"
    echo "                     • ARM cross-compilation"
    echo "                     • Universal input device (/dev/input/guppy)"
    echo "                     • Custom AUTO_FULL_BED_LEVEL macro"
    echo "                     • TMC panels disabled (hardware limitation)"
    echo ""
    echo "  q) quit          - Exit without saving"
    echo ""

    while true; do
        read -p "Choice [1-4, q]: " choice
        case $choice in
            1)
                TARGET="simulator"
                break
                ;;
            2)
                TARGET="pi"
                break
                ;;
            3)
                TARGET="k1"
                break
                ;;
            4)
                TARGET="flashforge"
                break
                ;;
            q|Q)
                echo "Configuration cancelled."
                exit 0
                ;;
            *)
                echo -e "${RED}Invalid choice. Please enter 1, 2, 3, 4, or q.${NC}"
                ;;
        esac
    done
}

select_target() {
    if [ "$HAS_WHIPTAIL" = true ]; then
        select_target_whiptail
    else
        select_target_fallback
    fi
}

save_config() {
    cat > "$CONFIG_FILE" << EOF
# GuppyScreen Build Configuration
# Generated by: make config
#
# Valid TARGET values: simulator, k1, flashforge
TARGET=$TARGET
EOF

    echo ""
    echo -e "${GREEN}✓ Configuration saved to $CONFIG_FILE${NC}"
    echo ""
    echo -e "${BLUE}Build configuration:${NC}"
    echo -e "  Target: ${GREEN}$TARGET${NC}"
    echo ""
    echo -e "${YELLOW}Next steps:${NC}"
    echo -e "  1. Run ${GREEN}make build${NC} to compile for ${GREEN}$TARGET${NC}"
    echo -e "  2. Run ${GREEN}make config${NC} again to change target"
    echo ""
}

# Main execution
print_header
print_current_config
select_target
save_config

exit 0
