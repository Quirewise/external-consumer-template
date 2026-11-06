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
    {
        QwSpreadsheetCellView cell;
        const QwSpreadsheetSheet *sheet;
        size_t name_size = 0u;
        if (qw_spreadsheet_document_format(document) != QW_SPREADSHEET_FORMAT_CSV) {
            qw_spreadsheet_document_free(document);
            return 3;
        }
        sheet = qw_spreadsheet_sheet_at(document, 0u);
        if (!sheet) {
            qw_spreadsheet_document_free(document);
            return 4;
        }
        if (!qw_spreadsheet_sheet_name(sheet, &name_size) || name_size == 0u) {
            qw_spreadsheet_document_free(document);
            return 5;
        }
        if (qw_spreadsheet_cell_count(sheet) < 4u) {
            qw_spreadsheet_document_free(document);
            return 6;
        }
        if (qw_spreadsheet_cell_at(sheet, 0u, &cell) != QW_SPREADSHEET_OK ||
            cell.kind != QW_SPREADSHEET_CELL_STRING || !cell.value) {
            qw_spreadsheet_document_free(document);
            return 7;
        }
    }
    qw_spreadsheet_document_free(document);
    return status == QW_SPREADSHEET_OK ? 0 : 2;
}
