# Bundled export font

This directory includes Atkinson Hyperlegible Regular for map SVG/PNG/PDF export text.

- Source: https://github.com/googlefonts/atkinson-hyperlegible
- Font file: `AtkinsonHyperlegible-Regular.ttf`
- License: SIL Open Font License 1.1, included as `OFL.txt`

The app loads this TTF at export time so downloaded SVG files can embed it as a data URI and PDF export can register the same font with jsPDF.
