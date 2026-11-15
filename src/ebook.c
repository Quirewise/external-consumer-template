#include <quirewise/epub.h>
#include <quirewise/fb2.h>
#include <quirewise/mdd.h>
#include <quirewise/mdx.h>
#include <quirewise/mobi.h>
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
    {
        QwEpubSectionView section = {0};
        if (qw_epub_section_at(epub, 0u, &section) != QW_EPUB_OK) {
            qw_epub_document_free(epub);
            free(data);
            return 18;
        }
        if (section.paragraph_count != 0u) {
            QwEpubParagraphView paragraph = {0};
            if (qw_epub_paragraph_at(epub, 0u, 0u, &paragraph) != QW_EPUB_OK) {
                qw_epub_document_free(epub);
                free(data);
                return 19;
            }
            if (paragraph.run_count != 0u) {
                QwEpubRunView run = {0};
                if (qw_epub_run_at(epub, 0u, 0u, 0u, &run) != QW_EPUB_OK) {
                    qw_epub_document_free(epub);
                    free(data);
                    return 20;
                }
            }
        }
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
    {
        static const uint8_t junk[16] = {0x00u, 0x01u, 0x02u, 0x03u};
        QwMobiDocument *mobi = NULL;
        size_t text_size = 0u;
        if (qw_mobi_probe(junk, sizeof(junk)) != 0) return 21;
        if (qw_mobi_parse(junk, sizeof(junk), &mobi) == QW_MOBI_OK) return 22;
        if (qw_mobi_document_text(NULL, &text_size) != NULL) return 23;
        qw_mobi_document_kind(NULL);
        qw_mobi_document_free(NULL);
    }
    {
        static const uint8_t junk[16] = {0x00u, 0x00u, 0x00u, 0x10u};
        QwMdxDocument *mdx = NULL;
        QwMddDocument *mdd = NULL;
        const uint8_t *bytes = NULL;
        size_t size = 0u;
        char error[64];
        if (qw_mdx_probe(junk, sizeof(junk)) != 0) return 24;
        if (qw_mdd_probe(junk, sizeof(junk)) != 0) return 25;
        if (qw_mdx_parse(junk, sizeof(junk), &mdx, error, sizeof(error)) ==
            QW_MDX_OK) return 26;
        if (qw_mdd_parse(junk, sizeof(junk), &mdd, error, sizeof(error)) ==
            QW_MDD_OK) return 27;
        if (qw_mdx_document_title(NULL) != NULL) return 28;
        if (qw_mdx_document_description(NULL) != NULL) return 29;
        if (qw_mdx_document_encoding(NULL) != NULL) return 30;
        if (qw_mdx_document_entry_count(NULL) != 0u) return 31;
        if (qw_mdx_document_entry_at(NULL, 0u, NULL, NULL, NULL, NULL) !=
            QW_MDX_ERROR_INVALID_ARGUMENT) return 32;
        if (qw_mdx_document_html(NULL, &bytes, &size) !=
            QW_MDX_ERROR_INVALID_ARGUMENT) return 33;
        if (qw_mdx_document_text(NULL, &bytes, &size) !=
            QW_MDX_ERROR_INVALID_ARGUMENT) return 34;
        if (qw_mdx_document_html_with_resources(NULL, NULL, &bytes, &size) !=
            QW_MDX_ERROR_INVALID_ARGUMENT) return 41;
        if (qw_mdd_document_title(NULL) != NULL) return 35;
        if (qw_mdd_document_resource_count(NULL) != 0u) return 36;
        if (qw_mdd_document_resource_at(NULL, 0u, NULL, NULL, NULL, NULL) !=
            QW_MDD_ERROR_INVALID_ARGUMENT) return 37;
        if (qw_mdd_document_resource_find(NULL, "x", 1u, NULL, NULL) !=
            QW_MDD_ERROR_INVALID_ARGUMENT) return 38;
        qw_mdx_document_free(NULL);
        qw_mdd_document_free(NULL);
    }
    {
        static const char text[] = "External consumer TXTZ\n";
        static const char html[] =
            "<html><body><p>External consumer HTMLZ</p></body></html>";
        uint8_t *archive = NULL;
        size_t archive_size = 0u;

        if (qw_text_archive_create(QW_TEXT_ARCHIVE_TXTZ, text,
                                   sizeof(text) - 1u, &archive,
                                   &archive_size) != QW_TEXT_ARCHIVE_OK ||
            !archive || !archive_size)
            return 39;
        qw_text_archive_buffer_free(archive);
        archive = NULL;
        archive_size = 0u;
        if (qw_htmlz_create(html, sizeof(html) - 1u, &archive, &archive_size) !=
                QW_HTMLZ_OK ||
            !archive || !archive_size)
            return 40;
        qw_htmlz_buffer_free(archive);
    }
    return 0;
}
