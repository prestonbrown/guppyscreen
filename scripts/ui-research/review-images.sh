#!/usr/bin/env bash
#===============================================================================
# Interactive Image Reviewer for Terminal (Sixel/Kitty/iTerm2)
#===============================================================================
#
# USAGE:
#   ./review-images.sh directory/ [output_file]
#
# REQUIREMENTS:
#   - chafa (for image display): brew install chafa
#   OR
#   - img2sixel (alternative): brew install libsixel
#
# DESCRIPTION:
#   Interactive terminal-based image viewer for reviewing extracted screenshots.
#   Displays images one at a time and lets you flag relevant UI screenshots.
#
# CONTROLS:
#   y/Y - Flag as RELEVANT (UI screenshot)
#   n/N - Skip (not UI)
#   q/Q - Quit review
#   ? - Show help
#
# OUTPUT:
#   Creates flagged_images.txt with list of flagged files
#
#===============================================================================

set -euo pipefail

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

# Check requirements
if ! command -v chafa >/dev/null 2>&1 && ! command -v img2sixel >/dev/null 2>&1; then
    echo -e "${RED}Error: Neither chafa nor img2sixel found.${NC}"
    echo ""
    echo "Install one of:"
    echo "  brew install chafa        (recommended - better quality)"
    echo "  brew install libsixel     (alternative)"
    exit 1
fi

# Determine which tool to use
if command -v chafa >/dev/null 2>&1; then
    IMAGE_VIEWER="chafa"
else
    IMAGE_VIEWER="img2sixel"
fi

# Parse arguments
if [ $# -lt 1 ]; then
    echo "Usage: $0 directory/ [output_file]"
    echo ""
    echo "Example:"
    echo "  $0 docs/ui-research/bambu-x1c/main-screen/"
    echo "  $0 docs/ui-research/flashforge-ad5m-pro/main-screen/ flagged.txt"
    exit 1
fi

INPUT_DIR="$1"
OUTPUT_FILE="${2:-flagged_images.txt}"

if [ ! -d "$INPUT_DIR" ]; then
    echo -e "${RED}Error: Directory not found: $INPUT_DIR${NC}"
    exit 1
fi

# Get absolute path
INPUT_DIR=$(cd "$INPUT_DIR" && pwd)

# Find all images (compatible with older bash versions)
IMAGES=()
while IFS= read -r -d '' file; do
    IMAGES+=("$file")
done < <(find "$INPUT_DIR" -type f \( -iname "*.jpg" -o -iname "*.jpeg" -o -iname "*.png" -o -iname "*.tif" -o -iname "*.tiff" \) -print0 | sort -z)

TOTAL=${#IMAGES[@]}

if [ "$TOTAL" -eq 0 ]; then
    echo -e "${RED}Error: No images found in $INPUT_DIR${NC}"
    exit 1
fi

# Initialize output file
> "$OUTPUT_FILE"

echo -e "${CYAN}╔═══════════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║${NC}  ${BOLD}Interactive Image Reviewer for UI Screenshot Curation${NC}            ${CYAN}║${NC}"
echo -e "${CYAN}╚═══════════════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${YELLOW}Directory:${NC} $INPUT_DIR"
echo -e "${YELLOW}Total Images:${NC} $TOTAL"
echo -e "${YELLOW}Output File:${NC} $OUTPUT_FILE"
echo -e "${YELLOW}Viewer:${NC} $IMAGE_VIEWER"
echo ""
echo -e "${BOLD}Controls:${NC}"
echo -e "  ${GREEN}y/Y${NC} - Flag as RELEVANT UI screenshot"
echo -e "  ${RED}n/N${NC} - Skip (not a UI screenshot)"
echo -e "  ${BLUE}d/D${NC} - Flag for DELETE (junk/non-UI)"
echo -e "  ${YELLOW}?${NC}   - Show this help again"
echo -e "  ${RED}q/Q${NC} - Quit review"
echo ""
echo -e "Press ${BOLD}ENTER${NC} to start..."
read -r < /dev/tty

CURRENT=0
FLAGGED_COUNT=0
DELETE_COUNT=0
SKIPPED_COUNT=0

# Function to display image
display_image() {
    local img="$1"
    local terminal_width=$(tput cols)
    local terminal_height=$(tput lines)

    # Calculate display size (leave room for UI)
    local max_height=$((terminal_height - 12))
    local max_width=$((terminal_width - 4))

    # Clear screen
    clear

    # Show image with appropriate viewer
    if [ "$IMAGE_VIEWER" = "chafa" ]; then
        # Chafa has better quality and auto-detects terminal type
        chafa --size="${max_width}x${max_height}" --animate=off "$img" 2>/dev/null || echo "[Image display failed]"
    else
        # img2sixel fallback
        img2sixel -w "$max_width" -h "$max_height" "$img" 2>/dev/null || echo "[Image display failed]"
    fi
}

# Function to show status
show_status() {
    local img="$1"
    local num="$2"
    local basename=$(basename "$img")
    local filesize=$(du -h "$img" | cut -f1)

    echo ""
    echo -e "${CYAN}════════════════════════════════════════════════════════════════════════${NC}"
    echo -e "${BOLD}Image $((num + 1))/$TOTAL${NC} - ${YELLOW}$basename${NC} (${filesize})"
    echo -e "${CYAN}────────────────────────────────────────────────────────────────────────${NC}"
    echo -e "Flagged: ${GREEN}$FLAGGED_COUNT${NC} | Skipped: ${YELLOW}$SKIPPED_COUNT${NC} | Delete: ${RED}$DELETE_COUNT${NC}"
    echo -e "${CYAN}════════════════════════════════════════════════════════════════════════${NC}"
    echo -e "Is this a ${BOLD}UI screenshot${NC}? [${GREEN}y${NC}/${RED}n${NC}/${BLUE}d${NC}elete/${YELLOW}?${NC}/${RED}q${NC}uit] "
}

# Main review loop
while [ $CURRENT -lt $TOTAL ]; do
    IMG="${IMAGES[$CURRENT]}"

    # Display image
    display_image "$IMG"

    # Show status and prompt
    show_status "$IMG" "$CURRENT"

    # Get user input (read from /dev/tty to avoid process substitution conflicts)
    read -r -n 1 RESPONSE < /dev/tty
    echo ""

    case "$RESPONSE" in
        y|Y)
            echo "$IMG" >> "$OUTPUT_FILE"
            echo -e "${GREEN}✓ Flagged as UI screenshot${NC}"
            ((FLAGGED_COUNT++))
            ((CURRENT++))
            sleep 0.5
            ;;
        n|N)
            echo -e "${YELLOW}○ Skipped${NC}"
            ((SKIPPED_COUNT++))
            ((CURRENT++))
            sleep 0.3
            ;;
        d|D)
            echo "DELETE:$IMG" >> "$OUTPUT_FILE"
            echo -e "${RED}✗ Flagged for deletion${NC}"
            ((DELETE_COUNT++))
            ((CURRENT++))
            sleep 0.5
            ;;
        \?)
            clear
            echo -e "${BOLD}Help:${NC}"
            echo ""
            echo -e "  ${GREEN}y/Y${NC} - Flag as RELEVANT UI screenshot (keep)"
            echo -e "  ${RED}n/N${NC} - Skip (neither keep nor delete)"
            echo -e "  ${BLUE}d/D${NC} - Flag for DELETE (junk/non-UI)"
            echo -e "  ${YELLOW}?${NC}   - Show this help"
            echo -e "  ${RED}q/Q${NC} - Quit review"
            echo ""
            echo "Press ENTER to continue..."
            read -r < /dev/tty
            ;;
        q|Q)
            echo -e "${YELLOW}Quitting review...${NC}"
            break
            ;;
        *)
            echo -e "${YELLOW}Invalid input. Press '?' for help.${NC}"
            sleep 1
            ;;
    esac
