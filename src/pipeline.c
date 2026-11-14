#include <quirewise/pipeline.h>
#include <quirewise/status.h>

#include <stdio.h>
#include <string.h>

#ifndef QUIREWISE_CONSUMER_FIXTURE_DIR
#define QUIREWISE_CONSUMER_FIXTURE_DIR "."
#endif

int main(void)
{
    QwPipelineOptions options = QW_PIPELINE_OPTIONS_INIT;
    QwConvertReport report;
    const char *version = qw_pipeline_version();
    FILE *file;
    char magic[5] = {0};

    if (!version || !version[0]) return 1;
    memset(&report, 0, sizeof(report));
    if (qw_pipeline_check_file(
            QUIREWISE_CONSUMER_FIXTURE_DIR "/sample.docx", &options,
            &report) != QW_STATUS_OK) {
        fprintf(stderr, "pipeline check failed: %s\n", report.note);
        return 2;
    }
    remove("pipeline-consumer-out.pdf");
    memset(&report, 0, sizeof(report));
    if (qw_pipeline_convert_file(
            QUIREWISE_CONSUMER_FIXTURE_DIR "/one-page.pdf",
            "pipeline-consumer-out.pdf", QW_FORMAT_PDF, &options,
            &report) != QW_STATUS_OK) {
        fprintf(stderr, "pipeline convert failed: %s\n", report.note);
        return 3;
    }
    file = fopen("pipeline-consumer-out.pdf", "rb");
    if (!file || fread(magic, 1u, 5u, file) != 5u ||
        memcmp(magic, "%PDF-", 5u) != 0) {
        if (file) fclose(file);
        remove("pipeline-consumer-out.pdf");
        return 4;
    }
    fclose(file);
    remove("pipeline-consumer-out.pdf");
    puts("pipeline consumer OK");
    return 0;
}
