#include <quirewise/archive.h>
#include <quirewise/rar.h>
#include <quirewise/seven_zip.h>

#include "fixture_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static QwArchiveTarVisitAction tar_visitor(void *context, const char *name,
                                           const uint8_t *data, size_t size)
{
    int *seen = (int *)context;
    if (!name || !data || size != 5u) return QW_ARCHIVE_TAR_CONTINUE;
    if (memcmp(name, "hello.txt", 9u) == 0 && memcmp(data, "hello", 5u) == 0) {
        *seen = 1;
    }
    return QW_ARCHIVE_TAR_STOP;
}

static void write_octal(char *field, size_t width, unsigned int value)
{
    size_t index;
    for (index = 0u; index < width - 1u; ++index) field[index] = '0';
    field[width - 1u] = '\0';
    for (index = width - 1u; index > 0u && value; --index) {
        field[index - 1u] = (char)('0' + (value & 7u));
        value >>= 3;
    }
}

/* Minimal ustar archive: one regular file "hello.txt" containing "hello". */
static size_t build_ustar(uint8_t *archive, size_t capacity)
{
    unsigned int checksum = 0u;
    size_t index;
    if (capacity < 1536u) return 0u;
    memset(archive, 0, 1536u);
    memcpy(archive, "hello.txt", 9u);
    write_octal((char *)archive + 100, 8u, 0644u);
    write_octal((char *)archive + 108, 8u, 0u);
    write_octal((char *)archive + 116, 8u, 0u);
    write_octal((char *)archive + 124, 12u, 5u);
    write_octal((char *)archive + 136, 12u, 0u);
    memset(archive + 148, ' ', 8u);
    archive[156] = '0';
    memcpy(archive + 257, "ustar", 6u);
    memcpy(archive + 263, "00", 2u);
    for (index = 0u; index < 512u; ++index) checksum += archive[index];
    write_octal((char *)archive + 148, 7u, checksum);
    archive[155] = ' ';
    memcpy(archive + 512, "hello", 5u);
    return 1024u; /* header + data block; two zero blocks terminate */
}

int main(void)
{
    static const char expected[] = "Quirewise external consumer fixture\n";
    uint8_t *data = NULL;
    size_t size = 0u;
    const char *error = NULL;
    QwZipArchiveReader *reader = NULL;
    QwArchiveEntry entry = {0};
    QwArchiveBuffer extracted = {0};
    QwArchiveStatus status;
    int result = 0;

    if (!qw_consumer_read_fixture("archive.zip", &data, &size)) return 2;
    status = qw_zip_archive_open(data, size, &reader, &error);
    if (status != QW_ARCHIVE_OK || !reader) {
        free(data);
        return 3;
    }
    if (qw_zip_archive_entry_count(reader) != 1u) {
        fputs("expected one ZIP fixture entry\n", stderr);
        result = 4;
    } else if (qw_zip_archive_get_entry(reader, 0u, &entry) != QW_ARCHIVE_OK) {
        fputs("could not read ZIP fixture entry metadata\n", stderr);
        result = 5;
    } else if (entry.compression_method != 8u) {
        fprintf(stderr, "expected DEFLATE method 8, got %u\n",
                (unsigned)entry.compression_method);
        result = 6;
    } else {
        status = qw_zip_archive_extract(reader, 0u, NULL, &extracted);
        if (status != QW_ARCHIVE_OK) {
            fprintf(stderr, "could not extract ZIP fixture entry: %s (%s)\n",
                    qw_archive_status_string(status),
                    qw_zip_archive_last_error(reader));
            result = 7;
        } else if (extracted.size != sizeof(expected) - 1u) {
            fprintf(stderr, "unexpected ZIP fixture size: %zu\n",
                    extracted.size);
            result = 8;
        } else if (memcmp(extracted.data, expected,
                          sizeof(expected) - 1u) != 0) {
            fputs("ZIP fixture contents do not match\n", stderr);
            result = 9;
        }
    }
    qw_archive_buffer_free(&extracted);
    qw_zip_archive_close(reader);
    free(data);
    if (result != 0) return result;
    if (qw_archive_abi_version() == 0u) return 10;
    {
        uint8_t tar[2048];
        int seen = 0;
        size_t tar_size = build_ustar(tar, sizeof(tar));
        if (tar_size == 0u) return 11;
        status = qw_tar_archive_visit_ustar(tar, tar_size, tar_visitor, &seen);
        if (status != QW_ARCHIVE_OK || !seen) return 12;
    }
    {
        static const uint8_t junk[16] = {0x00u, 0x01u, 0x02u, 0x03u};
        QwSevenZipSink sink;
        memset(&sink, 0, sizeof(sink));
        if (qw_rar_archive_visit(junk, sizeof(junk), NULL, NULL, NULL) ==
            QW_RAR_OK) return 13;
        if (qw_seven_zip_probe(junk, sizeof(junk), NULL) == QW_SEVEN_ZIP_OK)
            return 14;
        if (qw_seven_zip_extract(junk, sizeof(junk), NULL, &sink) ==
            QW_SEVEN_ZIP_OK) return 15;
    }
    return 0;
}
