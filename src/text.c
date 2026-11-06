#include <quirewise/asciidoc.h>
#include <quirewise/html_grid.h>
#include <quirewise/latex.h>
#include <quirewise/man.h>
#include <quirewise/markdown.h>
#include <quirewise/mediawiki.h>
#include <quirewise/org.h>
#include <quirewise/plain_text.h>
#include <quirewise/rst.h>
#include <quirewise/rtf.h>
#include <quirewise/textile.h>
#include <quirewise/typst.h>

#include <string.h>

typedef struct GridCounts {
    int tables;
    int rows;
    int cells;
} GridCounts;

static int grid_begin_row(void *context, size_t row_index)
{
    (void)row_index;
    ++((GridCounts *)context)->rows;
    return 0;
}

static int grid_cell(void *context, size_t row_index, size_t column_index,
                     const uint8_t *text, size_t text_size)
{
    (void)row_index;
    (void)column_index;
    if (text && text_size) ++((GridCounts *)context)->cells;
    return 0;
}

static int grid_end_row(void *context, size_t row_index)
{
    (void)context;
    (void)row_index;
    return 0;
}

static int grid_begin_table(void *context, size_t table_index,
                            const uint8_t *name, size_t name_size)
{
    (void)name;
    (void)name_size;
    (void)table_index;
    ++((GridCounts *)context)->tables;
    return 0;
}

static int grid_end_table(void *context, size_t table_index)
{
    (void)context;
    (void)table_index;
    return 0;
}

