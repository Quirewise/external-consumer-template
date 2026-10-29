#include <quirewise/email.h>

int main(void)
{
    static const char message[] =
        "From: sender@example.test\r\n"
        "To: reader@example.test\r\n"
        "Subject: external consumer\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "\r\n"
        "Hello from the external consumer.\r\n";
    QwEmailDocument *document = NULL;
    QwEmailStatus status = qw_email_parse(message, sizeof(message) - 1u,
                                          &document);
    if (status != QW_EMAIL_OK || !document) return 1;
    qw_email_document_free(document);
    return 0;
}
