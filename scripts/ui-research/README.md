# UI Research Automation Scripts

Helper scripts for gathering reference UI screenshots from PDFs, videos, and websites.

## Prerequisites

Install required tools (macOS):

```bash
brew install ffmpeg poppler imagemagick yt-dlp wget
```

Or run the installation script:

```bash
brew install ffmpeg poppler imagemagick  # Already installed
```

## Scripts

### 1. Extract Images from PDFs

Extract all images from printer manuals and documentation.

```bash
./extract-pdf-images.sh input.pdf output_directory/

# Example
./extract-pdf-images.sh ~/Downloads/bambu-x1c-manual.pdf \
    docs/ui-research/bambu-x1c/main-screen/
```

**Tool used:** `pdfimages` (from poppler)

### 2. Extract Frames from Videos

Capture screenshots from YouTube reviews or local video files.

```bash
./extract-video-frames.sh VIDEO_URL_OR_PATH output_directory/ [interval_seconds]

# Example: YouTube video, 1 frame every 5 seconds
./extract-video-frames.sh 'https://youtube.com/watch?v=ABC123' \
    docs/ui-research/creality-k1/print-status/ 5

# Example: Local video, 1 frame every 2 seconds
./extract-video-frames.sh ~/Downloads/review.mp4 \
    docs/ui-research/flashforge-ad5m-pro/settings/ 2
```

**Tools used:** `yt-dlp` (download), `ffmpeg` (frame extraction)

### 3. Download Images from Websites

Use `wget` to download images from review sites:

```bash
# Download all images from a review page
wget -r -l 1 -H -t 1 -nd -N -np \
    -A jpg,jpeg,png \
    -erobots=off \
    -P docs/ui-research/bambu-x1c/misc/ \
    'https://example.com/review-page'
```

**Tool used:** `wget`

## Workflow

1. **Identify sources** (see `docs/ui-research/SOURCES.md`)

2. **Extract screenshots:**
   ```bash
   # PDFs
   ./extract-pdf-images.sh manual.pdf docs/ui-research/printer-name/category/

   # Videos
   ./extract-video-frames.sh 'VIDEO_URL' docs/ui-research/printer-name/category/ 5
   ```

3. **Curate images:**
   - Delete non-UI images (logos, diagrams, people)
   - Keep only clear UI screenshots
   - Rename descriptively: `main-01.jpg`, `print-status-with-thumbnail.png`

4. **Document sources:**
   - Update `docs/ui-research/SOURCES.md`
   - Note source URL, date, firmware version (if known)

## Directory Structure

```
docs/ui-research/
├── bambu-x1c/
│   ├── main-screen/
│   ├── print-status/
│   ├── file-browser/
│   ├── settings/
│   └── SOURCES.md
├── creality-k1/
│   └── ...
└── flashforge-ad5m-pro/
    └── ...
```

## Tips

### For PDFs
- Manuals often have the clearest UI screenshots
- Look for "Screen Operation" or "Using the Display" sections
- Page ranges can be specified if needed

### For Videos
- Use longer intervals (5-10 seconds) for initial pass
- YouTube reviews often have full UI walkthroughs around 5-10 minute mark
- Look for "unboxing" and "first print" videos

### For Websites
- Review sites (3DWithUs, Tom's Hardware) often have gallery images
- Right-click "Save Image As..." works for individual high-res images
- Use browser DevTools to find original image URLs

## Troubleshooting

**"pdfimages: command not found"**
```bash
brew install poppler
```

**"ffmpeg: command not found"**
```bash
brew install ffmpeg
```

**"yt-dlp: command not found"**
```bash
brew install yt-dlp
```

**Video download fails**
- Check if video is age-restricted or region-locked
- Try updating yt-dlp: `brew upgrade yt-dlp`
- Use `--cookies-from-browser firefox` if logged in required

## See Also

- `docs/ui-research/README.md` - Organization guidelines
- `docs/ui-research/SOURCES.md` - Identified screenshot sources
- `UI_REDESIGN.md` - Full UI redesign documentation
