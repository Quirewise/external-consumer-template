#include <quirewise/page_status.h>
#include <quirewise/pdf_parser.h>
#include <quirewise/ceb.h>
#include <quirewise/djvu.h>
#include <quirewise/uvz_parser.h>
#include <quirewise/caj.h>

#include "fixture_io.h"

#include <stdio.h>
#include <string.h>

static int contains_bytes(const uint8_t *data, size_t size,
                          const uint8_t *needle, size_t needle_size)
{
    size_t offset;
    if (!data || !needle || needle_size > size) return 0;
    for (offset = 0; offset <= size - needle_size; ++offset) {
        if (memcmp(data + offset, needle, needle_size) == 0) return 1;
    }
    return 0;
}

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    uint8_t *ceb_data = NULL;
    size_t ceb_size = 0u;
    uint8_t *decrypted_pdf = NULL;
    size_t decrypted_size = 0u;
    uint8_t *bad_rsa_data = NULL;
    size_t bad_rsa_size = 0u;
    uint8_t *bad_rsa_pdf = NULL;
    size_t bad_rsa_pdf_size = 0u;
    QwPdfParser *parser = NULL;
    int status;

    {
        static const uint8_t junk[16] = {0x00u, 0x01u, 0x02u, 0x03u};
        QwPdfParserInfo pdf_info = {0};
        QwPdfParserPageInfo page_info = {0};
        QwDjvuStatus djvu_status;
        QwUvzDocument *uvz = NULL;
        QwUvzPageInfo uvz_info = {0};
        uint8_t *uvz_page = NULL;
        size_t uvz_page_size = 0u;
        QwCajDocument *caj = NULL;
        QwCajPageInfo caj_info = {0};
        QwCajFigureInfo figure_info = {0};
        QwCajClassicOutlineList *outlines = NULL;
        QwCajClassicOutlineInfo outline_info = {0};
        const uint8_t *text = NULL;
        size_t text_size = 0u;
        uint8_t *pdf = NULL;
        size_t pdf_size = 0u;
        size_t djvu_pages = 0u;

        if (qw_djvu_abi_version() == 0u) return 20;
        if (!qw_djvu_status_string(0)) return 21;
        if (qw_djvu_is_supported(junk, sizeof(junk)) != 0) return 22;
        djvu_status = qw_djvu_get_page_count(junk, sizeof(junk), &djvu_pages);
        if (djvu_status == QW_DJVU_OK) return 23;

        if (qw_uvz_document_open(junk, sizeof(junk), &uvz) == QW_UVZ_OK) return 24;
        if (qw_uvz_document_page_count(NULL) != 0u) return 25;
        if (qw_uvz_document_page_info(NULL, 0u, &uvz_info) == QW_UVZ_OK) return 26;
        if (qw_uvz_document_read_page(NULL, 0u, &uvz_page, &uvz_page_size) ==
            QW_UVZ_OK) return 27;
        qw_uvz_page_data_free(NULL);
        qw_uvz_document_close(NULL);

        if (qw_caj_document_open(junk, sizeof(junk), &caj) == QW_CAJ_OK) return 28;
        if (qw_caj_document_page_count(NULL) != 0u) return 29;
        if (qw_caj_document_page_info(NULL, 0u, &caj_info) == QW_CAJ_OK) return 30;
        if (qw_caj_document_figure_info(NULL, 0u, 0u, &figure_info) ==
            QW_CAJ_OK) return 31;
        if (qw_caj_document_page_text(NULL, 0u, &text, &text_size) ==
            QW_CAJ_OK) return 32;
        if (qw_caj_classic_outlines_parse(junk, sizeof(junk), &outlines) ==
            QW_CAJ_OK) return 33;
        if (qw_caj_classic_outline_count(NULL) != 0u) return 34;
        if (qw_caj_classic_outline_get(NULL, 0u, &outline_info) == QW_CAJ_OK)
            return 35;
        qw_caj_classic_outlines_free(NULL);
        if (qw_caj_extract_classic_pdf(junk, sizeof(junk), &pdf, &pdf_size) ==
            QW_CAJ_OK) return 36;
        qw_caj_pdf_data_free(pdf);
        pdf = NULL;
        if (qw_caj_decrypt_kdh_pdf(junk, sizeof(junk), &pdf, &pdf_size) ==
            QW_CAJ_OK) return 37;
        qw_caj_pdf_data_free(pdf);
        qw_caj_pdf_data_free(NULL);
        qw_caj_document_close(NULL);

        (void)pdf_info;
        (void)page_info;
    }

    if (!qw_consumer_read_fixture("one-page.pdf", &data, &size)) return 2;
    status = qw_pdf_parser_open(data, size, &parser);
    if (status != QW_PAGE_OK || !parser) { free(data); return 3; }
    {
        QwPdfParserInfo info = {0};
        QwPdfParserPageInfo page = {0};
        if (qw_pdf_parser_get_info(parser, &info) != QW_PAGE_OK ||
            info.page_count != 1u) {
            qw_pdf_parser_close(parser);
            free(data);
            return 19;
        }
        if (qw_pdf_parser_get_page_info(parser, 0u, &page) != QW_PAGE_OK ||
            page.media_box.x1 <= page.media_box.x0 || page.media_box.y1 <= page.media_box.y0) {
            qw_pdf_parser_close(parser);
            free(data);
            return 18;
        }
    }
    qw_pdf_parser_close(parser);
    free(data);

    if (!qw_consumer_read_fixture("synthetic-ceb.ceb", &ceb_data, &ceb_size))
        return 4;
    {
        QwCebStatus ceb_status = qw_ceb_decrypt_to_pdf(
            ceb_data, ceb_size, &decrypted_pdf, &decrypted_size);
        if (ceb_status != QW_CEB_OK || !decrypted_pdf ||
            decrypted_size == 0u) {
            fprintf(stderr, "CEB fixture decryption failed: status %d\n",
                    (int)ceb_status);
            free(ceb_data);
            qw_ceb_pdf_free(decrypted_pdf);
            return 5;
        }
    }
    if (!contains_bytes(decrypted_pdf, decrypted_size,
                        (const uint8_t *)"CEBtest!", 8u) ||
        contains_bytes(decrypted_pdf, decrypted_size,
                       (const uint8_t *)"/Encrypt", 8u)) {
        free(ceb_data);
        qw_ceb_pdf_free(decrypted_pdf);
        return 6;
    }
    qw_ceb_pdf_free(decrypted_pdf);
    decrypted_pdf = NULL;
    decrypted_size = 0u;
    {
        QwCebStatus truncated_status = qw_ceb_decrypt_to_pdf(
            ceb_data, ceb_size - 1u, &decrypted_pdf, &decrypted_size);
        if (truncated_status == QW_CEB_OK ||
            decrypted_pdf || decrypted_size != 0u) {
            free(ceb_data);
            qw_ceb_pdf_free(decrypted_pdf);
            return 7;
        }
    }
    free(ceb_data);

    if (!qw_consumer_read_fixture("synthetic-ceb-invalid-rsa.ceb",
                                  &bad_rsa_data, &bad_rsa_size)) return 8;
    {
        QwCebStatus invalid_rsa_status = qw_ceb_decrypt_to_pdf(
            bad_rsa_data, bad_rsa_size, &bad_rsa_pdf, &bad_rsa_pdf_size);
        if (invalid_rsa_status != QW_CEB_ERR_DECODE ||
            bad_rsa_pdf || bad_rsa_pdf_size != 0u) {
            free(bad_rsa_data);
            qw_ceb_pdf_free(bad_rsa_pdf);
            return 9;
        }
    }
    free(bad_rsa_data);
    return 0;
}
