#include <quirewise/page_status.h>
#include <quirewise/pdf_parser.h>

#include "fixture_io.h"

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    QwPdfParser *parser = NULL;
    int status;

    if (!qw_consumer_read_fixture("one-page.pdf", &data, &size)) return 2;
    status = qw_pdf_parser_open(data, size, &parser);
    free(data);
    if (status != QW_PAGE_OK || !parser) return 3;
    qw_pdf_parser_close(parser);
    return 0;
}
