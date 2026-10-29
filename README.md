# Quirewise external consumer template

This project lives outside the `app/` and `libraries/` source repositories. It
uses only installed Quirewise package configs, imported targets, public headers,
the C standard library, and checked-in input fixtures.

Each executable discovers one package with `find_package`, links only that
package's imported target, includes only its installed public headers, and calls
one of its public APIs. The default package list covers all 15 current
packages. A single package can be selected while validating an individual
installation prefix:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH=C:/quirewise/install `
  -DQUIREWISE_CONSUMER_PACKAGES=Image
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

To exercise every package from one installed prefix, omit
`QUIREWISE_CONSUMER_PACKAGES` or pass the full semicolon-separated package list.
When static and shared package variants are available, configure them in separate
build directories. The current library targets are static, so shared evidence is
still pending. The repository does not add source-tree include paths,
third-party headers, third-party link names, or calls into `app/`.

The current installed Image package is not yet dependency-free: its package
config invokes `find_dependency(PkgConfig)` and requests JPEG, zlib, WebP, and
OpenJPEG modules. The successful Image configure log and resulting link command
preserve that boundary failure instead of hiding it in this consumer project.

The consumer programs are intentionally small. Their fixtures cover ZIP/CBZ,
WAV, JSON, EML, EPUB, BMP, PDF, PDG image input, CSV, SRT, plain text, and SVG.
`Runtime` exercises its public buffer allocation API because it has no file
format surface.

| Package | Public API entry point | Input |
|---|---|---|
| Archive | `qw_zip_archive_open` | `archive.zip` |
| Audio | `qw_audio_reader_open` | `tone.wav` |
| Comic | `qw_comic_archive_open` | `sample.cbz` |
| Compression | `qw_compression_inflate_zlib` | RFC 1950 known-answer stream |
| Data | `qw_data_parse` | inline JSON |
| Document | `qw_email_parse` | inline EML |
| Ebook | `qw_epub_parse` | `sample.epub` |
| Image | `qw_image_decode` | `pixel.bmp` |
| Page | `qw_pdf_parser_open` | `one-page.pdf` |
| PDG | `pdg_decode_memory` | `pixel.bmp` |
| Runtime | `qw_runtime_buffer_alloc` | 32-byte buffer |
| Spreadsheet | `qw_spreadsheet_parse` | inline CSV |
| Subtitle | `qw_subtitle_parse` | inline SRT |
| Text | `qw_plain_text_parse` | inline plain text |
| Vector | `qw_svg_decode` | inline SVG |
