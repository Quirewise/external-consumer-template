#include <quirewise/subtitle.h>

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
    qw_subtitle_free(document);
    return status == QW_SUBTITLE_OK && info.cue_count == 1u ? 0 : 2;
}
