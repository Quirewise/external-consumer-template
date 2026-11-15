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

    /* Target-aware conversion: a JSON fixture becomes YAML through the data
     * writer, not through a PDF route. */
    file = fopen("pipeline-consumer.json", "wb");
    if (!file) return 5;
    if (fputs("{\"name\":\"quirewise consumer\"}", file) == EOF) {
        fclose(file);
        return 5;
    }
    fclose(file);
    remove("pipeline-consumer.yaml");
    memset(&report, 0, sizeof(report));
    if (qw_pipeline_convert_file_to_format("pipeline-consumer.json",
                                           "pipeline-consumer.yaml",
                                           QW_FORMAT_YAML, &options,
                                           &report) != QW_STATUS_OK) {
        fprintf(stderr, "pipeline target convert failed: %s\n", report.note);
        remove("pipeline-consumer.json");
        return 6;
    }
    file = fopen("pipeline-consumer.yaml", "rb");
    if (!file) {
        remove("pipeline-consumer.json");
        return 7;
    }
    {
        char yaml[256];
        size_t length = fread(yaml, 1u, sizeof(yaml) - 1u, file);
        fclose(file);
        yaml[length] = '\0';
        remove("pipeline-consumer.yaml");
        remove("pipeline-consumer.json");
        if (!strstr(yaml, "quirewise consumer")) return 8;
    }
    puts("pipeline consumer OK");
    return 0;
}
