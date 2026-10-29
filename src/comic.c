#include <quirewise/comic.h>

#include "fixture_io.h"

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    QwComicArchive *archive = NULL;
    QwComicStatus status;

    if (!qw_consumer_read_fixture("sample.cbz", &data, &size)) return 2;
    status = qw_comic_archive_open(data, size, QW_COMIC_ARCHIVE_ZIP,
                                   NULL, &archive);
    free(data);
    if (status != QW_COMIC_OK || !archive) return 3;
    status = qw_comic_archive_page_count(archive) ? QW_COMIC_OK : QW_COMIC_ERR_FORMAT;
    qw_comic_archive_close(archive);
    return status == QW_COMIC_OK ? 0 : 4;
}
