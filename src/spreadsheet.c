#include <quirewise/spreadsheet.h>

#include <string.h>

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
    document = NULL;
    if (qw_spreadsheet_parse(QW_SPREADSHEET_FORMAT_CSV, csv,
                             sizeof(csv) - 1u, &document) !=
            QW_SPREADSHEET_OK ||
        !document)
        return 8;
    {
        QwConvertReport report;
        uint8_t *written = NULL;
        size_t written_size = 0u;

        memset(&report, 0, sizeof(report));
        if (qw_spreadsheet_write_document(document, QW_FORMAT_TSV, NULL,
                                          &written, &written_size,
                                          &report) != QW_SPREADSHEET_OK ||
            !written || !written_size || !strchr((const char *)written, '\t')) {
            qw_spreadsheet_buffer_free(written);
            qw_spreadsheet_document_free(document);
            return 9;
        }
        qw_spreadsheet_buffer_free(written);
        if (qw_spreadsheet_write_document(document, QW_FORMAT_PNG, NULL,
                                          &written, &written_size,
                                          &report) !=
            QW_SPREADSHEET_ERR_UNSUPPORTED) {
            qw_spreadsheet_document_free(document);
            return 10;
        }
    }
    {
        const QwSpreadsheetSheet *sheet =
            qw_spreadsheet_sheet_at(document, 0u);
        if (qw_spreadsheet_sheet_merge_count(NULL) != 0u) return 11;
        if (qw_spreadsheet_sheet_merge_count(sheet) != 0u) return 12;
        if (qw_spreadsheet_sheet_merge_at(sheet, 0u, NULL, NULL, NULL, NULL) !=
            QW_SPREADSHEET_ERR_INVALID_ARGUMENT) return 13;
    }
    qw_spreadsheet_document_free(document);
    return status == QW_SPREADSHEET_OK ? 0 : 2;
}
