# Quirewise external consumer template

This project lives outside the `app/` and `libraries/` source repositories. It
uses only installed Quirewise package configs, imported targets, public headers,
the C standard library, and checked-in input fixtures.

Each executable discovers one package with `find_package`, links only that
package's imported target, includes only its installed public headers, and calls
one or more public APIs. The default package list covers all 17 current
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
Static and shared package variants are consumed from separate build directories;
both modes are covered by the release acceptance runner. The repository does not
add source-tree include paths, third-party headers, third-party link names, or
calls into `app/`.

The consumer programs are intentionally small. Their fixtures cover ZIP/CBZ,
WAV, JSON, EML, EPUB, BMP, PDF, PDG image input, CSV, SRT, plain text, SVG,
character conversion, compression and crypto known-answer vectors.
`Runtime` exercises its public buffer allocation API because it has no file
format surface. Coverage is tracked by `app/tools/check_consumer_coverage.py`;
the current mapping covers every exported public function.

| Package | Public API entry point | Input |
|---|---|---|
| Archive | `qw_zip_archive_open` | `archive.zip` |
| Audio | `qw_audio_reader_open` | `tone.wav` |
| Charset | `qw_charset_convert` | inline UTF-8 text |
| Comic | `qw_comic_archive_open` | `sample.cbz` |
| Compression | `qw_compression_inflate_zlib` | RFC 1950 known-answer stream |
| Crypto | `qw_crypto_md5` | known-answer vector |
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

## Build and verify

```powershell
# build/install the libraries first (or use app/tools/run_acceptance.py)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH=C:/quirewise/install
cmake --build build
ctest --test-dir build --output-on-failure
```

The release acceptance runner builds this project after installing all
libraries:

```powershell
python ../app/tools/run_acceptance.py --workspace ..
```

## License

MIT. See [LICENSE](LICENSE).
