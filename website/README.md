# Hide Icons website

Product site: **https://hideicons.emp0ry.com/**. Source: `/Users/emp0ry/Projects/С++/Hide-Icons/website`. The application and its release pipeline are independent of this website.

## Build and preview

Requires Node.js 24 or newer and npm. From this directory:

```sh
npm ci
npm run dev
npm test
npm run build
npm run check:output
npm run preview
```

Development uses `http://127.0.0.1:5177/`; production preview uses **http://127.0.0.1:4177/**. Dependencies are pinned with an npm lockfile. Vite builds into `dist/` and synchronizes managed output into `../docs/` for the existing GitHub Pages configuration. Commit generated HTML and `docs/site/` together. No runtime server is required by GitHub Pages.

`pages-files.json` records managed files. The sync script removes only obsolete managed hashes and preserves the original domain, icons, screenshot URLs and documentation. Old public CSS/JS URLs remain available for compatibility; new pages use fingerprinted assets under `site/build/`.

## Edit

- `src/index.html`: semantic product copy, working release links, screenshots/diagrams, installation and compatibility. Content remains available without JavaScript.
- `src/404.html`: the matching not-found page.
- `src/tokens.css`, `styles.css`, `brand.css`, `captures.css`: self-hosted Geist, shared graphite/lavender controls and responsive product composition.
- `src/main.ts`: accessible tabs, focus-managed mobile navigation, screenshot viewer and native anchors. No working mini-app or account login is simulated.
- `src/core/reticle-field.ts`, `reticle.ts`, `pulses.ts`: original icons point artwork. A 24-second cycle, independent bounded waves, pointer/keyboard input, 30fps rendering cap, DPR cap, offscreen/hidden pausing, static reduced motion and SVG fallback.
- `src/core/releases.ts`: official GitHub release validation. Published download links work without the live API.
- `site.config.json`: identity and verified asset names used by brand/output checks.

No original analytics property was present on this website, so none is invented. Canonical, Open Graph/Twitter, application structured data, strict CSP, manifest, robots and sitemap are included. Public support uses `support@emp0ry.com`.

See [DESIGN.md](DESIGN.md), [ASSETS.md](ASSETS.md) and [verification/REPORT.md](verification/REPORT.md). Application licensing and upstream attribution remain separate from Geist’s SIL OFL license.

## Artwork diagnostics

`/?reduce-motion` keeps a static pose; `/?no-canvas` retains the authored SVG. Normal builds use checked-in artwork. Regenerating it requires Python `fonttools[woff]` and ImageMagick:

```sh
npm run brand
npm run build
```

The social SVG uses outlined Geist and mathematical point geometry, converted to a 1200×630 PNG. Production builds were checked in the in-app browser at seven viewport sizes. See `verification/REPORT.md`, `browser-results.json`, and the 390/1440px screenshots for evidence and limitations.

The shared product header and controls match the established ErScripts/MiruShin components. See `verification/BRAND.md` and `brand-results.json` for the measured parity checks. Product compositions and artwork remain individual. Website copy, titles and accessibility labels use no long dashes.
