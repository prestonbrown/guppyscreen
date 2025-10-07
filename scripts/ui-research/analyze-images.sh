#!/usr/bin/env bash
#===============================================================================
# Analyze extracted images to identify likely UI screenshots
#===============================================================================
#
# USAGE:
#   ./analyze-images.sh directory/
#
# REQUIREMENTS:
#   - imagemagick (for identify command): brew install imagemagick
#
# DESCRIPTION:
#   Analyzes extracted images and categorizes them by size and format to help
#   identify which are likely touchscreen UI screenshots vs diagrams/photos.
#
#===============================================================================

set -euo pipefail

if [ $# -ne 1 ]; then
    echo "Usage: $0 directory/"
    echo ""
    echo "Example:"
    echo "  $0 docs/ui-research/bambu-x1c/main-screen/"
    exit 1
fi

INPUT_DIR="$1"

if [ ! -d "$INPUT_DIR" ]; then
    echo "Error: Directory not found: $INPUT_DIR"
    exit 1
fi

echo "Analyzing images in: $INPUT_DIR"
echo ""

# Count by format
echo "=== Image Formats ==="
for ext in jpg jpeg png tif tiff; do
    count=$(ls -1 "$INPUT_DIR"/*.$ext 2>/dev/null | wc -l | tr -d ' ')
    if [ "$count" -gt 0 ]; then
        echo "  $ext: $count files"
    fi
done
echo ""

# Analyze file sizes (likely UI screenshots are typically 100KB - 5MB)
echo "=== File Size Analysis ==="
echo "Tiny (<50KB) - likely icons/logos:"
find "$INPUT_DIR" -type f \( -name "*.jpg" -o -name "*.png" -o -name "*.tif" \) -size -50k | wc -l | xargs echo "  "
echo ""
echo "Small (50KB-500KB) - could be UI screenshots:"
find "$INPUT_DIR" -type f \( -name "*.jpg" -o -name "*.png" -o -name "*.tif" \) -size +50k -size -500k | wc -l | xargs echo "  "
echo ""
echo "Medium (500KB-5MB) - likely UI screenshots or photos:"
find "$INPUT_DIR" -type f \( -name "*.jpg" -o -name "*.png" -o -name "*.tif" \) -size +500k -size -5M | wc -l | xargs echo "  "
echo ""
echo "Large (>5MB) - likely high-res photos, not UI:"
find "$INPUT_DIR" -type f \( -name "*.jpg" -o -name "*.png" -o -name "*.tif" \) -size +5M | wc -l | xargs echo "  "
echo ""

# Use imagemagick to get dimensions (if available)
if command -v identify >/dev/null 2>&1; then
    echo "=== Image Dimensions Sample (first 5) ==="
    find "$INPUT_DIR" -type f \( -name "*.jpg" -o -name "*.png" -o -name "*.tif" \) | head -5 | while read -r file; do
        dims=$(identify -format "%wx%h" "$file" 2>/dev/null || echo "unknown")
        size=$(du -h "$file" | cut -f1)
        basename=$(basename "$file")
        echo "  $basename: ${dims} (${size})"
    done
    echo ""
fi

echo "=== Recommendations ==="
echo "1. Review 'Tiny' files - likely logos, delete most"
echo "2. Check 'Small' files - may include UI screenshots"
echo "3. Focus on 'Medium' files - most likely to be UI"
echo "4. Check 'Large' files - usually product photos, not UI"
echo ""
echo "Touchscreen UIs are typically:"
echo "  - 800x480 to 1280x720 resolution"
echo "  - 100KB to 2MB file size"
echo "  - PNG or JPG format"
