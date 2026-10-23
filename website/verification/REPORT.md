# Hide Icons · verification · 23 October 2026

Production source builds from `website/` into the existing `docs/` Pages directory.

## Bugs corrected

- Removed the zero-height `#top` rule from the content hero. At narrow widths it collapsed the first section and placed later sections over its text and artwork.
- Preserved standard and Safari-prefixed backdrop blur in the compiled CSS. The original declaration order caused the CSS compiler to retain only the Safari path, leaving readable page text behind the translucent menu in the test browser.
- Moved the GitHub link's base display rule before responsive visibility rules, keeping the compact header clear while preserving its desktop hit area.
- Added the documented `npm run brand` command and a production-output check for both backdrop-filter paths.

## Browser checks completed

- Seven rendered dimensions: 320×740, 390×844, 768×1024, 844×390, 1280×720, 1440×900, and 1920×1080. Section-child containment, horizontal overflow, header-control collisions and scroll width checks pass at every size.
- Mobile menu opening, scroll locking, Escape dismissal, focus restoration and Overview anchor alignment pass. Full smooth scrolling and resize alignment were also checked on the shared implementation; manual scrolling releases the stored anchor.
- Every available gallery and installation tab was clicked. Gallery arrow-key selection, full-image loading and Escape/focus restoration were checked where screenshots exist. FAQ disclosure opens correctly. Authentic captures load completely.
- Reduced-motion diagnostics render a static pose. With canvas deliberately unavailable the complete authored SVG remains visible. Full-motion artwork accepts four simultaneous keyboard-triggered waves; animation drawing stops offscreen.
- Browser console inspection returned no warnings or errors during the recorded interaction checks.
- Evidence: `browser-results.json`, `layout-390.png`, `layout-1440.png` and, where present, `menu-390.png`.

## Original bug-fix timing sample, before component alignment

The local production build on this host, without CPU or network throttling, reported LCP **116 ms**, CLS **0**, animation frame median **33.3 ms** and draw-cost p95 **4.80 ms** during the short recorded animation sample. Rendering is intentionally capped at 30fps. These are warm local measurements, not field metrics or a production-network guarantee; INP was not measured.

## Build and content checks

- Strict TypeScript, production build and output validation pass; `final-checks.log` records the commands.
- 19 shared logic tests pass in this independent project: anchor/resize, deterministic geometry and loop continuity, independent bounded waves, strict release URL validation and renderer lifecycle/fallback behavior.
- All configured artifacts match official release **v1.8.1**, recorded in `release-snapshot.json`. Published fallback links remain usable without the live API.
- Canonical/Open Graph/Twitter metadata, exact CSP hashes, assets, 1200×630 social PNG, existing anchors, CNAME, original public assets, robots, sitemap and output manifests pass validation. Geist and its OFL license remain self-hosted; no analytics ID was invented.
- Product copy and attribution remain tied to the original README, installation and release documentation. Native application sources and release automation are untouched.

## Limits

The browser checks use viewport emulation in the in-app browser. Physical iPhone/iPad/Android devices, Safari/WebKit and Firefox engines, screen-reader announcements, actual native app installation, login, mailbox delivery and production-device performance were not tested. Reduced motion and canvas unavailability were exercised through the documented diagnostic branches. No-JavaScript availability was checked in generated HTML rather than by disabling JavaScript in the browser.

Preview: **http://127.0.0.1:4177/**. Production: **https://hideicons.emp0ry.com/**.

## Shared component follow-up

The subsequent brand-alignment checks and current screenshots are documented in `BRAND.md` and `brand-results.json`. Those results supersede the earlier component styles.
