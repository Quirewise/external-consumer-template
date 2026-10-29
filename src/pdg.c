#include <pdg.h>

#include "fixture_io.h"

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    PdgBuffer buffer = {0};
    int status;

    if (!qw_consumer_read_fixture("pixel.bmp", &data, &size)) return 2;
    status = pdg_decode_memory(data, size, &buffer);
    free(data);
    if (status != PDG_OK || !buffer.data) return 3;
    pdg_buffer_free(&buffer);
    return 0;
}
