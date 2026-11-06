#include <quirewise/data.h>
#include <quirewise/docbook.h>
#include <quirewise/opml.h>
#include <quirewise/xml.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int walk_xml(void)
{
    static const char source[] =
        "<?xml version=\"1.0\"?>\n"
        "<?note attached?>\n"
        "<root xmlns:ns=\"urn:test\" id=\"r1\">\n"
        "  <!-- comment -->\n"
        "  <ns:child ns:attr=\"v\" plain=\"p\">text</ns:child>\n"
        "  <second>two</second>\n"
        "</root>";
    QwXmlError error = {0};
    QwXmlParseOptions options = {0};
    QwXmlNode *document;
    QwXmlNode *root;
    QwXmlNode *child;
    QwXmlNode *second;
    QwXmlNode *text;
    uint8_t *serialized = NULL;
    size_t serialized_size = 0u;

    if (qw_xml_abi_version() != QW_XML_ABI_VERSION) return 1;
    document = qw_xml_parse(source, sizeof(source) - 1u, &error);
    if (!document || error.code != QW_XML_ERROR_NONE) return 2;
    if (qw_xml_node_kind(document) != QW_XML_NODE_DOCUMENT) return 3;
    root = qw_xml_root(document);
    if (!root || qw_xml_node_kind(root) != QW_XML_NODE_ELEMENT) return 4;
    if (!qw_xml_is_tag(root, "root")) return 5;
    if (!qw_xml_tag(root) || strcmp(qw_xml_tag(root), "root") != 0) return 6;
    if (!qw_xml_qualified_tag(root) || !qw_xml_local_name(root)) return 7;
    if (qw_xml_namespace_uri(root) != NULL) return 8;
    if (!qw_xml_att(root, "id") || strcmp(qw_xml_att(root, "id"), "r1")) return 9;
    if (!qw_xml_att_eq(root, "id", "r1")) return 10;
    if (!qw_xml_att_alt(root, "missing", "id")) return 11;
    {
        size_t attribute_count = qw_xml_attribute_count(root);
        size_t attribute_index;
        int found_id = 0;
        if (attribute_count < 1u) return 12;
        for (attribute_index = 0u; attribute_index < attribute_count;
             ++attribute_index) {
            const char *name = qw_xml_attribute_name(root, attribute_index);
            if (!name || !qw_xml_attribute_local_name(root, attribute_index) ||
                !qw_xml_attribute_value(root, attribute_index)) return 13;
            (void)qw_xml_attribute_namespace_uri(root, attribute_index);
            if (strcmp(name, "id") == 0) found_id = 1;
        }
        if (!found_id) return 13;
    }
    if (!qw_xml_att_ns(root, NULL, "id")) return 14;
    child = qw_xml_find_down(root, "child");
    if (!child) return 16;
    second = qw_xml_find_next(child, "second");
    if (!second) return 17;
    if (qw_xml_find(root, "second") != NULL) return 18;
    if (!qw_xml_find_match(child, "second", NULL, NULL)) return 19;
    if (!qw_xml_find_next_match(child, "second", NULL, NULL)) return 20;
    if (!qw_xml_find_down_match(root, "child", "plain", "p")) return 21;
    if (!qw_xml_find_dfs(root, "child", NULL, NULL)) return 22;
    if (!qw_xml_find_dfs_top(root, "child", NULL, NULL, root)) return 23;
    if (!qw_xml_find_next_dfs(child, "second", NULL, NULL)) return 24;
    if (!qw_xml_find_next_dfs_top(child, "second", NULL, NULL, root)) return 25;
    if (qw_xml_down(root) == NULL) return 25;
    if (qw_xml_next(qw_xml_down(root)) == NULL) return 26;
    if (qw_xml_prev(qw_xml_next(qw_xml_down(root))) == NULL) return 27;
    if (qw_xml_up(child) != root) return 28;
    {
        QwXmlNode *text_node = qw_xml_down(child);
        while (text_node && qw_xml_node_kind(text_node) != QW_XML_NODE_TEXT)
            text_node = qw_xml_next(text_node);
        if (!text_node || !qw_xml_text(text_node) ||
            strcmp(qw_xml_text(text_node), "text") != 0) return 29;
    }
    if (!qw_xml_local_name(child) || strcmp(qw_xml_local_name(child), "child"))
        return 30;
    if (!qw_xml_namespace_uri(child) ||
        strcmp(qw_xml_namespace_uri(child), "urn:test")) return 31;
    if (qw_xml_serialize(root, &serialized, &serialized_size, &error) == 0 ||
        !serialized || serialized_size == 0u) return 32;
    free(serialized);
    qw_xml_drop(document);

    document = qw_xml_parse_with_options(source, sizeof(source) - 1u, &options,
                                         &error);
    if (!document) return 33;
    {
        QwXmlNode *comment = qw_xml_find_dfs(qw_xml_root(document), NULL, NULL,
                                             NULL);
        (void)comment;
    }
    {
        QwXmlNode *node = qw_xml_down(qw_xml_root(document));
        int saw_comment = 0;
        int saw_pi = 0;
        while (node) {
            if (qw_xml_node_kind(node) == QW_XML_NODE_COMMENT) {
                if (qw_xml_comment(node)) saw_comment = 1;
            } else if (qw_xml_node_kind(node) == QW_XML_NODE_PROCESSING_INSTRUCTION) {
                if (qw_xml_pi_target(node) && qw_xml_pi_data(node)) saw_pi = 1;
            }
            node = qw_xml_next(node);
        }
        if (!saw_comment) return 34;
        /* The processing instruction sits before the root element. */
        if (!saw_pi) {
            QwXmlNode *first = qw_xml_down(document);
            if (!first || qw_xml_node_kind(first) !=
                              QW_XML_NODE_PROCESSING_INSTRUCTION)
                return 35;
        }
    }
    text = qw_xml_find_dfs(qw_xml_root(document), "child", NULL, NULL);
    if (!text) return 36;
    qw_xml_drop(document);
    return 0;
}

