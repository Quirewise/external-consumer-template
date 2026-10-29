#include <quirewise/archive.h>

#include "fixture_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    static const char expected[] = "Quirewise external consumer fixture\n";
    uint8_t *data = NULL;
    size_t size = 0u;
    const char *error = NULL;
    QwZipArchiveReader *reader = NULL;
    QwArchiveEntry entry = {0};
    QwArchiveBuffer extracted = {0};
    QwArchiveStatus status;
    int result = 0;

    if (!qw_consumer_read_fixture("archive.zip", &data, &size)) return 2;
    status = qw_zip_archive_open(data, size, &reader, &error);
    if (status != QW_ARCHIVE_OK || !reader) {
        free(data);
        return 3;
    }
    if (qw_zip_archive_entry_count(reader) != 1u) {
        fputs("expected one ZIP fixture entry\n", stderr);
        result = 4;
    } else if (qw_zip_archive_get_entry(reader, 0u, &entry) != QW_ARCHIVE_OK) {
        fputs("could not read ZIP fixture entry metadata\n", stderr);
        result = 5;
    } else if (entry.compression_method != 8u) {
        fprintf(stderr, "expected DEFLATE method 8, got %u\n",
                (unsigned)entry.compression_method);
        result = 6;
    } else {
        status = qw_zip_archive_extract(reader, 0u, NULL, &extracted);
        if (status != QW_ARCHIVE_OK) {
            fprintf(stderr, "could not extract ZIP fixture entry: %s (%s)\n",
                    qw_archive_status_string(status),
                    qw_zip_archive_last_error(reader));
            result = 7;
        } else if (extracted.size != sizeof(expected) - 1u) {
            fprintf(stderr, "unexpected ZIP fixture size: %zu\n",
                    extracted.size);
            result = 8;
        } else if (memcmp(extracted.data, expected,
                          sizeof(expected) - 1u) != 0) {
            fputs("ZIP fixture contents do not match\n", stderr);
            result = 9;
        }
    }
    qw_archive_buffer_free(&extracted);
    qw_zip_archive_close(reader);
    free(data);
    return result;
}
