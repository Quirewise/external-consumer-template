#include <quirewise/audio.h>
#include <quirewise/encrypted_music.h>

#include "fixture_io.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    uint8_t *data = NULL;
    size_t size = 0u;
    QwAudioReader *reader = NULL;
    QwAudioInfo info = {0};
    QwAudioPacket packet;
    QwAudioStatus status;
    QwEncryptedMusicStatus enc_status;
    QwEncryptedMusicBuffer buffer = {0};
    char error[64];

    if (!qw_consumer_read_fixture("tone.wav", &data, &size)) return 2;
    status = qw_audio_reader_open(data, size, QW_AUDIO_INPUT_WAV, 0u, &reader);
    if (status != QW_AUDIO_OK || !reader) {
        fprintf(stderr, "Audio reader open failed with status %d\n", (int)status);
        free(data);
        return 3;
    }
    status = qw_audio_reader_get_info(reader, &info);
    if (status != QW_AUDIO_OK || info.sample_rate != 8000u) {
        qw_audio_reader_close(reader);
        free(data);
        return 4;
    }
    if (!qw_audio_codec_name(info.codec)) {
        qw_audio_reader_close(reader);
        free(data);
        return 5;
    }
    status = qw_audio_reader_next_packet(reader, &packet);
    if (status != QW_AUDIO_OK && status != QW_AUDIO_END &&
        status != QW_AUDIO_UNSUPPORTED) {
        qw_audio_reader_close(reader);
        free(data);
        return 6;
    }
    qw_audio_reader_close(reader);
    free(data); /* the reader borrows the input until close */

    if (qw_encrypted_music_abi_version() == 0u) return 7;
    if (!qw_encrypted_music_status_string(QW_ENCRYPTED_MUSIC_OK)) return 8;
    if (qw_encrypted_music_format_from_path("track.kgm") !=
        QW_ENCRYPTED_MUSIC_KGM) return 9;
    {
        static const uint8_t junk[16] = {0};
        if (qw_encrypted_music_probe_memory(junk, sizeof(junk)) !=
            QW_ENCRYPTED_MUSIC_UNKNOWN) return 10;
        enc_status = qw_encrypted_music_decrypt_buffer(
            QW_ENCRYPTED_MUSIC_KGM, junk, sizeof(junk), NULL, &buffer,
            error, sizeof(error));
        if (enc_status == QW_ENCRYPTED_MUSIC_OK || buffer.data != NULL) return 11;
    }
    qw_encrypted_music_buffer_free(&buffer);
    qw_encrypted_music_buffer_free(NULL);
    puts("PASS: installed Audio reader and encrypted-music API");
    return 0;
}
