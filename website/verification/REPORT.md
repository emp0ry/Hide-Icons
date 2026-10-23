# Hide Icons · verification · 23 October 2026

Production source is built locally from `website/` into the existing `docs/` Pages directory. Browser verification was explicitly waived by the user for this update.

## Completed checks

- Strict TypeScript and Vite production build; source and output assets validated.
- 19 logic tests covering native anchor/resize behavior, all seven deterministic geometric studies, loop continuity, bounded successive waves, strict release URL validation, renderer visibility/reduced-motion/fallback/disposal behavior. These are shared behavior tests run in this independent project.
- Output checks preserve all prior public section anchors, the exact CNAME, original icons, screenshots and documentation. Generated output and managed-file manifests match.
- Current official release **v1.8.1** checked against GitHub. All configured download formats were present and uploaded. Public release metadata is recorded in `release-snapshot.json`.
- Metadata includes canonical, Open Graph/Twitter, structured application information, CSP hashes, 1200×630 SVG-derived PNG, robots, sitemap and manifest. Geist is self-hosted with its OFL license. No analytics ID is invented.
- Product copy checked against README, installation and release documentation. Native app sources and release automation remain untouched. No package was installed or executed.
- Source review covers 320/390px, tablet, desktop and short-landscape rules; long package filenames and embedding paths wrap, headers stay outside the clipped native scroll layer, controls have visible focus and 44px hit areas, and captures preserve complete aspect ratios. This is source review, not a rendered viewport test.
- Essential copy, downloads, compatibility and all capture/install panels remain in the HTML without JavaScript. Keyboard tabs, focus-managed native dialogs, screenshot expansion, decorative keyboard input and reduced-motion/fallback branches are implemented.
- The original mathematical SVGs and social artwork were rendered with ImageMagick and inspected as a contact sheet. These are artwork renders, not website screenshots.

## Verification limits

No browser screenshot, browser-console inspection, viewport overflow measurement, LCP/CLS/INP measurement, actual frame timing, screen-reader announcement check or physical-device test is claimed. Motion is capped at 30fps/DPR2 by implementation; actual device performance remains unmeasured. No native app installation, login, game command, network scan or mailbox delivery was performed. Product behavior demonstrations are authentic images or explanatory diagrams, rather than working native-app replicas.

## Delivery

Preview: **http://127.0.0.1:4177/**. Production: **https://hideicons.emp0ry.com/**. The README documents independent npm commands. Deployment status and live HTTP asset verification are recorded separately after publication.
