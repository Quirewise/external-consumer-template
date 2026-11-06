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
    if (status != QW_COMIC_OK || !archive) { free(data); return 3; }
    status = qw_comic_archive_page_count(archive) ? QW_COMIC_OK : QW_COMIC_ERR_FORMAT;
    if (status == QW_COMIC_OK) {
        if (qw_comic_archive_format(archive) != QW_COMIC_ARCHIVE_ZIP)
            status = QW_COMIC_ERR_FORMAT;
    }
    if (status == QW_COMIC_OK) {
        QwComicPageInfo info;
        if (qw_comic_archive_page_info(archive, 0u, &info) != QW_COMIC_OK)
            status = QW_COMIC_ERR_FORMAT;
    }
    if (status == QW_COMIC_OK) {
        uint8_t *page = NULL;
        size_t page_size = 0u;
        if (qw_comic_archive_read_page(archive, 0u, &page, &page_size) !=
            QW_COMIC_OK || !page || !page_size)
            status = QW_COMIC_ERR_FORMAT;
        qw_comic_page_data_free(page);
    }
    qw_comic_archive_close(archive);
    free(data); /* the archive borrows the container until close */
    return status == QW_COMIC_OK ? 0 : 4;
}
