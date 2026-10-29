#include <quirewise/data.h>

int main(void)
{
    static const char json[] = "{\"enabled\":true}";
    QwDataDocument *document = NULL;
    QwDataStatus status = qw_data_parse(QW_DATA_FORMAT_JSON, json,
                                        sizeof(json) - 1u, &document);
    if (status != QW_DATA_OK || !document) return 1;
    status = qw_data_document_root(document) ? QW_DATA_OK : QW_DATA_ERR_FORMAT;
    qw_data_document_free(document);
    return status == QW_DATA_OK ? 0 : 2;
}
