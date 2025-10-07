#!/usr/bin/env bash
#===============================================================================
# Extract Frames from Video (YouTube or local)
#===============================================================================
#
# USAGE:
#   ./extract-video-frames.sh "VIDEO_URL_OR_PATH" output_directory/ [interval]
#
# REQUIREMENTS:
#   - yt-dlp: brew install yt-dlp
#   - ffmpeg: brew install ffmpeg
#
# DESCRIPTION:
#   Downloads a YouTube video (or uses local video) and extracts frames at
#   specified intervals. Useful for capturing UI screenshots from video reviews.
#
# PARAMETERS:
#   - interval: Seconds between frames (default: 5)
#
#===============================================================================

set -euo pipefail

if [ $# -lt 2 ]; then
    echo "Usage: $0 VIDEO_URL_OR_PATH output_directory/ [interval_seconds]"
    echo ""
    echo "Examples:"
    echo "  # Extract from YouTube every 5 seconds"
    echo "  $0 'https://youtube.com/watch?v=...' docs/ui-research/bambu-x1c/misc/ 5"
    echo ""
    echo "  # Extract from local video every 2 seconds"
    echo "  $0 local-video.mp4 docs/ui-research/creality-k1/main-screen/ 2"
    exit 1
fi

VIDEO_INPUT="$1"
OUTPUT_DIR="$2"
INTERVAL="${3:-5}"  # Default 5 seconds

mkdir -p "$OUTPUT_DIR"

# Check if input is a URL or local file
if [[ "$VIDEO_INPUT" =~ ^https?:// ]]; then
    echo "Downloading video from: $VIDEO_INPUT"
    TEMP_VIDEO="$OUTPUT_DIR/temp_video.mp4"
    yt-dlp "$VIDEO_INPUT" -o "$TEMP_VIDEO" --format 'best[ext=mp4]'
    VIDEO_FILE="$TEMP_VIDEO"
else
    if [ ! -f "$VIDEO_INPUT" ]; then
        echo "Error: Video file not found: $VIDEO_INPUT"
        exit 1
    fi
    VIDEO_FILE="$VIDEO_INPUT"
fi

echo "Extracting frames every $INTERVAL seconds..."
echo "Output directory: $OUTPUT_DIR"

# Extract frames at specified interval
# fps=1/5 means 1 frame every 5 seconds
ffmpeg -i "$VIDEO_FILE" -vf "fps=1/$INTERVAL" "$OUTPUT_DIR/frame_%04d.png" -hide_banner -loglevel error

# Clean up temporary video if we downloaded it
if [[ "$VIDEO_INPUT" =~ ^https?:// ]]; then
    rm -f "$TEMP_VIDEO"
fi

# Count extracted frames
COUNT=$(ls -1 "$OUTPUT_DIR"/frame_*.png 2>/dev/null | wc -l | tr -d ' ')
echo ""
echo "✓ Extracted $COUNT frames to $OUTPUT_DIR"
echo ""
echo "Next steps:"
echo "  1. Review extracted frames"
echo "  2. Delete frames without UI (transitions, black screens, etc.)"
echo "  3. Rename relevant screenshots: main-01.png, settings-02.png, etc."
echo "  4. Update docs/ui-research/SOURCES.md with video URL and timestamp info"