int main(void)
{
    static const char html_table[] =
        "<table><tr><td>one</td><td>two</td></tr>"
        "<tr><td>three</td><td>four</td></tr></table>";
    static const char html_tables[] =
        "<table><tr><td>a</td></tr></table>"
        "<table><tr><td>b</td></tr></table>";
    GridCounts counts = {0, 0, 0};
    QwHtmlGridVisitor grid = {grid_begin_row, grid_cell, grid_end_row};
    QwHtmlGridDocumentVisitor document_grid = {
        grid_begin_table, {grid_begin_row, grid_cell, grid_end_row},
        grid_end_table
    };

    {
        QwAsciiDocDocument document = {0};
        static const char input[] = "= Title\n\nHello.";
        if (qw_asciidoc_abi_version() != QW_ASCIIDOC_ABI_VERSION) return 1;
        if (!qw_asciidoc_status_string(QW_ASCIIDOC_OK)) return 2;
        if (qw_asciidoc_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_ASCIIDOC_OK || !document.markdown || !document.markdown_size)
            return 3;
        qw_asciidoc_document_free(&document);
    }
    {
        QwLatexDocument document = {0};
        static const char input[] = "\\section{Title}\nHello.";
        if (qw_latex_abi_version() != QW_LATEX_ABI_VERSION) return 4;
        if (!qw_latex_status_string(QW_LATEX_OK)) return 5;
        if (qw_latex_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_LATEX_OK || !document.markdown || !document.markdown_size)
            return 6;
        qw_latex_document_free(&document);
    }
    {
        QwManDocument document = {0};
        static const char input[] = ".TH TEST 1\n.SH NAME\nconsumer\n";
        if (qw_man_abi_version() != QW_MAN_ABI_VERSION) return 7;
        if (!qw_man_status_string(QW_MAN_OK)) return 8;
        if (qw_man_parse_html(input, sizeof(input) - 1u, &document) != QW_MAN_OK ||
            !document.html_utf8 || !document.html_size)
            return 9;
        qw_man_document_free(&document);
    }
    {
        QwMarkdownDocument document = {0};
        static const char input[] = "# Title\n\nHello.";
        if (qw_markdown_abi_version() != QW_MARKDOWN_ABI_VERSION) return 10;
        if (!qw_markdown_status_string(QW_MARKDOWN_OK)) return 11;
        if (qw_markdown_render_html(input, sizeof(input) - 1u, &document) !=
            QW_MARKDOWN_OK || !document.html || !document.html_size)
            return 12;
        qw_markdown_document_free(&document);
    }
    {
        QwMediaWikiDocument document = {0};
        static const char input[] = "== Title ==\nHello.";
        if (qw_mediawiki_abi_version() != QW_MEDIAWIKI_ABI_VERSION) return 13;
        if (!qw_mediawiki_status_string(QW_MEDIAWIKI_OK)) return 14;
        if (qw_mediawiki_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_MEDIAWIKI_OK || !document.markdown || !document.markdown_size)
            return 15;
        qw_mediawiki_document_free(&document);
    }
    {
        QwOrgDocument document = {0};
        static const char input[] = "* Title\nHello.";
        if (qw_org_abi_version() != QW_ORG_ABI_VERSION) return 16;
        if (!qw_org_status_string(QW_ORG_OK)) return 17;
        if (qw_org_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_ORG_OK || !document.markdown || !document.markdown_size)
            return 18;
        qw_org_document_free(&document);
    }
    {
        QwRstDocument document = {0};
        static const char input[] = "Title\n=====\n\nHello.";
        if (qw_rst_abi_version() != QW_RST_ABI_VERSION) return 19;
        if (!qw_rst_status_string(QW_RST_OK)) return 20;
        if (qw_rst_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_RST_OK || !document.markdown || !document.markdown_size)
            return 21;
        qw_rst_document_free(&document);
    }
    {
        QwTextileDocument document = {0};
        static const char input[] = "h1. Title\n\nHello.";
        if (qw_textile_abi_version() != QW_TEXTILE_ABI_VERSION) return 22;
        if (!qw_textile_status_string(QW_TEXTILE_OK)) return 23;
        if (qw_textile_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_TEXTILE_OK || !document.markdown || !document.markdown_size)
            return 24;
        qw_textile_document_free(&document);
    }
    {
        QwTypstDocument document = {0};
        static const char input[] = "= Title\n\nHello.";
        if (qw_typst_abi_version() != QW_TYPST_ABI_VERSION) return 25;
        if (!qw_typst_status_string(QW_TYPST_OK)) return 26;
        if (qw_typst_parse_markdown(input, sizeof(input) - 1u, &document) !=
            QW_TYPST_OK || !document.markdown || !document.markdown_size)
            return 27;
        qw_typst_document_free(&document);
    }
    if (qw_html_grid_visit_single_table((const uint8_t *)html_table,
                                        sizeof(html_table) - 1u, &grid,
                                        &counts) != QW_HTML_GRID_OK) return 28;
    if (counts.rows != 2 || counts.cells != 4) return 29;
    if (qw_html_grid_visit_tables((const uint8_t *)html_tables,
                                  sizeof(html_tables) - 1u, &document_grid,
                                  &counts) != QW_HTML_GRID_OK) return 30;
    if (counts.tables != 2) return 31;

    {
        static const uint8_t gbk_text[] = {0xd6u, 0xd0u, 0xceu, 0xc4u, '\n'};
        QwPlainTextDocument *document = NULL;
        size_t html_size = 0u;
        if (qw_plain_text_probe(gbk_text, sizeof(gbk_text)) == 0) return 32;
        if (qw_plain_text_parse(gbk_text, sizeof(gbk_text), &document) !=
            QW_PLAIN_TEXT_OK || !document) return 33;
        if (!qw_plain_text_document_html(document, &html_size) || !html_size)
            return 34;
        qw_plain_text_document_free(document);
    }
    {
        static const char rtf[] =
            "{\\rtf1\\ansi\\deff0{\\fonttbl{\\f0 Times;}}"
            "\\f0 External consumer RTF.\\par}";
        QwRtfDocument document = {0};
        if (qw_rtf_abi_version() != QW_RTF_ABI_VERSION) return 35;
        if (!qw_rtf_status_string(QW_RTF_OK)) return 36;
        if (qw_rtf_parse_html(rtf, sizeof(rtf) - 1u, &document) != QW_RTF_OK)
            return 37;
        if (!document.html || !document.html_size) return 38;
        qw_rtf_document_free(&document);
    }
    return 0;
}
