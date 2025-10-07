# UI Screenshot Curation Report

**Generated:** 2025-10-07  
**Status:** Awaiting Manual Review  
**Total Images:** 199

---

## Executive Summary

Automated extraction successfully retrieved 199 images from official printer manuals. However, **most images are NOT touchscreen UI screenshots** - they include product photos, assembly diagrams, icons, and other manual content.

**Estimated UI Screenshots:** 15-25 out of 199 (8-13%)

**Action Required:** Manual curation to identify and isolate actual touchscreen UI images.

---

## Image Analysis by Printer

### 1. Bambu Lab X1C (31 images)

**Location:** `docs/ui-research/bambu-x1c/main-screen/`

**File Pattern Analysis:**
3.4K CHANGELOG.md
7.3K CLAUDE.md
9.7K DEVELOPMENT.md
34K LICENSE
14K Makefile
7.3K README.md
14K UI_REDESIGN.md
12B VERSION
288B assets
96B build
192B debian
160B docs
684B guppyconfig-simulator.json
128B k1
1.4K libhv

**Findings:**
- **Large TIF files (6.2MB):** High-resolution product/setup photos - NOT UI
- **Medium TIF files (1.6MB):** Setup instruction photos - likely NOT UI  
- **Tiny PNG files (1.5KB):** Icons or logos - NOT UI
- **Source:** Quick Start Guide focuses on setup, not screen operation

**Estimated UI Screenshots:** 2-5 images max

**Recommendation:**  
- DELETE all files > 3MB (product photos)
- DELETE all files < 10KB (icons)
- REVIEW remaining medium-sized images
- **Need better source** - Find Screen Operation Guide or video walkthrough

---

### 2. FlashForge Adventurer 5M Pro (163 images)

**Location:** `docs/ui-research/flashforge-ad5m-pro/main-screen/`

2.1K extracted-000.png
1.5M extracted-001.tif
1.5M extracted-002.tif
1.5M extracted-003.tif
1.5M extracted-004.tif
1.5M extracted-005.tif
1.5M extracted-006.tif
1.5M extracted-007.tif
1.3M extracted-008.tif
2.1K extracted-009.jpg
1.3M extracted-010.tif
2.1K extracted-011.jpg
1.5M extracted-012.tif
8.0K extracted-013.png
1.5M extracted-014.tif
1.5M extracted-015.tif
1.5M extracted-016.tif
1.5M extracted-017.tif
1.5M extracted-018.tif
1.5M extracted-019.tif

**File Size Distribution:**
  Medium(1-5M): 95 files
  Tiny(<100K): 68 files

**Findings:**
- 163 images from 72-page User Manual
- Mix of: assembly diagrams, wiring, safety warnings, part photos, UI screenshots
- Many tiny PNGs are icons or UI elements (not full screenshots)
- Some JPGs are actual photos of the touchscreen
- TIF files are typically diagrams or schematics

**Estimated UI Screenshots:** 10-20 images

