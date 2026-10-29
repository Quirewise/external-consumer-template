#ifndef QUIREWISE_EXTERNAL_CONSUMER_FIXTURE_IO_H
#define QUIREWISE_EXTERNAL_CONSUMER_FIXTURE_IO_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef QUIREWISE_CONSUMER_FIXTURE_DIR
#define QUIREWISE_CONSUMER_FIXTURE_DIR "fixtures"
#endif

static int qw_consumer_read_fixture(const char *name,
                                    uint8_t **out_data,
                                    size_t *out_size)
{
    char path[2048];
    FILE *file;
    long length;
    uint8_t *data;
    int written;

    if (!name || !out_data || !out_size) return 0;
    *out_data = NULL;
    *out_size = 0u;
    written = snprintf(path, sizeof(path), "%s/%s",
                       QUIREWISE_CONSUMER_FIXTURE_DIR, name);
    if (written < 0 || (size_t)written >= sizeof(path)) return 0;
    file = fopen(path, "rb");
    if (!file) return 0;
    if (fseek(file, 0, SEEK_END) != 0 || (length = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return 0;
    }
    data = (uint8_t *)malloc(length ? (size_t)length : 1u);
    if (!data || fread(data, 1u, (size_t)length, file) != (size_t)length) {
        free(data);
        fclose(file);
        return 0;
    }
    fclose(file);
    *out_data = data;
    *out_size = (size_t)length;
    return 1;
}

#endif
