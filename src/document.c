#include <quirewise/convert.h>
#include <quirewise/docx.h>
#include <quirewise/email.h>
#include <quirewise/odt.h>

#include "fixture_io.h"

#include <stdlib.h>
#include <string.h>

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    size_t html_size = 0u;
    QwOdtDocument *odt = NULL;
    QwDocxDocument *docx = NULL;
    QwDocxPageLayout layout;
    QwEmailDocument *email = NULL;
    uint8_t *pdf = NULL;
    size_t pdf_size = 0u;
    static const char html[] =
        "<html><body><h1>Consumer</h1><p>convert entry</p></body></html>";

    if (!qw_consumer_read_fixture("sample.odt", &data, &size)) return 2;
    if (qw_odt_parse(QW_ODT_FORMAT_ODT, data, size, &odt) != 0 || !odt) {
        free(data);
        return 3;
    }
    if (!qw_odt_document_html(odt, &html_size) || html_size == 0u) {
        qw_odt_document_free(odt);
        free(data);
        return 4;
    }
    qw_odt_document_free(odt);
    free(data);

    if (!qw_consumer_read_fixture("sample.docx", &data, &size)) return 5;
    if (!qw_docx_probe(data, size)) {
        free(data);
        return 6;
    }
    if (qw_docx_parse(data, size, &docx) != 0 || !docx) {
        free(data);
        return 7;
    }
    if (!qw_docx_document_html(docx, &html_size) || html_size == 0u) {
        qw_docx_document_free(docx);
        free(data);
        return 8;
    }
    memset(&layout, 0, sizeof(layout));
    if (qw_docx_document_get_page_layout(docx, &layout) != 0 ||
        layout.width_twips == 0u) {
        qw_docx_document_free(docx);
        free(data);
        return 9;
    }
    qw_docx_document_free(docx);
    free(data);

    if (!qw_consumer_read_fixture("sample.eml", &data, &size)) return 10;
    if (!qw_email_probe(data, size)) {
        free(data);
        return 11;
    }
    if (qw_email_parse(data, size, &email) != 0 || !email) {
        free(data);
        return 12;
    }
    if (!qw_email_document_html(email, &html_size) || html_size == 0u) {
        qw_email_document_free(email);
        free(data);
        return 13;
    }
    if (qw_email_document_attachment_count(email) > 0u) {
        QwEmailAttachment attachment;
        if (qw_email_document_get_attachment(email, 0u, &attachment) != 0) {
            qw_email_document_free(email);
            free(data);
            return 14;
        }
    }
    qw_email_document_free(email);
    free(data);

    if (qw_convert_to_pdf_memory(html, sizeof(html) - 1u, NULL, &pdf, &pdf_size,
                                 NULL) != 0 || !pdf || pdf_size < 8u ||
        memcmp(pdf, "%PDF-", 5u) != 0) {
        free(pdf);
        return 15;
    }
    free(pdf);
    return 0;
}