done

# Final summary
clear
echo -e "${CYAN}╔═══════════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║${NC}  ${BOLD}Review Complete!${NC}                                                     ${CYAN}║${NC}"
echo -e "${CYAN}╚═══════════════════════════════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${BOLD}Summary:${NC}"
echo -e "  Total Reviewed: ${CYAN}$CURRENT${NC} / $TOTAL"
echo -e "  Flagged as UI: ${GREEN}$FLAGGED_COUNT${NC}"
echo -e "  Skipped: ${YELLOW}$SKIPPED_COUNT${NC}"
echo -e "  Marked for Delete: ${RED}$DELETE_COUNT${NC}"
echo ""
echo -e "${BOLD}Output saved to:${NC} ${YELLOW}$OUTPUT_FILE${NC}"
echo ""

# Show flagged files
if [ "$FLAGGED_COUNT" -gt 0 ]; then
    echo -e "${GREEN}Flagged UI Screenshots:${NC}"
    grep -v "^DELETE:" "$OUTPUT_FILE" | while read -r line; do
        echo "  $(basename "$line")"
    done
    echo ""
fi

if [ "$DELETE_COUNT" -gt 0 ]; then
    echo -e "${RED}Files Marked for Deletion:${NC}"
    grep "^DELETE:" "$OUTPUT_FILE" | sed 's/^DELETE://' | while read -r line; do
        echo "  $(basename "$line")"
    done
    echo ""
fi

# Generate parseable output
PARSED_OUTPUT="${OUTPUT_FILE%.txt}_parsed.json"
cat > "$PARSED_OUTPUT" <<EOF
{
  "review_date": "$(date -u +"%Y-%m-%dT%H:%M:%SZ")",
  "directory": "$INPUT_DIR",
  "total_images": $TOTAL,
  "reviewed": $CURRENT,
  "flagged_ui": $FLAGGED_COUNT,
  "skipped": $SKIPPED_COUNT,
  "marked_delete": $DELETE_COUNT,
  "flagged_files": [
EOF

# Add flagged files as JSON array
first=true
grep -v "^DELETE:" "$OUTPUT_FILE" 2>/dev/null | while read -r line; do
    if [ "$first" = true ]; then
        echo "    \"$line\"" >> "$PARSED_OUTPUT"
        first=false
    else
        echo "    ,\"$line\"" >> "$PARSED_OUTPUT"
    fi
done || true

cat >> "$PARSED_OUTPUT" <<EOF
  ],
  "delete_files": [
EOF

# Add delete files as JSON array
first=true
grep "^DELETE:" "$OUTPUT_FILE" 2>/dev/null | sed 's/^DELETE://' | while read -r line; do
    if [ "$first" = true ]; then
        echo "    \"$line\"" >> "$PARSED_OUTPUT"
        first=false
    else
        echo "    ,\"$line\"" >> "$PARSED_OUTPUT"
    fi
done || true

cat >> "$PARSED_OUTPUT" <<EOF
  ]
}
EOF

echo -e "${BOLD}Machine-readable output:${NC} ${CYAN}$PARSED_OUTPUT${NC}"
echo ""
echo -e "${GREEN}Review session complete!${NC}"
