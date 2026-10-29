#include <quirewise/crypto.h>

#include <string.h>

int main(void)
{
    static const uint8_t key[24] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x89, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67
    };
    static const uint8_t iv[8] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };
    static const uint8_t ciphertext[8] = {
        0x2a, 0x52, 0x05, 0x89, 0xed, 0xc5, 0xa6, 0x09
    };
    static const uint8_t expected[8] = {
        'C', 'E', 'B', 't', 'e', 's', 't', '!'
    };
    uint8_t buffer[sizeof(ciphertext)];

    if (qw_crypto_abi_version() != UINT32_C(0x00010000)) return 1;
    memcpy(buffer, ciphertext, sizeof(buffer));
    if (qw_crypto_3des_cfb64_decrypt(key, iv, buffer, sizeof(buffer)) !=
        QW_CRYPTO_OK) return 2;
    if (memcmp(buffer, expected, sizeof(buffer)) != 0) return 3;
    qw_crypto_secure_zero(buffer, sizeof(buffer));
    return 0;
}
