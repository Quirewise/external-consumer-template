#include <quirewise/convert.h>
#include <quirewise/docx.h>
#include <quirewise/email.h>
#include <quirewise/odt.h>
#include <quirewise/hwpx.h>
#include <quirewise/odf_presentation.h>
#include <quirewise/pdf.h>
#include <quirewise/pptx.h>
#include <quirewise/xps.h>

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
    {
        QwPdfDocument *document = NULL;
        size_t pdf_pages = 0u;
        QwPageImage page;
        uint8_t *pdf_text = NULL;
        size_t pdf_text_size = 0u;
        uint8_t *merged = NULL;
        size_t merged_size = 0u;
        size_t merged_pages = 0u;
        uint8_t *png = NULL;
        size_t png_size = 0u;
        const uint8_t *inputs[1];
        size_t input_sizes[1];
        int status;

        memset(&page, 0, sizeof(page));
        status = qw_pdf_open(pdf, pdf_size, &document, &pdf_pages, NULL);
        if (status != QW_STATUS_OK || !document || pdf_pages != 1u) {
            qw_pdf_close(document);
            free(pdf);
            return 28;
        }
        {
            double media[4];
            double crop[4];
            int rotation = -1;
            if (qw_pdf_page_geometry(document, 0u, media, crop, &rotation) !=
                    QW_STATUS_OK ||
                media[2] <= media[0] || media[3] <= media[1] ||
                rotation != 0) {
                qw_pdf_close(document);
                free(pdf);
                return 61;
            }
        }
        {
            QwPdfDocument *structure = NULL;
            size_t structure_pages = 0u;
            if (qw_pdf_open_structure(pdf, pdf_size, &structure,
                                      &structure_pages, NULL) !=
                    QW_STATUS_OK ||
                !structure || structure_pages != 1u) {
                qw_pdf_close(structure);
                qw_pdf_close(document);
                free(pdf);
                return 62;
            }
            qw_pdf_close(structure);
        }
        status = qw_pdf_render_page(document, 0u, 72u, &page, NULL);
        if (status != QW_STATUS_OK || !page.data || page.width == 0u) {
            qw_pdf_page_free(&page);
            qw_pdf_close(document);
            free(pdf);
            return 29;
        }
        qw_pdf_page_free(&page);

        status = qw_pdf_extract_text(document, &pdf_text, &pdf_text_size, NULL);
        if (status != QW_STATUS_OK || !pdf_text || pdf_text_size == 0u) {
            qw_pdf_free(pdf_text);
            qw_pdf_close(document);
            free(pdf);
            return 30;
        }
        qw_pdf_free(pdf_text);

        inputs[0] = pdf;
        input_sizes[0] = pdf_size;
        status = qw_pdf_merge(inputs, input_sizes, 1u, &merged, &merged_size,
                              &merged_pages, NULL);
        if (status != QW_STATUS_OK || !merged || merged_pages != 1u ||
            merged_size < 8u || memcmp(merged, "%PDF-", 5u) != 0) {
            qw_pdf_free(merged);
            qw_pdf_close(document);
            free(pdf);
            return 31;
        }
        qw_pdf_free(merged);

        status = qw_convert_to_format_memory(pdf, pdf_size, QW_FORMAT_PNG, NULL,
                                             &png, &png_size, NULL);
        if (status != QW_STATUS_OK || !png || png_size < 8u ||
            png[0] != 0x89u || memcmp(png + 1, "PNG", 3u) != 0) {
            qw_buffer_free(png);
            qw_pdf_close(document);
            free(pdf);
            return 32;
        }
        qw_buffer_free(png);

        status = qw_convert_to_format_memory(pdf, pdf_size, QW_FORMAT_TXT, NULL,
                                             &pdf_text, &pdf_text_size, NULL);
        if (status != QW_STATUS_OK || !pdf_text) {
            qw_buffer_free(pdf_text);
            qw_pdf_close(document);
            free(pdf);
            return 33;
        }
        qw_buffer_free(pdf_text);
        qw_pdf_close(document);
    }
    free(pdf);
    {
        static const uint8_t junk[16] = {0x00u, 0x01u, 0x02u, 0x03u};
        QwOdpDocument *odp = NULL;
        QwPptxDocument *pptx = NULL;
        QwHwpxDocument *hwpx = NULL;
        QwXpsDocument *xps = NULL;
        QwXpsPageInfo page_info = {0};
        uint8_t *page_xml = NULL;
        size_t page_xml_size = 0u;
        if (qw_odp_parse(QW_ODP_FORMAT_ODP, junk, sizeof(junk), &odp) ==
            QW_ODP_OK) return 16;
        if (qw_odp_document_html(NULL, &html_size) != NULL) return 17;
        if (qw_odp_document_slide_count(NULL) != 0u) return 18;
        qw_odp_document_free(NULL);
        if (qw_pptx_parse(QW_PPTX_FORMAT_PPTX, junk, sizeof(junk), &pptx) ==
            QW_PPTX_OK) return 19;
        if (qw_pptx_document_html(NULL, &html_size) != NULL) return 20;
        qw_pptx_document_free(NULL);
        if (qw_hwpx_parse(junk, sizeof(junk), &hwpx) == QW_HWPX_OK) return 21;
        if (qw_hwpx_document_html(NULL, &html_size) != NULL) return 22;
        if (qw_hwpx_document_section_count(NULL) != 0u) return 23;
        if (qw_hwpx_document_omitted_object_count(NULL) != 0u) return 24;
        qw_hwpx_document_free(NULL);
        if (qw_xps_document_open(junk, sizeof(junk), &xps) == QW_XPS_OK)
            return 25;
        if (qw_xps_document_page_count(NULL) != 0u) return 26;
        if (qw_xps_document_read_page(NULL, 0u, &page_xml, &page_xml_size,
                                      &page_info) == QW_XPS_OK) return 27;
        qw_xps_data_free(NULL);
        qw_xps_document_close(NULL);
    }
    {
        /* Structure-only view of a fixture PDF: this is the replacement for
         * the page package's former qw_pdf_parser_* API. */
        uint8_t *fixture = NULL;
        size_t fixture_size = 0u;
        QwPdfDocument *structure = NULL;
        size_t structure_pages = 0u;
        double media[4];
        double crop[4];
        int rotation = -1;
        if (!qw_consumer_read_fixture("one-page.pdf", &fixture,
                                      &fixture_size)) {
            return 34;
        }
        if (qw_pdf_open_structure(fixture, fixture_size, &structure,
                                  &structure_pages, NULL) != QW_STATUS_OK ||
            !structure || structure_pages != 1u) {
            qw_pdf_close(structure);
            free(fixture);
            return 35;
        }
        if (qw_pdf_page_geometry(structure, 0u, media, crop, &rotation) !=
                QW_STATUS_OK ||
            media[2] <= media[0] || media[3] <= media[1]) {
            qw_pdf_close(structure);
            free(fixture);
            return 36;
        }
        qw_pdf_close(structure);
        free(fixture);
    }

    return 0;
}
