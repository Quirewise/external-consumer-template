#include <quirewise/svg.h>

int main(void)
{
    static const char svg[] =
        "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"1\" height=\"1\">"
        "<rect width=\"1\" height=\"1\" fill=\"#336699\"/></svg>";
    QwSvgImage image = {0};
    QwSvgStatus status = qw_svg_decode(svg, sizeof(svg) - 1u, &image, NULL);
    if (status != QW_SVG_OK) return 1;
    qw_svg_image_free(&image);
    return 0;
}
