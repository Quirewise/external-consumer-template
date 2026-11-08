#include <pdg.h>
#include <pdz.h>
#include <pdz_pdf.h>

#include "fixture_io.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

/* Caller-owned sink for the streaming PDF writer. */
typedef struct ConsumerPdfSink {
    uint8_t *data;
    size_t size;
    size_t capacity;
} ConsumerPdfSink;

static int consumer_pdf_sink(void *context, const void *data, size_t size)
{
    ConsumerPdfSink *sink = (ConsumerPdfSink *)context;
    uint8_t *grown;
    if (!sink || !data) return -1;
    if (sink->size + size > sink->capacity) {
        size_t capacity = sink->capacity ? sink->capacity : 4096u;
        while (capacity < sink->size + size) capacity *= 2u;
        grown = (uint8_t *)realloc(sink->data, capacity);
        if (!grown) return -1;
        sink->data = grown;
        sink->capacity = capacity;
    }
    memcpy(sink->data + sink->size, data, size);
    sink->size += size;
    return 0;
}

int main(void)
{
    static const uint8_t junk[16] = {0x00u, 0x01u, 0x02u, 0x03u};
    PdgContext *context;
    PdgBuffer buffer = {0};
    PdgImage image = {0};
    PdgBatchItem batch[2];
    PdgRenderItem render_batch[2];
    int rc;

    if (pdg_abi_version() != PDG_ABI_VERSION) return 1;
    if (!check_text(pdg_error_string(PDG_OK)) ||
        !check_text(pdg_error_string(PDG_ERR_NOT_PDG))) return 2;
    if (!check_text(pdg_output_extension(PDG_OUTPUT_BMP)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_JPEG)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_PNG)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_RAW)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_DJVU)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_TIFF)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_PDF)) ||
        !check_text(pdg_output_extension(PDG_OUTPUT_JP2))) return 3;
    if (!check_text(pdg_output_kind_name(PDG_OUTPUT_RAW)) ||
        !check_text(pdg_output_kind_name(PDG_OUTPUT_PDF))) return 4;
    (void)pdg_output_requires_external_decoder(PDG_OUTPUT_DJVU);
    (void)pdg_output_requires_external_decoder(PDG_OUTPUT_BMP);

    context = pdg_context_create();
    if (!context) return 5;
    rc = pdg_decode_memory_ctx(context, junk, sizeof(junk), &buffer);
    if (rc != PDG_ERR_NOT_PDG) return 6;
    pdg_buffer_free(&buffer);
    rc = pdg_decrypt_memory_ctx(context, junk, sizeof(junk), &buffer);
    if (rc != PDG_ERR_NOT_PDG) return 7;
    pdg_buffer_free(&buffer);
    pdg_context_free(context);
    pdg_context_free(NULL);

    rc = pdg_decode_memory(junk, sizeof(junk), &buffer);
    if (rc != PDG_ERR_NOT_PDG) return 8;
    pdg_buffer_free(&buffer);
    rc = pdg_decrypt_memory(junk, sizeof(junk), &buffer);
    if (rc != PDG_ERR_NOT_PDG) return 9;
    pdg_buffer_free(&buffer);

    memset(batch, 0, sizeof(batch));
    batch[0].data = junk;
    batch[0].size = sizeof(junk);
    batch[1].data = junk;
    batch[1].size = sizeof(junk);
    (void)pdg_decode_memory_batch(batch, 2u, 1u);
    if (batch[0].status == PDG_OK || batch[1].status == PDG_OK) return 10;
    memset(batch, 0, sizeof(batch));
    batch[0].data = junk;
    batch[0].size = sizeof(junk);
    (void)pdg_decrypt_memory_batch(batch, 1u, 1u);
    if (batch[0].status == PDG_OK) return 11;

    rc = pdg_render_memory(junk, sizeof(junk), &image);
    if (rc == PDG_OK) return 12;
    pdg_image_free(&image);
    context = pdg_context_create();
    if (!context) return 13;
    rc = pdg_render_memory_ctx(context, junk, sizeof(junk), &image);
    if (rc == PDG_OK) return 14;
    pdg_image_free(&image);
    pdg_context_free(context);
    memset(render_batch, 0, sizeof(render_batch));
    render_batch[0].data = junk;
    render_batch[0].size = sizeof(junk);
    (void)pdg_render_memory_batch(render_batch, 1u, 1u);
    if (render_batch[0].status == PDG_OK) return 15;

    rc = pdg_decode_file("does-not-exist.pdg", &buffer);
    if (rc != PDG_ERR_IO) return 16;
    pdg_buffer_free(&buffer);
    rc = pdg_decrypt_file("does-not-exist.pdg", &buffer);
    if (rc != PDG_ERR_IO) return 17;
    pdg_buffer_free(&buffer);
    pdg_buffer_free(NULL);
    pdg_image_free(NULL);

    if (pdz_abi_version() != PDZ_ABI_VERSION) return 18;
    if (pdz_is_container(junk, sizeof(junk)) != 0) return 19;
    if (!check_text(pdz_cipher_name(4u)) || !check_text(pdz_cipher_name(0u)))
        return 20;
    if (!check_text(pdz_member_scope_name(PDZ_MEMBER_SCOPE_PREFIX))) return 21;
    {
        PdzInfo info = {0};
        if (pdz_parse(junk, sizeof(junk), &info) == PDG_OK) return 22;
    }
    {
        size_t count = 99u;
        if (pdz_members(junk, sizeof(junk), NULL, 0u, &count) == PDG_OK)
            return 23;
    }
    {
        PdzValidation validation = {0};
        if (pdz_validate(junk, sizeof(junk), NULL, 0u, &validation) == PDG_OK)
            return 23;
    }
    {
        PdzMember member;
        PdzBuffer payload = {0};
        memset(&member, 0, sizeof(member));
        member.offset = 0x1000u;
        member.size = 16u;
        if (pdz_member_payload(junk, sizeof(junk), &member, NULL, &payload) ==
            PDG_OK) return 24;
        pdz_buffer_free(&payload);
        if (pdz_member_payload_ex(junk, sizeof(junk), &member, NULL,
                                  PDZ_MEMBER_SCOPE_PREFIX, &payload) == PDG_OK)
            return 25;
        pdz_buffer_free(&payload);
    }
    (void)pdz_member_scope_for(junk, sizeof(junk));
    {
        static const char xml[] =
            "<cert><bookkey>ABCDEF012345</bookkey></cert>";
        char key[32] = {0};
        if (pdz_ssct_bookkey_from_xml(xml, sizeof(xml) - 1u, key, sizeof(key)) !=
            PDG_OK) return 26;
        if (strcmp(key, "ABCDEF012345") != 0) return 27;
    }
    {
        char key[32] = {0};
        if (pdz_ssct_bookkey(junk, sizeof(junk), key, sizeof(key)) == PDG_OK)
            return 28;
    }
    {
        uint8_t *xml = NULL;
        size_t xml_len = 0u;
        if (pdz_ssct_decode(junk, sizeof(junk), &xml, &xml_len) == PDG_OK)
            return 29;
        if (pdz_ssct_decode_key64(junk, sizeof(junk), "AAAA", &xml, &xml_len) ==
            PDG_OK) return 30;
        if (pdz_ssct_solve_type2(junk, sizeof(junk), NULL, 0u, &xml,
                                 &xml_len) == PDG_OK) return 31;
    }
    {
        uint8_t expanded[32];
        if (pdz_ssct_expand_key("ABCDEFGH", "tail", 4u, expanded) != PDG_OK)
            return 32;
    }
    {
        char hdd_key[64] = {0};
        (void)pdz_ssct_hdd2key("0123456789ABCDEF", hdd_key, sizeof(hdd_key));
    }
    {
        char hex[33] = {0};
        pdz_md5_hex_upper("abc", 3u, hex);
        if (strcmp(hex, "900150983CD24FB0D6963F7D28E17F72") != 0) return 33;
    }
    {
        uint8_t key16[16];
        if (pdz_derive_key("*s{P2a_1mP^?", key16) != PDG_OK) return 34;
    }
    {
        uint8_t *out = NULL;
        size_t out_size = 0u;
        if (pdz_inflate_raw(junk, sizeof(junk), &out, &out_size) == PDG_OK)
            return 35;
        free(out);
        out = NULL;
        if (pdz_inflate_zlib(junk, sizeof(junk), &out, &out_size) == PDG_OK)
            return 36;
        free(out);
    }
    {
        PdzInfoRule rule = {0};
        if (pdz_inforule_parse(junk, sizeof(junk), &rule) == PDG_OK) return 37;
        PdzRuleTable table = {0};
        if (pdz_inforule_rule_table(junk, sizeof(junk), &table) == PDG_OK)
            return 38;
        free((void *)table.raw);
    }
    {
        PdzMember members[1];
        PdzPageRange ranges[1];
        size_t count = 0u;
        memset(members, 0, sizeof(members));
        memcpy(members[0].name, "000001_1", 9u);
        members[0].offset = 0x60u;
        members[0].size = 16u;
        if (pdz_inforule_member_pages(members, 1u, ranges, 1u, &count) !=
            PDG_OK) return 39;
        if (count > 1u) return 40;
    }
    {
        uint8_t *jpeg = NULL;
        size_t jpeg_size = 0u;
        PdzPdfImage images[1];
        uint8_t *pdf = NULL;
        size_t pdf_size = 0u;
        size_t pages = 0u;
        size_t skipped = 0u;
        const uint8_t *blobs[1];
        size_t blob_sizes[1];
        static const char meta[] = "<meta><pages>3</pages></meta>";
        ConsumerPdfSink sink;
        memset(&sink, 0, sizeof(sink));
        if (!qw_consumer_read_fixture("tiny.jpg", &jpeg, &jpeg_size)) return 41;
        memset(images, 0, sizeof(images));
        images[0].data = jpeg;
        images[0].size = jpeg_size;
        images[0].kind = QW_PAGE_IMAGE_JPEG;
        images[0].width = 2u;
        images[0].height = 2u;
        rc = pdz_pdf_from_images(images, 1u, PDZ_PDF_PAGE_IMAGE, &pdf, &pdf_size,
                                 &pages);
        if (rc != PDG_OK || !pdf || pdf_size < 8u || pages != 1u ||
            memcmp(pdf, "%PDF-", 5u) != 0) return 42;
        pdz_pdf_free(pdf);
        pdf = NULL;
        blobs[0] = jpeg;
        blob_sizes[0] = jpeg_size;
        rc = pdz_pdf_from_pdg_files(blobs, blob_sizes, 1u, PDZ_PDF_PAGE_A4, &pdf,
                                    &pdf_size, &pages, &skipped);
        if (rc != PDG_OK || !pdf || pages != 1u) return 43;
        pdz_pdf_free(pdf);
        pdf = NULL;
        if (pdz_pdf_merge(blobs, blob_sizes, 1u, &pdf, &pdf_size, &pages) !=
            PDG_ERR_UNSUPPORTED) return 44;
        pdz_pdf_free(pdf);
        if (pdz_pdf_meta_pages(meta, sizeof(meta) - 1u) != 3u) return 45;
        if (!pdz_pdf_page_rule_name(PDZ_PDF_PAGES_ALL) ||
            !pdz_pdf_route_name(PDZ_PDF_ROUTE_IMAGES)) return 46;
        {
            PdzMember members[2];
            memset(members, 0, sizeof(members));
            memcpy(members[0].name, "000001_1", 9u);
            memcpy(members[1].name, "att001", 7u);
            (void)pdz_pdf_rule_count(members, 2u, PDZ_PDF_PAGES_ALL);
        }
        {
            PdzPdfReport report;
            uint8_t *out = NULL;
            size_t out_size = 0u;
            memset(&report, 0, sizeof(report));
            rc = pdz_pdf_from_pdz(junk, sizeof(junk), NULL, PDZ_PDF_PAGES_AUTO,
                                  PDZ_PDF_PAGE_A4, &out, &out_size, &report);
            if (rc == PDG_OK) return 47;
            pdz_pdf_free(out);
        }
        if (pdz_pdfpdg_is_member(junk, sizeof(junk)) != 0) return 48;
        {
            PdzRuleTable table;
            PdzRuleMember rule_member;
            uint32_t page_index = 0u;
            memset(&table, 0, sizeof(table));
            (void)pdz_inforule_rule_member(&table, 0u, &rule_member);
            (void)pdz_inforule_page_member(&table, 1u, &page_index);
        }
        /* Streaming writer: a caller sink receives the document one object at
         * a time. An empty writer must refuse to publish a page tree. */
        {
            PdzPdfWriter *writer = NULL;
            size_t written_pages = 0u;
            rc = pdz_pdf_writer_create(PDZ_PDF_PAGE_A4, consumer_pdf_sink,
                                       &sink, &writer);
            if (rc != PDG_OK || !writer) return 49;
            if (pdz_pdf_writer_finish(writer, &written_pages) != PDG_ERR_EMPTY) {
                pdz_pdf_writer_destroy(writer);
                return 50;
            }
            pdz_pdf_writer_destroy(writer);
            writer = NULL;
            sink.size = 0u;
            rc = pdz_pdf_writer_create(PDZ_PDF_PAGE_IMAGE, consumer_pdf_sink,
                                       &sink, &writer);
            if (rc != PDG_OK || !writer) return 51;
            if (pdz_pdf_writer_add_page(writer, &images[0]) != PDG_OK) {
                pdz_pdf_writer_destroy(writer);
                return 52;
            }
            if (pdz_pdf_writer_finish(writer, &written_pages) != PDG_OK ||
                written_pages != 1u || sink.size < 8u ||
                memcmp(sink.data, "%PDF-", 5u) != 0) {
                pdz_pdf_writer_destroy(writer);
                return 53;
            }
            pdz_pdf_writer_destroy(writer);
            free(sink.data);
        }
        /* Bitonal re-encode refuses pages that already carry a native
         * encoding; a caller must embed those verbatim. */
        {
            PdgBuffer ccitt;
            memset(&ccitt, 0, sizeof(ccitt));
            if (pdg_render_to_ccitt(jpeg, jpeg_size, &ccitt) == PDG_OK) {
                pdg_buffer_free(&ccitt);
                return 54;
            }
            pdg_buffer_free(&ccitt);
        }

        free(jpeg);
    }

    puts("PASS: installed PDG and PDZ primitives");
    return 0;
}