**High-Priority Files to Check:**
- PNG files 50-500KB (likely UI elements or screenshots)
- JPG files 100KB-2MB (could be UI photos)
- Files named with numbers in 30-80 range (manual's UI section likely mid-document)

**Recommendation:**
- DELETE: Large TIF files (>2MB) - diagrams
- DELETE: Tiny PNG files (<10KB) - icons
- KEEP & REVIEW: Medium PNG/JPG (50KB-2MB)
- Focus on extracted-030 through extracted-080 range

---

### 3. Creality K1 (5 images)

**Location:** `docs/ui-research/creality-k1/main-screen/`

total 192
-rw-r--r--@ 1 pbrown  staff    14K Apr 11  2024 1712820364-bpfull.jpg
-rw-r--r--@ 1 pbrown  staff    50K Oct 26  2022 3D-Printing-for-Mental-Health-and-Suicide-Prevention-300px.jpg
-rw-r--r--@ 1 pbrown  staff   9.4K Jun 15  2021 3DWithUs-3D-Transparent-Logo-100.png
-rw-r--r--@ 1 pbrown  staff   2.5K Jul 30  2020 3DWithUs-Logo-Square-400x400-100x100.jpg
-rw-r--r--@ 1 pbrown  staff    10K Jul 30  2020 3DWithUs-Logo-Square-400x400-300x300.jpg

**Findings:**
- Only 5 images from website scrape
- 4 are site logos/branding (3DWithUs logos)
- 1 is article banner image (mental health awareness)
- **ZERO actual UI screenshots**

**Estimated UI Screenshots:** 0

**Recommendation:**
- DELETE all 5 current images
- **Need new source:**
  - Download Creality K1 Official Manual PDF
  - Find YouTube "Creality K1 menu walkthrough" videos
  - Check Tom's Hardware / HowToMechatronics reviews (known to have UI photos)

---

## Curation Workflow

### Step 1: Quick Cleanup (Delete Obvious Non-UI)

**Bambu X1C:**
```bash
cd docs/ui-research/bambu-x1c/main-screen/
# Delete large product photos
rm extracted-00{0,1,2}.tif  # 6.2MB files
# Delete tiny icons
rm extracted-0{04,06,08,10,12,14,16,18,20}.png  # 1.5KB files
```

**FlashForge AD5M Pro:**
```bash
cd docs/ui-research/flashforge-ad5m-pro/main-screen/
# Delete all files > 3MB (product photos/diagrams)
find . -type f -size +3M -delete
# Delete all files < 10KB (icons)
find . -type f -size -10k -delete
```

**Creality K1:**
```bash
cd docs/ui-research/creality-k1/main-screen/
# Delete all (none are UI)
rm *.jpg *.png
```

### Step 2: Manual Review (Identify UI Screenshots)

Open remaining images in Preview/image viewer:

1. **Look for:**
   - Touchscreen interface elements (buttons, menus, status)
   - Typical resolutions: 800x480, 1024x600, 1280x720
   - Clear text and icons (not blurry)
   - Dark or light theme with clear visual hierarchy

2. **Reject:**
   - Assembly diagrams (line drawings)
   - Wiring schematics
   - Safety warning icons
   - Product exterior photos
   - Blurry or partial captures

### Step 3: Organize Keepers

Move UI screenshots to appropriate subdirectories:

```bash
# Example for FlashForge
mv extracted-042.png ../print-status/ff-print-status-01.png
mv extracted-056.jpg ../main-screen/ff-main-menu-01.jpg  
mv extracted-071.png ../settings/ff-settings-wifi-01.png
```

### Step 4: Document Sources

Update `SOURCES.md` with:
- Which images came from which PDF pages
- Firmware version (if identifiable)
- Date extracted
- Original filename → renamed filename mapping

---

## Alternative Sources Needed

### Bambu Lab X1C (HIGH PRIORITY)
- **Official Screen Operation Guide:**  
  https://wiki.bambulab.com/en/x1/manual/screen-operation  
  (Text-only, but may link to images)
  
- **YouTube Videos:**
  - Search: "Bambu Lab X1C firmware 01.09 UI walkthrough"
  - Search: "Bambu X1C touchscreen menu tour"
  - Use `./scripts/ui-research/extract-video-frames.sh` to capture

- **Reddit/Forums:**  
  Users often post photos of screens showing issues

### Creality K1 (HIGH PRIORITY)
- **Official Manual PDF:**  
  Need to find direct PDF download from Creality Wiki

- **Review Sites with Photos:**
  - Tom's Hardware K1 Review
  - HowToMechatronics K1 Max Review  
  - 3DWithUs (revisit for actual UI images)

- **YouTube Videos:**
  - Search: "Creality K1 menu walkthrough"
  - Search: "K1 touchscreen tutorial"

### FlashForge AD5M Pro (MEDIUM PRIORITY)
- Current extraction likely has UI screenshots
- Focus curation efforts on existing 163 images first
- YouTube as backup if needed

---

## Next Actions

### Immediate (Do First)
1. ✅ **Quick cleanup** - Run delete commands above (~5 min)
2. ✅ **Manual review** - Open remaining images, identify UI screenshots (~30-60 min)
3. ✅ **Organize** - Move keepers to appropriate directories (~15 min)

### Short-term (This Week)
4. ⏳ **Get better Bambu source** - Find Screen Operation images or video
5. ⏳ **Get Creality K1 sources** - Official manual PDF + review site images
6. ⏳ **Extract additional content** - YouTube videos if needed

### Documentation (After Curation)
7. 📝 **Update SOURCES.md** - Document what we kept and why
8. 📝 **Create design analysis** - Compare UI patterns across printers
9. 📝 **Draft GuppyScreen redesign concepts** - Based on research

---

## Success Criteria

**Before moving to design phase, we need:**
- ✅ At least 5 clear UI screenshots from Bambu X1C (firmware 01.09+)
- ✅ At least 5 clear UI screenshots from Creality K1
- ✅ At least 8 clear UI screenshots from FlashForge AD5M Pro
- ✅ Screenshots organized by screen type (main, print-status, settings, etc.)
- ✅ Source documentation complete

**Current Status:**
- Bambu X1C: 2-5 UI screenshots estimated ❌ (need 5+)
- Creality K1: 0 UI screenshots ❌ (need 5+)
- FlashForge: 10-20 UI screenshots estimated ✅ (have enough)

---

## Tools & Scripts Available

- `scripts/ui-research/extract-pdf-images.sh` - Extract from PDFs
- `scripts/ui-research/extract-video-frames.sh` - Grab frames from videos  
- `scripts/ui-research/analyze-images.sh` - Analyze image sizes/formats
- `wget` - Download images from websites

---

## Estimated Time Investment

- **Quick Cleanup:** 5-10 minutes
- **Manual Curation:** 1-2 hours (reviewing 199 images)
- **Getting Better Sources:** 1-2 hours (finding, downloading, extracting)
- **Organization & Docs:** 30-60 minutes

**Total:** 3-5 hours of focused work

---

*Generated: 2025-10-07*  
*Ready for manual review and curation*
