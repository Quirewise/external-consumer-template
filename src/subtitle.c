#include <quirewise/subtitle.h>

#include <string.h>

int main(void)
{
    static const char srt[] =
        "1\r\n00:00:00,000 --> 00:00:00,500\r\nHello\r\n";
    QwSubtitle *document = NULL;
    QwSubtitleInfo info = {0};
    QwSubtitleStatus status = qw_subtitle_parse(
        srt, sizeof(srt) - 1u, QW_SUBTITLE_INPUT_SRT, &document);
    if (status != QW_SUBTITLE_OK || !document) return 1;
    if (!qw_subtitle_format_name(QW_SUBTITLE_INPUT_SRT)) {
        qw_subtitle_free(document);
        return 3;
    }
    {
        QwSubtitleCueView cue;
        if (qw_subtitle_get_cue(document, 0u, &cue) != QW_SUBTITLE_OK) {
            qw_subtitle_free(document);
            return 4;
        }
    }
    status = qw_subtitle_get_info(document, &info);
    {
        QwConvertReport report;
        uint8_t *written = NULL;
        size_t written_size = 0u;

        memset(&report, 0, sizeof(report));
        if (qw_subtitle_write(document, QW_FORMAT_VTT, NULL, &written,
                              &written_size, &report) != QW_SUBTITLE_OK ||
            !written || written_size < 6u ||
            memcmp(written, "WEBVTT", 6u) != 0) {
            qw_subtitle_buffer_free(written);
            qw_subtitle_free(document);
            return 5;
        }
        qw_subtitle_buffer_free(written);
        if (qw_subtitle_write(document, QW_FORMAT_PDF, NULL, &written,
                              &written_size, &report) !=
            QW_SUBTITLE_UNSUPPORTED) {
            qw_subtitle_free(document);
            return 6;
        }
    }
    qw_subtitle_free(document);
    return status == QW_SUBTITLE_OK && info.cue_count == 1u ? 0 : 2;
}
