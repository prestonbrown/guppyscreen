#!/usr/bin/env bash
#===============================================================================
# Extract Images from PDF Files
#===============================================================================
#
# USAGE:
#   ./extract-pdf-images.sh input.pdf output_directory/
#
# REQUIREMENTS:
#   - poppler (for pdfimages): brew install poppler
#
# DESCRIPTION:
#   Extracts all images from a PDF file and saves them to the specified
#   output directory. Useful for extracting UI screenshots from printer manuals.
#
#===============================================================================

set -euo pipefail

if [ $# -ne 2 ]; then
    echo "Usage: $0 input.pdf output_directory/"
    echo ""
    echo "Example:"
    echo "  $0 bambu-manual.pdf docs/ui-research/bambu-x1c/main-screen/"
    exit 1
fi

INPUT_PDF="$1"
OUTPUT_DIR="$2"

if [ ! -f "$INPUT_PDF" ]; then
    echo "Error: Input PDF not found: $INPUT_PDF"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

echo "Extracting images from: $INPUT_PDF"
echo "Output directory: $OUTPUT_DIR"

# Extract all images with original format preserved
pdfimages -all "$INPUT_PDF" "$OUTPUT_DIR/extracted"

# Count extracted files
COUNT=$(ls -1 "$OUTPUT_DIR" | wc -l | tr -d ' ')
echo ""
echo "✓ Extracted $COUNT images to $OUTPUT_DIR"
echo ""
echo "Next steps:"
echo "  1. Review extracted images"
echo "  2. Delete non-UI images (diagrams, logos, etc.)"
echo "  3. Rename relevant screenshots: main-01.jpg, print-status-01.jpg, etc."
echo "  4. Update docs/ui-research/SOURCES.md with source info"
