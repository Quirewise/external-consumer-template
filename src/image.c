#include <quirewise/image.h>

#include "fixture_io.h"

#include <stdlib.h>
#include <string.h>

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
    {
        uint8_t *encoded = NULL;
        size_t encoded_size = 0u;

        if (qw_image_encode(&image, QW_IMAGE_ENCODE_PNG, &encoded,
                            &encoded_size) != QW_IMAGE_OK ||
            !encoded || encoded_size < 8u ||
            memcmp(encoded, "\x89PNG", 4u) != 0) {
            qw_image_buffer_free(encoded);
            qw_image_free(&image);
            return 8;
        }
        qw_image_buffer_free(encoded);
        encoded = NULL;
        encoded_size = 0u;
        if (qw_image_encode(&image, QW_IMAGE_ENCODE_TIFF, &encoded,
                            &encoded_size) != QW_IMAGE_OK ||
            !encoded || encoded_size < 4u ||
            memcmp(encoded, "II", 2u) != 0) {
            qw_image_buffer_free(encoded);
            qw_image_free(&image);
            return 9;
        }
        qw_image_buffer_free(encoded);
        encoded = NULL;
        encoded_size = 0u;
        if (qw_image_encode(&image, QW_IMAGE_ENCODE_JPEG, &encoded,
                            &encoded_size) != QW_IMAGE_OK ||
            !encoded || encoded_size < 2u || encoded[0] != 0xffu ||
            encoded[1] != 0xd8u) {
            qw_image_buffer_free(encoded);
            qw_image_free(&image);
            return 10;
        }
        qw_image_buffer_free(encoded);
        encoded = NULL;
        encoded_size = 0u;
        if (qw_image_encode(&image, QW_IMAGE_ENCODE_WEBP, &encoded,
                            &encoded_size) != QW_IMAGE_OK ||
            !encoded || encoded_size < 12u ||
            memcmp(encoded, "RIFF", 4u) != 0 ||
            memcmp(encoded + 8u, "WEBP", 4u) != 0) {
            qw_image_buffer_free(encoded);
            qw_image_free(&image);
            return 11;
        }
        qw_image_buffer_free(encoded);
    }
    qw_image_free(&image);
    if (!qw_image_status_string(0)) return 7;
    return status;
}
