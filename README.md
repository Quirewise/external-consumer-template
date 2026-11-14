# Quirewise external consumer template

An out-of-tree C project that consumes only installed Quirewise packages. It
proves that every library is usable through its CMake config, imported target
and public headers, without the application source tree or private include
paths.

Each executable discovers one package with `find_package`, links only that
package's imported target, and calls its public API. The default set covers all
18 packages.

## Packages and fixtures

| Package | Example entry point | Fixture |
|---|---|---|
| Archive | `qw_zip_archive_open` | `archive.zip` |
| Audio | `qw_audio_reader_open` | `tone.wav` |
| Charset | `qw_charset_convert` | inline UTF-8 |
| Comic | `qw_comic_archive_open` | `sample.cbz` |
| Compression | `qw_compression_inflate_zlib` | RFC 1950 known answer |
| Crypto | `qw_crypto_md5_digest` | known answer |
| Data | `qw_data_parse` | inline JSON |
| Document | `qw_email_parse` | inline EML |
| Ebook | `qw_epub_parse` | `sample.epub` |
| Image | `qw_image_decode` | `pixel.bmp` |
| Page | `qw_ceb_decrypt_to_pdf` | `synthetic-ceb.ceb` |
| PDG | `pdg_decode_memory` | `pixel.bmp` |
| Pipeline | `qw_pipeline_convert_file` | `one-page.pdf` |
| Runtime | `qw_runtime_buffer_alloc` | 32-byte buffer |
| Spreadsheet | `qw_spreadsheet_parse` | inline CSV |
| Subtitle | `qw_subtitle_parse` | inline SRT |
| Text | `qw_plain_text_parse` | inline text |
| Vector | `qw_svg_decode` | inline SVG |

## Build and test

Install the Quirewise libraries first, then:

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/quirewise/install
cmake --build build
ctest --test-dir build --output-on-failure
```

Select a single package while validating one installation prefix:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/prefix \
  -DQUIREWISE_CONSUMER_PACKAGES=Image
```

Select several packages with a semicolon-separated list:

```sh
-DQUIREWISE_CONSUMER_PACKAGES="Document;Pipeline;Subtitle"
```

Static and shared installs should be consumed from separate build directories.

## How it works

The supported package list and per-package executable are in
[CMakeLists.txt](CMakeLists.txt). For each selected package, CMake:

1. calls `find_package(Quirewise<Package> CONFIG REQUIRED)`;
2. verifies that the imported target `Quirewise::<Package>` exists;
3. builds `src/<package-lowercase>.c` and links only that target;
4. adds a CTest entry.

A package that installs incorrectly, exports the wrong target, or depends on a
private include path fails here before it reaches the application.

## Fixtures

`fixtures/` contains small, license-compatible inputs used by the consumer
programs. Tests write outputs only to their build directory and clean up.

## Adding a package

1. Add the package name to `_quirewise_supported_packages` in
   [CMakeLists.txt](CMakeLists.txt).
2. Add `src/<package-lowercase>.c` with a `main` that calls one or more public
   APIs; use `QUIREWISE_CONSUMER_FIXTURE_DIR` for fixture paths.
3. Run the build and CTest.

See [docs/usage.md](docs/usage.md) for a worked example and troubleshooting.

## Validation

The release acceptance runner builds and tests this project after installing
all libraries:

```sh
python app/tools/run_acceptance.py --workspace .
```

Coverage mapping is checked by
`app/tools/check_consumer_coverage.py --strict`.

## License

MIT. See [LICENSE](LICENSE).
