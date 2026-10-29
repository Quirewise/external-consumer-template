#include <quirewise/epub.h>

#include "fixture_io.h"

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    QwEpubDocument *document = NULL;
    QwEpubStatus status;

    if (!qw_consumer_read_fixture("sample.epub", &data, &size)) return 2;
    status = qw_epub_parse(data, size, &document);
    free(data);
    if (status != QW_EPUB_OK || !document) return 3;
    status = qw_epub_section_count(document) ? QW_EPUB_OK : QW_EPUB_ERR_FORMAT;
    qw_epub_document_free(document);
    return status == QW_EPUB_OK ? 0 : 4;
}
