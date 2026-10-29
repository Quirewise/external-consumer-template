#include <quirewise/page_status.h>
#include <quirewise/pdf_parser.h>
#include <quirewise/ceb.h>

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

    if (!qw_consumer_read_fixture("one-page.pdf", &data, &size)) return 2;
    status = qw_pdf_parser_open(data, size, &parser);
    free(data);
    if (status != QW_PAGE_OK || !parser) return 3;
    qw_pdf_parser_close(parser);

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
