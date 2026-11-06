#include <quirewise/image.h>

#include "fixture_io.h"

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    QwImage image = {0};
    int status;

    if (!qw_consumer_read_fixture("pixel.bmp", &data, &size)) return 2;
    if (qw_image_abi_version() == 0u) { free(data); return 5; }
    if (!qw_image_probe(data, size)) { free(data); return 6; }
    status = qw_image_decode(data, size, &image);
    free(data);
    if (status != 0) return 3;
    status = image.width == 1u && image.height == 1u ? 0 : 4;
    qw_image_free(&image);
    if (!qw_image_status_string(0)) return 7;
    return status;
}
