#include <quirewise/plain_text.h>

int main(void)
{
    static const char text[] = "Plain text consumer sample.\n";
    QwPlainTextDocument *document = NULL;
    QwPlainTextStatus status = qw_plain_text_parse(
        text, sizeof(text) - 1u, &document);
    if (status != 0 || !document) return 1;
    qw_plain_text_document_free(document);
    return 0;
}
