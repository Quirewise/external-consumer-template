#include <quirewise/compression.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int hex_value(char value)
{
    if (value >= '0' && value <= '9') return value - '0';
    if (value >= 'a' && value <= 'f') return value - 'a' + 10;
    if (value >= 'A' && value <= 'F') return value - 'A' + 10;
    return -1;
}

static size_t decode_hex(const char *hex, uint8_t *output, size_t capacity)
{
    size_t length = strlen(hex);
    if ((length & 1u) || length / 2u > capacity) return 0u;
    for (size_t i = 0u; i < length / 2u; ++i) {
        int high = hex_value(hex[i * 2u]);
        int low = hex_value(hex[i * 2u + 1u]);
        if (high < 0 || low < 0) return 0u;
        output[i] = (uint8_t)((high << 4) | low);
    }
    return length / 2u;
}

int main(void)
{
    static const uint8_t zlib_abc[] = {
        0x78u, 0x01u, 0x01u, 0x03u, 0x00u, 0xfcu, 0xffu,
        'a', 'b', 'c', 0x02u, 0x4du, 0x01u, 0x27u
    };
    static const uint8_t expected[] =
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n"
        "Quirewise 7z raw LZMA fixture\n";
    static const char lzma1_hex[] =
        "00289d4927558dc1605540399fcd93704cde5761cb836d0e855ab19589d6f409"
        "32219e3ed7f3fffff5b0c000";
    static const char lzma2_hex[] =
        "e0012b00265d00289d4927558dc1605540399fcd93704cde5761cb836d0e855ab"
        "19589d6f40932219e28ea000000";
    static const uint8_t xz_stream[] = {
        0xfdu, 0x37u, 0x7au, 0x58u, 0x5au, 0x00u, 0x00u, 0x04u,
        0xe6u, 0xd6u, 0xb4u, 0x46u, 0x02u, 0x00u, 0x21u, 0x01u,
        0x16u, 0x00u, 0x00u, 0x00u, 0x74u, 0x2fu, 0xe5u, 0xa3u,
        0x01u, 0x00u, 0x21u, 0x51u, 0x75u, 0x69u, 0x72u, 0x65u,
        0x77u, 0x69u, 0x73u, 0x65u, 0x20u, 0x58u, 0x5au, 0x20u,
        0x6bu, 0x6eu, 0x6fu, 0x77u, 0x6eu, 0x2du, 0x61u, 0x6eu,
        0x73u, 0x77u, 0x65u, 0x72u, 0x20u, 0x66u, 0x69u, 0x78u,
        0x74u, 0x75u, 0x72u, 0x65u, 0x0au, 0x00u, 0x00u, 0x00u,
        0xe9u, 0x96u, 0xabu, 0xbeu, 0x1fu, 0x3eu, 0x94u, 0xc8u,
        0x00u, 0x01u, 0x3au, 0x22u, 0xb6u, 0x2au, 0x4fu, 0xd0u,
        0x1fu, 0xb6u, 0xf3u, 0x7du, 0x01u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x04u, 0x59u, 0x5au
    };
    static const uint8_t xz_expected[] = "Quirewise XZ known-answer fixture\n";
    static const uint8_t lzma1_properties[] = {0x5du, 0u, 0u, 0x10u, 0u};
    static const uint8_t lzma2_properties[] = {0x10u};
    uint8_t zlib_output[3];
    uint8_t lzma1_stream[64];
    uint8_t lzma2_stream[64];
    uint8_t decoded[sizeof(expected) - 1u];
    uint8_t xz_decoded[sizeof(xz_expected) - 1u];
    size_t lzma1_size = decode_hex(lzma1_hex, lzma1_stream,
                                   sizeof(lzma1_stream));
    size_t lzma2_size = decode_hex(lzma2_hex, lzma2_stream,
                                   sizeof(lzma2_stream));
    QwCompression7zCoder coder;

    if (!qw_compression_inflate_zlib(zlib_abc, sizeof(zlib_abc), zlib_output,
                                     sizeof(zlib_output)) ||
        memcmp(zlib_output, "abc", sizeof(zlib_output)) != 0 ||
        qw_compression_crc32("123456789", 9u) != 0xcbf43926u) {
        fputs("Compression external consumer failed\n", stderr);
        return 1;
    }
    if (!qw_compression_inflate_xz(xz_stream, sizeof(xz_stream), xz_decoded,
                                   sizeof(xz_decoded),
                                   64u * 1024u * 1024u) ||
        memcmp(xz_decoded, xz_expected, sizeof(xz_decoded)) != 0) {
        fputs("Compression external XZ consumer failed\n", stderr);
        return 5;
    }
    if (!lzma1_size || !lzma2_size) return 2;
    coder = (QwCompression7zCoder){QW_COMPRESSION_7Z_LZMA1_METHOD,
                                   lzma1_properties,
                                   sizeof(lzma1_properties)};
    if (!qw_compression_inflate_7z_chain(&coder, 1u, lzma1_stream,
            lzma1_size, decoded, sizeof(decoded), 256u * 1024u * 1024u) ||
        memcmp(decoded, expected, sizeof(decoded)) != 0) {
        fputs("Compression external LZMA1 consumer failed\n", stderr);
        return 3;
    }
    coder = (QwCompression7zCoder){QW_COMPRESSION_7Z_LZMA2_METHOD,
                                   lzma2_properties,
                                   sizeof(lzma2_properties)};
    if (!qw_compression_inflate_7z_chain(&coder, 1u, lzma2_stream,
            lzma2_size, decoded, sizeof(decoded), 256u * 1024u * 1024u) ||
        memcmp(decoded, expected, sizeof(decoded)) != 0) {
        fputs("Compression external LZMA2 consumer failed\n", stderr);
        return 4;
    }
    {
        static const uint8_t gzip_hello[] = {
            0x1fu, 0x8bu, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xffu,
            0x01u, 0x05u, 0x00u, 0xfau, 0xffu,
            'h', 'e', 'l', 'l', 'o',
            0x86u, 0xa6u, 0x10u, 0x36u, 0x05u, 0x00u, 0x00u, 0x00u
        };
        uint8_t stored_raw[64];
        uint8_t stored_zlib[64];
        uint8_t gzip_output[5];
        size_t stored_raw_size = 0u;
        size_t stored_zlib_size = 0u;
        size_t bounded_size = 0u;
        if (qw_compression_abi_version() == 0u) return 6;
        if (qw_compression_adler32("123456789", 9u) != 0x091e01deu) return 7;
        if (qw_compression_raw_stored_bound(3u) > sizeof(stored_raw)) return 8;
        if (qw_compression_zlib_stored_bound(3u) > sizeof(stored_zlib)) return 9;
        if (!qw_compression_deflate_raw_stored((const uint8_t *)"abc", 3u,
                                               stored_raw, sizeof(stored_raw),
                                               &stored_raw_size)) return 10;
        if (!qw_compression_inflate_raw(stored_raw, stored_raw_size,
                                        gzip_output, 3u)) return 11;
        if (memcmp(gzip_output, "abc", 3u) != 0) return 12;
        if (qw_compression_inflate_raw_bounded(stored_raw, stored_raw_size,
                                               gzip_output, sizeof(gzip_output),
                                               &bounded_size) !=
                QW_COMPRESSION_INFLATE_OK || bounded_size != 3u) return 13;
        if (!qw_compression_deflate_zlib_stored((const uint8_t *)"abc", 3u,
                                                stored_zlib, sizeof(stored_zlib),
                                                &stored_zlib_size)) return 14;
        if (qw_compression_inflate_zlib_bounded(stored_zlib, stored_zlib_size,
                                                gzip_output, sizeof(gzip_output),
                                                &bounded_size) !=
                QW_COMPRESSION_INFLATE_OK || bounded_size != 3u) return 15;
        if (!qw_compression_inflate_gzip(gzip_hello, sizeof(gzip_hello),
                                         gzip_output, sizeof(gzip_output))) return 16;
        if (memcmp(gzip_output, "hello", sizeof(gzip_output)) != 0) return 17;
        if (!qw_compression_inflate_xz_prefix(xz_stream, sizeof(xz_stream),
                                              xz_decoded, sizeof(xz_decoded),
                                              64u * 1024u * 1024u)) return 18;
        if (memcmp(xz_decoded, xz_expected, sizeof(xz_decoded)) != 0) return 19;
    }
    puts("PASS: installed Compression API including XZ and 7z LZMA1/LZMA2");
    return 0;
}
