# EMP0RY product ecosystem · 23 October 2026

Authority: existing EMP0RY v19, MiruShin and ErScripts product design specifications.

Graphite #09090d, surface #111116, readable off-white #f4f2f7, muted #aaa6b5, restrained lavender #b7a0e5. Self-hosted Geist 400–600 for UI, 900 for the optical lowercase wordmark. 1320px content maximum, fluid 20–72px gutters, four-pixel spacing rhythm. Dark translucent buttons, thin lavender rules, five-pixel corners, 44px minimum controls and visible keyboard focus. Fixed shared header outside a clipped native scroll layer; page content cannot appear above it.

Seven distinct product compositions: Hide Icons uses a dissolving point lattice and quiet editorial columns; AutoHide Cursor uses a cursor silhouette and measured trails with small authentic Windows captures; EID uses a folded point book and actual landscape game captures; Steam Sync uses linked save planes and a deliberate reconciliation diagram; Console uses a point prompt and actual in-game capture with command reference; Fixes uses directional geometry and a clear controls/HUD guide; AltDaemon uses unfolding refresh brackets, an authentication route diagram and authentic AltStore/Shortcuts imagery. Mathematical artwork responds to pointer/keyboard and independent waves. No fake app controls or authentication forms.

Hero type 64–88px on desktop and 40–48px on phones, section type 32–48px, readable 14–18px UI/body. Natural content-sized sections, no empty full-screen product panels or repeated oversized titles. Captures remain complete and modestly sized; expand them in a focus-managed viewer. Installation choices and download formats remain semantic and available without script.

Motion: one calm 24-second point cycle, bounded pointer easing, 160–220ms control transitions. Render at 30fps with capped DPR and fewer phone points; stop offscreen/hidden, clean up listeners/observers/frames, preserve static reduced-motion pose and original SVG fallback. A text entrance must never gate content availability. No visible move/click hints, background pause buttons, custom cursors, noisy full-page particle clouds or imported 3D models.

Accessibility: semantic heading order, native anchors, accessible mobile dialog with Escape/focus restoration, touch-safe controls, gallery tabs with arrow/Home/End keys, descriptive screenshot alt text, one product-support address. Preserve compatibility, cautions, attribution, original domains and public anchors. Do not invent testimonials, installations, versions, platform builds, analytics properties or software guarantees.

The previously inspected frontend-design and emilkowalski-motion workflows inform implementation; the EMP0RY specification is the brand authority. No advertised upstream review/document bundle is claimed. Verify the production build in a browser at phone, tablet, desktop and short-landscape dimensions. Record actual results and distinguish local measurements from physical-device and production-network performance.


## Layout and compiled-style regressions

Content sections always keep their natural height. A fragment ID on a hero must not inherit a zero-height marker rule. Keep the header outside the clipped scroll layer, and test both section containment and header-link collisions after reflow. Put vendor-prefixed backdrop-filter declarations before the standard declaration so the production CSS retains both engine paths. Mobile visibility rules must come after base display rules.
