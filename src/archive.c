#include <quirewise/archive.h>

#include "fixture_io.h"

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    const char *error = NULL;
    QwZipArchiveReader *reader = NULL;
    QwArchiveStatus status;

    if (!qw_consumer_read_fixture("archive.zip", &data, &size)) return 2;
    status = qw_zip_archive_open(data, size, &reader, &error);
    free(data);
    if (status != QW_ARCHIVE_OK || !reader) return 3;
    status = qw_zip_archive_entry_count(reader) ? QW_ARCHIVE_OK : QW_ARCHIVE_ERR_FORMAT;
    qw_zip_archive_close(reader);
    return status == QW_ARCHIVE_OK ? 0 : 4;
}
