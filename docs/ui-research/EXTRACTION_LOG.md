# UI Screenshot Extraction Log

**Date:** 2025-10-07
**Status:** Initial extraction complete
**Total Images:** 199

---

## Bambu Lab X1C

**Source:** Official Quick Start Guide PDF
**URL:** https://cdn1.bambulab.com/documentation/Quick%20Start%20Guide%20for%20X1-Carbon%20Combo.pdf
**Downloaded:** 47MB PDF
**Extracted:** 31 images (TIF + PNG formats)
**Location:** `docs/ui-research/bambu-x1c/main-screen/`
**Tool Used:** `pdfimages` (poppler)

**Notes:**
- Mix of TIF (6.2MB, 1.6MB sizes) and small PNG (1.5KB) files
- Larger TIFs likely are high-res setup photos
- Smaller PNGs might be UI icons or small screenshots
- Need to review and identify actual touchscreen UI images
- Quick Start Guide focuses on setup, may have limited screen UI images

**Next Steps:**
- Review extracted images
- Identify touchscreen UI screenshots vs setup photos
- Move UI-only images to appropriate subdirectories
- Delete non-UI content (product photos, diagrams)

---

## Creality K1

**Source:** 3DWithUs Review Article
**URL:** https://3dwithus.com/creality-k1-review-3d-printer-testing-settings-and-tips
**Downloaded:** 5 images (JPG + PNG)
**Location:** `docs/ui-research/creality-k1/main-screen/`
**Tool Used:** `wget` with image filter

**Images Downloaded:**
1. `1712820364-bpfull.jpg` - Unknown content
2. `3D-Printing-for-Mental-Health-and-Suicide-Prevention-300px.jpg` - Article banner (not UI)
3. `3DWithUs-3D-Transparent-Logo-100.png` - Site logo (not UI)
4. `3DWithUs-Logo-Square-400x400-100x100.jpg` - Site logo (not UI)
5. `3DWithUs-Logo-Square-400x400-300x300.jpg` - Site logo (not UI)

**Notes:**
- Most images are site branding, not UI screenshots
- Article confirmed to have screenshots in text (per WebFetch)
- Need to manually visit page to identify specific UI screenshots
- May need to use browser DevTools to find actual UI image URLs

**Next Steps:**
- Manually identify UI screenshots from article
- Download specific UI image URLs
- May need alternative K1 sources (YouTube, other reviews)

---

## FlashForge Adventurer 5M Pro

**Source:** Official User Manual PDF
**URL:** https://en.fss.flashforge.com/10000/software/591658f80f34f9653eaf43caccaedb14.pdf
**Downloaded:** 12.5MB PDF
**Extracted:** 163 images
**Location:** `docs/ui-research/flashforge-ad5m-pro/main-screen/`
**Tool Used:** `pdfimages` (poppler)

**Notes:**
- 163 images is a LOT - comprehensive manual with many diagrams
- Likely mix of:
  - Touchscreen UI screenshots
  - Assembly diagrams
  - Part photos
  - Wiring diagrams
  - Safety instructions
  - Maintenance illustrations
- Significant curation needed to identify actual UI screenshots
- Manual is 72 pages, so ~2.3 images/page average

**Next Steps:**
- PRIORITY: Curate images - delete non-UI content
- Focus on finding:
  - Main menu screen
  - Print status screen
  - Settings menus
  - File browser
  - Temperature controls
- Organize into appropriate subdirectories

---

## Extraction Tools Used

### PDF Image Extraction
- **Tool:** `pdfimages` from poppler 25.09.1
- **Script:** `scripts/ui-research/extract-pdf-images.sh`
- **Command:** `pdfimages -all input.pdf output_dir/prefix`
- **Formats:** Extracts TIF, PNG, JPG as found in PDF

### Website Image Download
- **Tool:** `wget` with filters
- **Command:**
  ```bash
  wget -r -l1 -H -nd -N -np \
    -A.jpg,.jpeg,.png \
    -erobots=off \
    -P output_dir/ \
    'URL'
  ```
- **Limitation:** Downloads ALL images, not just UI screenshots

---

## Summary Statistics

| Printer             | Source Type | Images | Usable? | Status       |
|---------------------|-------------|--------|---------|--------------|
| Bambu X1C           | PDF Manual  | 31     | TBD     | Review needed|
| Creality K1         | Review Site | 5      | Low     | Need better source|
| FlashForge AD5M Pro | PDF Manual  | 163    | TBD     | High curation needed|
| **TOTAL**           |             | **199**| **TBD** |              |

---

## Next Actions

### Immediate (Curation Phase)
1. **Bambu X1C** (31 images)
   - Open `docs/ui-research/bambu-x1c/main-screen/` in file browser
   - Identify touchscreen UI screenshots
   - Delete setup photos, product shots, non-UI content
   - Rename keepers: `main-01.tif`, `settings-01.png`, etc.

2. **FlashForge AD5M Pro** (163 images)
   - Open `docs/ui-research/flashforge-ad5m-pro/main-screen/`
   - Delete assembly, wiring, safety diagrams
   - Keep only touchscreen UI images
   - Move to appropriate subdirectories
   - Estimate: 10-20 UI screenshots out of 163

3. **Creality K1** (5 images → Need more)
   - Current images are mostly logos
   - **Action:** Find alternative sources
   - Try: YouTube video frame extraction
   - Try: Tom's Hardware / HowToMechatronics reviews

### YouTube Video Sources (To Do)
- Search for: "Creality K1 touchscreen menu"
- Search for: "Bambu X1C firmware 01.09 UI"
- Search for: "FlashForge AD5M Pro interface"
- Use `scripts/ui-research/extract-video-frames.sh` to capture

### Documentation (To Do)
- Update `docs/ui-research/SOURCES.md` with extraction details
- Document firmware versions (if identifiable)
- Note which screens are from which source

---

## Lessons Learned

1. **PDF extraction works well** - Got 194 total images from 2 manuals
2. **Website scraping is hit-or-miss** - Many images are not UI content
3. **Manual curation is essential** - Most images need review/filtering
4. **Need YouTube videos** - Better source for actual screen walkthroughs
5. **FlashForge has most content** - 163 images, but needs heavy curation

---

*Last Updated: 2025-10-07 16:40*
