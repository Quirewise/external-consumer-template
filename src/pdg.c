#include <pdg.h>
#include <pdz.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int check_text(const char *value)
{
    return value != NULL && value[0] != '\0';
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
    puts("PASS: installed PDG and PDZ primitives");
    return 0;
}
