#include <quirewise/charset.h>

#include <stdio.h>
#include <string.h>

int main(void)
{
    static const uint8_t source[] = {'C', 'a', 'f', 0xe9u, ' ', 0x80u};
    static const uint8_t expected[] = {
        'C', 'a', 'f', 0xc3u, 0xa9u, ' ', 0xe2u, 0x82u, 0xacu
    };
    uint8_t output[16];
    size_t output_size = 0u;
    QwCharsetStatus status = qw_charset_convert(
        "CP1252", "UTF-8", source, sizeof(source), output, sizeof(output),
        &output_size);
    if (status != QW_CHARSET_OK || output_size != sizeof(expected) ||
        memcmp(output, expected, sizeof(expected)) != 0) {
        fprintf(stderr, "Installed Charset consumer failed with status %d\n",
                (int)status);
        return 2;
    }
    puts("PASS: installed Charset Windows-1252 to UTF-8 API");
    return 0;
}
