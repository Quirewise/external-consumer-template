#include <quirewise/page_status.h>
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
    int status;

    {
        static const uint8_t junk[16] = {0x00u, 0x01u, 0x02u, 0x03u};
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

        {
            uint8_t *djvu_data = NULL;
            size_t djvu_size = 0u;
            QwDjvuPageInfo djvu_info;
            QwDjvuImage djvu_image;
            uint8_t *djvu_text_data = NULL;
            size_t djvu_text_size = 0u;
            char *djvu_text = NULL;
            size_t djvu_text_out = 0u;
            if (!qw_consumer_read_fixture("tiny.djvu", &djvu_data, &djvu_size))
                return 40;
            if (!qw_djvu_is_supported(djvu_data, djvu_size)) return 41;
            if (qw_djvu_get_page_count(djvu_data, djvu_size, &djvu_pages) !=
                QW_DJVU_OK || djvu_pages != 1u) return 42;
            memset(&djvu_info, 0, sizeof(djvu_info));
            if (qw_djvu_get_page_info(djvu_data, djvu_size, 0u, &djvu_info) !=
                QW_DJVU_OK || djvu_info.width != 64u ||
                djvu_info.height != 48u || djvu_info.dpi != 300u) return 43;
            memset(&djvu_image, 0, sizeof(djvu_image));
            if (qw_djvu_render_page(djvu_data, djvu_size, 0u, 1u, &djvu_image) !=
                QW_DJVU_OK || djvu_image.format != QW_DJVU_PIXEL_GRAY8 ||
                djvu_image.size != 64u * 48u) return 44;
            qw_djvu_image_free(&djvu_image);
            free(djvu_data);
            if (!qw_consumer_read_fixture("tiny-text.djvu", &djvu_text_data,
                                          &djvu_text_size)) return 45;
            if (qw_djvu_extract_text(djvu_text_data, djvu_text_size, 0u,
                                     &djvu_text, &djvu_text_out) != QW_DJVU_OK ||
                !djvu_text || !strstr(djvu_text, "Hello DjVu")) return 46;
            qw_djvu_text_free(djvu_text);
            free(djvu_text_data);
        }

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

    }

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