int main(void)
{
    static const char json[] = "{\"enabled\":true,\"list\":[1,2,3]}";
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

    document = NULL;
    if (qw_data_parse(QW_DATA_FORMAT_JSON, json, sizeof(json) - 1u, &document) !=
        QW_DATA_OK || !document) return 5;
    if (qw_data_document_format(document) != QW_DATA_FORMAT_JSON) return 6;
    {
        const QwDataValue *root = qw_data_document_root(document);
        const char *key = NULL;
        size_t key_size = 0u;
        int enabled = 0;
        const QwDataValue *list;
        if (qw_data_value_kind(root) != QW_DATA_KIND_OBJECT) return 7;
        if (qw_data_value_count(root) != 2u) return 8;
        if (!qw_data_object_at(root, 0u, &key, &key_size) || !key) return 9;
        if (key_size != 7u || memcmp(key, "enabled", 7u) != 0) return 10;
        if (qw_data_object_at(root, 1u, NULL, NULL) == NULL) return 11;
        if (qw_data_value_boolean(qw_data_object_find(root, "enabled", 7u),
                                  &enabled) != QW_DATA_OK || !enabled) return 12;
        list = qw_data_object_find(root, "list", 4u);
        if (!list || qw_data_value_kind(list) != QW_DATA_KIND_ARRAY) return 13;
        if (qw_data_value_count(list) != 3u) return 14;
    }
    qw_data_document_free(document);

    {
        static const char docbook[] =
            "<book><title>Consumer</title><para>DocBook fixture.</para></book>";
        QwDocBookDocument doc_book = {0};
        if (qw_docbook_abi_version() != QW_DOCBOOK_ABI_VERSION) return 15;
        if (!qw_docbook_status_string(QW_DOCBOOK_OK)) return 16;
        if (qw_docbook_parse_html(docbook, sizeof(docbook) - 1u, &doc_book) !=
            QW_DOCBOOK_OK) return 17;
        if (!doc_book.html_utf8 || !doc_book.html_size) return 18;
        qw_docbook_document_free(&doc_book);
    }
    {
        static const char opml[] =
            "<opml version=\"2.0\"><body><outline text=\"Feed\" "
            "xmlUrl=\"https://example.test/rss\"/></body></opml>";
        QwOpmlDocument opml_document = {0};
        if (qw_opml_abi_version() != QW_OPML_ABI_VERSION) return 19;
        if (!qw_opml_status_string(QW_OPML_OK)) return 20;
        if (qw_opml_parse_html(opml, sizeof(opml) - 1u, &opml_document) !=
            QW_OPML_OK) return 21;
        if (!opml_document.html_utf8 || !opml_document.html_size) return 22;
        qw_opml_document_free(&opml_document);
    }
    return walk_xml();
}
