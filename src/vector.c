#include <quirewise/svg.h>
#include <quirewise/svgz.h>

#include <stdint.h>
#include <string.h>

int main(void)
{
    static const uint8_t gzip_svg[] = {
        0x1fu, 0x8bu, 0x08u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0xffu,
        0x01u, 0x06u, 0x00u, 0xf9u, 0xffu, '<', 's', 'v', 'g', '/', '>',
        0x49u, 0xfbu, 0xb9u, 0xacu, 0x06u, 0x00u, 0x00u, 0x00u
    };
    static const char svg[] =
        "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1\" height=\"1\">"
        "<rect width=\"1\" height=\"1\" fill=\"#336699\"/></svg>";
    QwSvgImage image = {0};
    QwSvgzDocument svgz = {0};
    QwSvgStatus status = qw_svg_decode(svg, sizeof(svg) - 1u, &image, NULL);
    if (status != QW_SVG_OK) return 1;
    qw_svg_image_free(&image);
    if (qw_svgz_decode(gzip_svg, sizeof(gzip_svg), &svgz) != QW_SVGZ_OK)
        return 2;
    if (svgz.svg_size != 6u || memcmp(svgz.svg_utf8, "<svg/>", 6u) != 0) {
        qw_svgz_document_free(&svgz);
        return 3;
    }
    qw_svgz_document_free(&svgz);
    {
        uint8_t invalid_gzip[sizeof(gzip_svg)];
        uint8_t trailing_gzip[sizeof(gzip_svg) + 1u];
        memcpy(invalid_gzip, gzip_svg, sizeof(gzip_svg));
        invalid_gzip[21] ^= 1u;
        if (qw_svgz_decode(invalid_gzip, sizeof(invalid_gzip), &svgz) !=
            QW_SVGZ_ERR_DECODE) return 4;
        memcpy(trailing_gzip, gzip_svg, sizeof(gzip_svg));
        trailing_gzip[sizeof(gzip_svg)] = 0u;
        if (qw_svgz_decode(trailing_gzip, sizeof(trailing_gzip), &svgz) !=
            QW_SVGZ_ERR_DECODE) return 5;
    }
    return 0;
}
