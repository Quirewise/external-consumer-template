#include <quirewise/spreadsheet.h>

int main(void)
{
    static const char csv[] = "name,value\r\nalpha,1\r\n";
    QwSpreadsheetDocument *document = NULL;
    QwSpreadsheetStatus status = qw_spreadsheet_parse(
        QW_SPREADSHEET_FORMAT_CSV, csv, sizeof(csv) - 1u, &document);
    if (status != QW_SPREADSHEET_OK || !document) return 1;
    status = qw_spreadsheet_sheet_count(document)
        ? QW_SPREADSHEET_OK : QW_SPREADSHEET_ERR_FORMAT;
    qw_spreadsheet_document_free(document);
    return status == QW_SPREADSHEET_OK ? 0 : 2;
}
