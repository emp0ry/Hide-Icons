# Hide Icons: shared component alignment

The header, optical wordmark, product label/divider, navigation, button surfaces and type sizes now use the established ErScripts component definitions. Supporting typography, tabs, contact links and the mobile navigation sheet follow the same system. The existing clipped scroll layer remains in place.

## Verification

- Eight rendered dimensions: 320×740, 390×844, 701×900, 768×1024, 844×390, 1280×720, 1440×900 and 1920×1080. No horizontal overflow, section-child escapes or header-control collisions were observed.
- Computed properties compared with MiruShin at 390, 768 and 1440px. The wordmark, dot, divider, product label, navigation type, button, compact CTA, menu control and header agree for the recorded size/weight/tracking/line-height/padding/background/radius/blur/height/alignment properties. Product-specific label widths and hero positions are intentionally excluded.
- Mobile menu opening, Escape focus restoration and menu-to-download navigation pass. The download destination is within one pixel of its intended scroll position. Compact captures now reserve their native widths before loading, avoiding a deferred image-size jump.
- The matching 404 page was rendered on a phone viewport, with no long dashes, overflow or browser errors. The recorded normal-page interactions also produced no warnings/errors.
- Build, strict TypeScript and output checks pass; the existing 19 logic tests pass. `brand-checks.log` records the commands. Titles, copy, metadata and accessibility labels were checked for em/en dashes. Exact package filenames, commands, domains, anchors and licenses remain intact.

Evidence: `brand-results.json`, `brand-390.png`, `brand-1440.png`, and the optional `brand-menu-390.png`.

Checks use the in-app browser with viewport emulation. Separate physical devices and browser engines were not tested. These are styling/layout checks; the timing samples in the earlier report predate this component alignment.
