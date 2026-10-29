#include <quirewise/audio.h>

#include "fixture_io.h"

#include <stdio.h>

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    QwAudioReader *reader = NULL;
    QwAudioInfo info = {0};
    QwAudioStatus status;

    if (!qw_consumer_read_fixture("tone.wav", &data, &size)) return 2;
    status = qw_audio_reader_open(data, size, QW_AUDIO_INPUT_WAV, 0u, &reader);
    free(data);
    if (status != QW_AUDIO_OK || !reader) {
        fprintf(stderr, "Audio reader open failed with status %d\n",
                (int)status);
        return 3;
    }
    status = qw_audio_reader_get_info(reader, &info);
    qw_audio_reader_close(reader);
    return status == QW_AUDIO_OK && info.sample_rate == 8000u ? 0 : 4;
}
