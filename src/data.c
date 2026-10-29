#include <quirewise/data.h>

#include <stdint.h>
#include <string.h>

int main(void)
{
    static const char json[] = "{\"enabled\":true}";
    static const char xml[] =
        "<?xml version=\"1.0\" encoding=\"GB18030\"?><root>"
        "\xd6\xd0\xce\xc4</root>";
    static const uint8_t expected_utf8[] = {
        0xe4u, 0xb8u, 0xadu, 0xe6u, 0x96u, 0x87u
    };
    QwDataDocument *document = NULL;
    QwDataStatus status = qw_data_parse(QW_DATA_FORMAT_JSON, json,
                                        sizeof(json) - 1u, &document);
    const QwDataValue *xml_element;
    const QwDataValue *content;
    const QwDataValue *text_node;
    const char *text;
    size_t text_size;

    if (status != QW_DATA_OK || !document) return 1;
    if (!qw_data_document_root(document)) {
        qw_data_document_free(document);
        return 2;
    }
    qw_data_document_free(document);
    document = NULL;

    status = qw_data_parse(QW_DATA_FORMAT_XML, xml, sizeof(xml) - 1u,
                           &document);
    if (status != QW_DATA_OK || !document) return 3;
    xml_element = qw_data_object_find(qw_data_document_root(document),
                                      "$xml", 4u);
    content = qw_data_object_find(xml_element, "$content", 8u);
    text_node = qw_data_array_at(content, 0u);
    status = qw_data_value_text(text_node, &text, &text_size);
    if (status != QW_DATA_OK || text_size != sizeof(expected_utf8) ||
        memcmp(text, expected_utf8, sizeof(expected_utf8)) != 0) {
        qw_data_document_free(document);
        return 4;
    }
    qw_data_document_free(document);
    return 0;
}
