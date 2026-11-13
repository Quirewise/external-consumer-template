#include <quirewise/cad.h>
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
    static const char dxf[] =
        "0\nSECTION\n2\nENTITIES\n"
        "0\nLINE\n10\n0\n20\n0\n11\n10\n21\n0\n"
        "0\nTEXT\n10\n1\n20\n2\n40\n1\n1\nCAD\n"
        "0\nENDSEC\n0\nEOF\n";
    if (qw_svg_abi_version() != QW_SVG_ABI_VERSION) return 6;
    if (!qw_svg_status_string(QW_SVG_OK)) return 7;
    if (qw_svgz_abi_version() != QW_SVGZ_ABI_VERSION) return 8;
    if (!qw_svgz_status_string(QW_SVGZ_OK)) return 9;
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
    if (qw_cad_abi_version() != QUIREWISE_CAD_ABI_VERSION) return 10;
    if (!qw_cad_status_string(QW_CAD_OK)) return 11;
    if (!qw_cad_probe(dxf, sizeof(dxf) - 1u)) return 12;
    {
        QwCadDocument *document = NULL;
        QwCadBounds bounds = {0};
        QwCadTextInfo text = {0};
        QwCadImage cad_image = {0};
        char *cad_svg = NULL;
        size_t cad_svg_size = 0u;
        if (qw_cad_parse(dxf, sizeof(dxf) - 1u, &document) != QW_CAD_OK)
            return 13;
        if (!document) return 14;
        if (qw_cad_format(document) != QW_CAD_FORMAT_DXF_ASCII) return 15;
        if (qw_cad_entity_count(document) != 2u) return 16;
        if (qw_cad_layer_count(document) == 0u) return 17;
        if (qw_cad_bounds(document, &bounds) != QW_CAD_OK) return 18;
        if (qw_cad_text_count(document) != 1u) return 19;
        if (qw_cad_text_at(document, 0u, &text) != QW_CAD_OK) return 20;
        if (text.text_size != 3u || memcmp(text.text, "CAD", 3u) != 0)
            return 21;
        if (qw_cad_render_rgba(document, 64u, &cad_image) != QW_CAD_OK)
            return 22;
        if (!cad_image.pixels || cad_image.width != 64u) return 23;
        qw_cad_image_free(&cad_image);
        if (qw_cad_render_svg(document, &cad_svg, &cad_svg_size) != QW_CAD_OK)
            return 24;
        if (!cad_svg || cad_svg_size == 0u) return 25;
        qw_cad_buffer_free(cad_svg);
        qw_cad_document_free(document);
    }
    return 0;
}
