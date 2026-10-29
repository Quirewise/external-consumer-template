#include <quirewise/compression.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    static const uint8_t zlib_abc[] = {
        0x78u, 0x01u, 0x01u, 0x03u, 0x00u, 0xfcu, 0xffu,
        'a', 'b', 'c', 0x02u, 0x4du, 0x01u, 0x27u
    };
    uint8_t output[3];
    if (!qw_compression_inflate_zlib(zlib_abc, sizeof(zlib_abc), output,
                                     sizeof(output)) ||
        memcmp(output, "abc", sizeof(output)) != 0 ||
        qw_compression_crc32("123456789", 9u) != 0xcbf43926u) {
        fputs("Compression external consumer failed\n", stderr);
        return 1;
    }
    puts("PASS: installed Compression API");
    return 0;
}
