#include <quirewise/plain_text.h>
#include <quirewise/rtf.h>

#include <string.h>

static int contains_bytes(const uint8_t *data, size_t size,
                          const uint8_t *expected, size_t expected_size)
{
    size_t i;
    if (!data || !expected || expected_size > size) return 0;
    for (i = 0u; i <= size - expected_size; ++i) {
        if (memcmp(data + i, expected, expected_size) == 0) return 1;
    }
    return 0;
}

int main(void)
{
    static const uint8_t gbk_text[] = {
        0xd6u, 0xd0u, 0xceu, 0xc4u, '\n'
    };
    static const uint8_t expected_chinese[] = {
        0xe4u, 0xb8u, 0xadu, 0xe6u, 0x96u, 0x87u
    };
    static const uint8_t rtf_cyrillic[] =
        "{\\rtf1\\ansi\\ansicpg1251 \xcf\xf0\xe8\xe2\xe5\xf2}";
    static const uint8_t expected_cyrillic[] = {
        0xd0u, 0x9fu, 0xd1u, 0x80u, 0xd0u, 0xb8u,
        0xd0u, 0xb2u, 0xd0u, 0xb5u, 0xd1u, 0x82u
    };
    QwPlainTextDocument *text_document = NULL;
    QwPlainTextStatus text_status;
    QwRtfDocument rtf_document = {0};
    size_t html_size = 0u;
    const uint8_t *html;

    text_status = qw_plain_text_parse(gbk_text, sizeof(gbk_text),
                                      &text_document);
    if (text_status != QW_PLAIN_TEXT_OK || !text_document) return 1;
    html = qw_plain_text_document_html(text_document, &html_size);
    if (!contains_bytes(html, html_size, expected_chinese,
                        sizeof(expected_chinese))) {
        qw_plain_text_document_free(text_document);
        return 2;
    }
    qw_plain_text_document_free(text_document);

    if (qw_rtf_parse_html(rtf_cyrillic, sizeof(rtf_cyrillic) - 1u,
                          &rtf_document) != QW_RTF_OK) return 3;
    if (!contains_bytes((const uint8_t *)rtf_document.html,
                        rtf_document.html_size, expected_cyrillic,
                        sizeof(expected_cyrillic))) {
        qw_rtf_document_free(&rtf_document);
        return 4;
    }
    qw_rtf_document_free(&rtf_document);
    return 0;
}
