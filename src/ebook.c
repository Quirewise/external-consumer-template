#include <quirewise/epub.h>
#include <quirewise/fb2.h>
#include <quirewise/htmlz.h>
#include <quirewise/pml.h>
#include <quirewise/text_archive.h>

#include "fixture_io.h"

#include <stdlib.h>

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    size_t html_size = 0u;
    QwEpubDocument *epub = NULL;
    QwFB2Document *fb2 = NULL;
    QwHtmlzDocument htmlz = {0};
    QwTextArchiveDocument text_archive = {0};
    QwPmlDocument pml = {0};

    if (!qw_consumer_read_fixture("sample.epub", &data, &size)) return 2;
    if (qw_epub_parse(data, size, &epub) != 0 || !epub ||
        !qw_epub_section_count(epub)) {
        qw_epub_document_free(epub);
        free(data);
        return 3;
    }
    qw_epub_document_free(epub);
    free(data);

    if (!qw_consumer_read_fixture("sample.fb2", &data, &size)) return 4;
    if (qw_fb2_parse(data, size, &fb2) != 0 || !fb2 ||
        !qw_fb2_document_html(fb2, &html_size) || html_size == 0u) {
        qw_fb2_document_free(fb2);
        free(data);
        return 5;
    }
    qw_fb2_document_free(fb2);
    free(data);

    if (!qw_consumer_read_fixture("sample.htmlz", &data, &size)) return 6;
    if (qw_htmlz_abi_version() == 0u) { free(data); return 7; }
    if (qw_htmlz_extract_primary_document(data, size, &htmlz, NULL) != 0 ||
        htmlz.html_size == 0u) {
        qw_htmlz_document_free(&htmlz);
        free(data);
        return 8;
    }
    if (!qw_htmlz_status_string(0)) {
        qw_htmlz_document_free(&htmlz);
        free(data);
        return 9;
    }
    qw_htmlz_document_free(&htmlz);
    free(data);

    if (!qw_consumer_read_fixture("sample.txtz", &data, &size)) return 10;
    if (qw_text_archive_abi_version() == 0u) { free(data); return 11; }
    if (qw_text_archive_extract_primary(data, size, QW_TEXT_ARCHIVE_TXTZ,
                                        &text_archive, NULL) != 0 ||
        text_archive.text_size == 0u) {
        qw_text_archive_document_free(&text_archive);
        free(data);
        return 12;
    }
    if (!qw_text_archive_status_string(0)) {
        qw_text_archive_document_free(&text_archive);
        free(data);
        return 13;
    }
    qw_text_archive_document_free(&text_archive);
    free(data);

    if (!qw_consumer_read_fixture("sample.pml", &data, &size)) return 14;
    if (qw_pml_abi_version() == 0u) { free(data); return 15; }
    if (qw_pml_parse(data, size, &pml) != 0 || pml.text_size == 0u) {
        qw_pml_document_free(&pml);
        free(data);
        return 16;
    }
    if (!qw_pml_status_string(0)) {
        qw_pml_document_free(&pml);
        free(data);
        return 17;
    }
    qw_pml_document_free(&pml);
    free(data);
    return 0;
}
