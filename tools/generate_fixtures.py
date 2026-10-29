#!/usr/bin/env python3
"""Regenerate the small, dependency-free binary fixtures shipped with the consumer."""

from __future__ import annotations

import binascii
import io
import struct
import wave
import zipfile
import zlib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "fixtures"


def png_1x1() -> bytes:
    def chunk(kind: bytes, data: bytes) -> bytes:
        crc = binascii.crc32(kind + data) & 0xFFFFFFFF
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", crc)

    return (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, 8, 2, 0, 0, 0))
        + chunk(b"IDAT", zlib.compress(b"\x00\x33\x66\x99"))
        + chunk(b"IEND", b"")
    )


def bmp_1x1() -> bytes:
    pixel_offset = 14 + 40
    pixel_data = b"\x99\x66\x33\x00"
    file_size = pixel_offset + len(pixel_data)
    file_header = b"BM" + struct.pack("<IHHI", file_size, 0, 0, pixel_offset)
    dib_header = struct.pack(
        "<IiiHHIIiiII", 40, 1, 1, 1, 24, 0, len(pixel_data), 2835, 2835, 0, 0
    )
    return file_header + dib_header + pixel_data


def pdf_one_page() -> bytes:
    objects = [
        b"<< /Type /Catalog /Pages 2 0 R >>",
        b"<< /Type /Pages /Kids [3 0 R] /Count 1 >>",
        b"<< /Type /Page /Parent 2 0 R /MediaBox [0 0 1 1] /Resources << >> >>",
    ]
    result = bytearray(b"%PDF-1.4\n%\xe2\xe3\xcf\xd3\n")
    offsets = [0]
    for index, body in enumerate(objects, 1):
        offsets.append(len(result))
        result.extend(f"{index} 0 obj\n".encode("ascii"))
        result.extend(body + b"\nendobj\n")
    xref_offset = len(result)
    result.extend(f"xref\n0 {len(offsets)}\n".encode("ascii"))
    result.extend(b"0000000000 65535 f \n")
    for offset in offsets[1:]:
        result.extend(f"{offset:010d} 00000 n \n".encode("ascii"))
    result.extend(
        f"trailer\n<< /Size {len(offsets)} /Root 1 0 R >>\n"
        f"startxref\n{xref_offset}\n%%EOF\n".encode("ascii")
    )
    return bytes(result)


def founder_keystream(key: bytes, size: int) -> bytes:
    state = list(range(256))
    j = 0
    for i in range(256):
        j = (j + state[i] + (key[i % len(key)] | 0xAA)) & 0xFF
        state[i], state[j] = state[j], state[i]
    output = bytearray(size)
    i = 0
    for offset in range(size):
        i = (i + 1) & 0xFF
        old_si = state[i]
        j = (j + old_si) & 0xFF
        state[i] = state[j]
        state[j] = state[old_si]
        output[offset] = state[(state[old_si] + state[i]) & 0xFF]
    return bytes(output)


def ceb_synthetic(*, invalid_rsa_key: bool = False) -> bytes:
    founder_key = bytes.fromhex("00112233445566778899aabbccddeeff")
    triple_des_key = bytes.fromhex(
        "0123456789abcdeffedcba987654321089abcdef01234567"
    )
    algorithm_id = 0x80000002 if invalid_rsa_key else 2
    algorithm = struct.pack("<I", algorithm_id)
    encrypted_stream = bytes.fromhex("2a520589edc5a609")
    pdf = (
        b"%PDF-1.4\n"
        b"1 0 obj\n<< /Length 8 >>\nstream\n"
        + encrypted_stream
        + b"\nendstream\nendobj\ntrailer\n<< /Encrypt 1 0 R >>\n%%EOF\n"
    )
    stream = founder_keystream(founder_key, len(pdf))
    encrypted_pdf = bytes(a ^ b for a, b in zip(pdf, stream))
    symmetric_key = bytes(64) if invalid_rsa_key else triple_des_key
    sections = [
        (encrypted_pdf, 0),
        (founder_key, 3),
        (symmetric_key, 2),
        (algorithm, 4),
    ]
    index_start = 0x1F
    index_item_size = 17
    result = bytearray(index_start + len(sections) * index_item_size)
    result[:11] = b"Founder CEB"
    struct.pack_into("<H", result, 0x14, len(sections))
    for index, (payload, section_type) in enumerate(sections):
        offset = len(result)
        item = index_start + index * index_item_size
        struct.pack_into("<II", result, item, offset, len(payload))
        result[item + 8] = section_type
        result.extend(payload)
    return bytes(result)


def write_zip(path: Path, entries: list[tuple[str, bytes]], *, epub: bool = False) -> None:
    with zipfile.ZipFile(path, "w") as archive:
        for name, data in entries:
            compression = zipfile.ZIP_STORED if epub and name == "mimetype" else zipfile.ZIP_DEFLATED
            archive.writestr(name, data, compress_type=compression)


def main() -> None:
    FIXTURES.mkdir(parents=True, exist_ok=True)
    image_png = png_1x1()
    image_bmp = bmp_1x1()

    write_zip(FIXTURES / "archive.zip", [("readme.txt", b"Quirewise external consumer fixture\n")])
    write_zip(FIXTURES / "sample.cbz", [("001.png", image_png)])

    container_xml = (
        b'<?xml version="1.0"?>'
        b'<container xmlns="urn:oasis:names:tc:opendocument:xmlns:container" version="1.0">'
        b'<rootfiles><rootfile full-path="EPUB/package.opf" '
        b'media-type="application/oebps-package+xml"/></rootfiles></container>'
    )
    package_opf = (
        b'<?xml version="1.0" encoding="UTF-8"?>'
        b'<package xmlns="http://www.idpf.org/2007/opf" version="3.0" '
        b'unique-identifier="book-id"><metadata '
        b'xmlns:dc="http://purl.org/dc/elements/1.1/">'
        b'<dc:identifier id="book-id">urn:quirewise:consumer</dc:identifier>'
        b'<dc:title>External consumer fixture</dc:title><dc:language>en</dc:language>'
        b'</metadata><manifest><item id="chapter" href="chapter.xhtml" '
        b'media-type="application/xhtml+xml"/></manifest>'
        b'<spine><itemref idref="chapter"/></spine></package>'
    )
    chapter = (
        b'<?xml version="1.0" encoding="UTF-8"?>'
        b'<html xmlns="http://www.w3.org/1999/xhtml"><head><title>Sample</title>'
        b'</head><body><section><h1>Sample</h1><p>External consumer fixture.</p>'
        b'</section></body></html>'
    )
    write_zip(
        FIXTURES / "sample.epub",
        [
            ("mimetype", b"application/epub+zip"),
            ("META-INF/container.xml", container_xml),
            ("EPUB/package.opf", package_opf),
            ("EPUB/chapter.xhtml", chapter),
        ],
        epub=True,
    )

    wav_buffer = io.BytesIO()
    with wave.open(wav_buffer, "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(1)
        wav.setframerate(8000)
        wav.writeframes(b"\x80")
    (FIXTURES / "tone.wav").write_bytes(wav_buffer.getvalue())
    (FIXTURES / "pixel.bmp").write_bytes(image_bmp)
    (FIXTURES / "one-page.pdf").write_bytes(pdf_one_page())
    (FIXTURES / "synthetic-ceb.ceb").write_bytes(ceb_synthetic())
    (FIXTURES / "synthetic-ceb-invalid-rsa.ceb").write_bytes(
        ceb_synthetic(invalid_rsa_key=True)
    )
    print(f"Generated fixtures in {FIXTURES}")


if __name__ == "__main__":
    main()
