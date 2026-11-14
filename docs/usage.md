# External consumer usage guide

This repository is the "outside the source tree" integration test for
Quirewise. It has no Quirewise source checkout dependency; everything comes
from an installed prefix.

## 1. Layout

```text
CMakeLists.txt          package list, find_package loop, targets, CTest
src/<package>.c         one small program per package
fixtures/               small inputs used by the programs
```

The executable for `Document` is `src/document.c`, linked only to
`Quirewise::Document`. The same rule applies to every package.

## 2. Configure and build all packages

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/quirewise/install
cmake --build build
ctest --test-dir build --output-on-failure
```

The build fails if:

- a package config is missing from the prefix;
- a package does not export `Quirewise::<Package>`;
- a public header is missing;
- a consumer source cannot compile against the installed headers;
- a consumer program fails at runtime.

## 3. Configure one package

```sh
cmake -S . -B build-image -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/quirewise/install \
  -DQUIREWISE_CONSUMER_PACKAGES=Image
cmake --build build-image
ctest --test-dir build-image --output-on-failure
```

This is useful when validating a package in isolation, or when a partial
install intentionally contains only some packages.

## 4. Add a consumer

### 4.1 CMake

Add the package to the list at the top of `CMakeLists.txt`:

```cmake
set(_quirewise_supported_packages
    Archive Audio Charset Comic Compression Crypto Data Document Ebook Image Page PDG Runtime
    Spreadsheet Subtitle Text Vector Pipeline)
```

No other CMake change is needed: the loop creates `qw_external_consumer_<name>`
and its CTest entry automatically.

### 4.2 Source

Create `src/<package-lowercase>.c`. For example, a minimal `Runtime` consumer:

```c
#include <quirewise/runtime.h>

#include <stdio.h>

int main(void)
{
    void *buffer = qw_runtime_buffer_alloc(32u);
    if (!buffer) return 1;
    qw_runtime_buffer_free(buffer);
    puts("runtime consumer OK");
    return 0;
}
```

Use the installed public header for the package you consume. Do not include
private headers or source-tree paths.

For file-based packages, the CMake target defines
`QUIREWISE_CONSUMER_FIXTURE_DIR`:

```c
#define FIXTURE(name) QUIREWISE_CONSUMER_FIXTURE_DIR "/" name

if (qw_image_decode(FIXTURE("pixel.bmp"), ...) != QW_IMAGE_OK) return 1;
```

### 4.3 Run

```sh
cmake --build build
ctest --test-dir build -R external_<package> --output-on-failure
```

## 5. Fixture rules

- Keep fixtures small and license-compatible.
- Do not generate large outputs in the source tree; use the build directory.
- Remove outputs after the test so repeated CTest runs are deterministic.
- If a fixture is optional, skip explicitly and print why; do not fail silently.

## 6. Static vs shared

Use a separate build directory per install variant:

```sh
cmake -S . -B build-static  -DCMAKE_PREFIX_PATH=/prefix/static
cmake -S . -B build-shared  -DCMAKE_PREFIX_PATH=/prefix/shared
```

Mixing a static prefix and a shared prefix in one CMake cache is not supported.

## 7. Troubleshooting

| Symptom | Check |
|---|---|
| `Could not find a package configuration file` | `CMAKE_PREFIX_PATH` points at the install prefix, not the source tree |
| Target `Quirewise::X` missing | the package's `install(EXPORT ...)` and config were installed |
| Header not found | the package installed `include/quirewise/` |
| Linker cannot find symbols | link the imported target, not a raw `.a` path |
| Runtime file not found | use `QUIREWISE_CONSUMER_FIXTURE_DIR` or an absolute path |

## 8. Coverage

`app/tools/check_consumer_coverage.py --strict` intersects installed exported
symbols with calls in `src/*.c`. A new public API should get a consumer call so
the coverage gate stays at 100%.

## 9. License

MIT. See `LICENSE`.
