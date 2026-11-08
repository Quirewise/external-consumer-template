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
    {
        static const uint8_t md5_expected[16] = {
            0x90u, 0x01u, 0x50u, 0x98u, 0x3cu, 0xd2u, 0x4fu, 0xb0u,
            0xd6u, 0x96u, 0x3fu, 0x7du, 0x28u, 0xe1u, 0x7fu, 0x72u
        };
        static const uint8_t aes128_key[16] = {
            0x2bu, 0x7eu, 0x15u, 0x16u, 0x28u, 0xaeu, 0xd2u, 0xa6u,
            0xabu, 0xf7u, 0x15u, 0x88u, 0x09u, 0xcfu, 0x4fu, 0x3cu
        };
        static const uint8_t aes128_cipher[16] = {
            0x3au, 0xd7u, 0x7bu, 0xb4u, 0x0du, 0x7au, 0x36u, 0x60u,
            0xa8u, 0x9eu, 0xcau, 0xf3u, 0x24u, 0x66u, 0xefu, 0x97u
        };
        static const uint8_t aes128_plain[16] = {
            0x6bu, 0xc1u, 0xbeu, 0xe2u, 0x2eu, 0x40u, 0x9fu, 0x96u,
            0xe9u, 0x3du, 0x7eu, 0x11u, 0x73u, 0x93u, 0x17u, 0x2au
        };
        static const uint8_t aes256_key[32] = {
            0x60u, 0x3du, 0xebu, 0x10u, 0x15u, 0xcau, 0x71u, 0xbeu,
            0x2bu, 0x73u, 0xaeu, 0xf0u, 0x85u, 0x7du, 0x77u, 0x81u,
            0x1fu, 0x35u, 0x2cu, 0x07u, 0x3bu, 0x61u, 0x08u, 0xd7u,
            0x2du, 0x98u, 0x10u, 0xa3u, 0x09u, 0x14u, 0xdfu, 0xf4u
        };
        static const uint8_t aes256_cipher[16] = {
            0xf3u, 0xeeu, 0xd1u, 0xbdu, 0xb5u, 0xd2u, 0xa0u, 0x3cu,
            0x06u, 0x4bu, 0x5au, 0x7eu, 0x3du, 0xb1u, 0x81u, 0xf8u
        };
        uint8_t digest[16];
        uint8_t block[16];
        QwCryptoPdgDeslikeSchedule schedule;
        uint8_t rsa_cipher[64] = {0};
        uint8_t rsa_exponent[64] = {0};
        uint8_t rsa_modulus[64] = {0};
        uint8_t rsa_output[64] = {0};
        uint8_t keystream[8];
        if (qw_crypto_md5_digest("abc", 3u, digest) != QW_CRYPTO_OK) return 4;
        if (memcmp(digest, md5_expected, sizeof(digest)) != 0) return 5;
        memcpy(block, aes128_cipher, sizeof(block));
        if (qw_crypto_aes128_ecb_decrypt(aes128_key, block, sizeof(block)) !=
            QW_CRYPTO_OK || memcmp(block, aes128_plain, sizeof(block)) != 0) return 6;
        memcpy(block, aes256_cipher, sizeof(block));
        if (qw_crypto_aes256_ecb_decrypt(aes256_key, block, sizeof(block)) !=
            QW_CRYPTO_OK || memcmp(block, aes128_plain, sizeof(block)) != 0) return 7;
        {
            static const uint8_t sha256_expected[32] = {
                0xbau, 0x78u, 0x16u, 0xbfu, 0x8fu, 0x01u, 0xcfu, 0xeau,
                0x41u, 0x41u, 0x40u, 0xdeu, 0x5du, 0xaeu, 0x22u, 0x23u,
                0xb0u, 0x03u, 0x61u, 0xa3u, 0x96u, 0x17u, 0x7au, 0x9cu,
                0xb4u, 0x10u, 0xffu, 0x61u, 0xf2u, 0x00u, 0x15u, 0xadu,
            };
            static const uint8_t sha384_expected[48] = {
                0xcbu, 0x00u, 0x75u, 0x3fu, 0x45u, 0xa3u, 0x5eu, 0x8bu,
                0xb5u, 0xa0u, 0x3du, 0x69u, 0x9au, 0xc6u, 0x50u, 0x07u,
                0x27u, 0x2cu, 0x32u, 0xabu, 0x0eu, 0xdeu, 0xd1u, 0x63u,
                0x1au, 0x8bu, 0x60u, 0x5au, 0x43u, 0xffu, 0x5bu, 0xedu,
                0x80u, 0x86u, 0x07u, 0x2bu, 0xa1u, 0xe7u, 0xccu, 0x23u,
                0x58u, 0xbau, 0xecu, 0xa1u, 0x34u, 0xc8u, 0x25u, 0xa7u,
            };
            static const uint8_t sha512_expected[64] = {
                0xddu, 0xafu, 0x35u, 0xa1u, 0x93u, 0x61u, 0x7au, 0xbau,
                0xccu, 0x41u, 0x73u, 0x49u, 0xaeu, 0x20u, 0x41u, 0x31u,
                0x12u, 0xe6u, 0xfau, 0x4eu, 0x89u, 0xa9u, 0x7eu, 0xa2u,
                0x0au, 0x9eu, 0xeeu, 0xe6u, 0x4bu, 0x55u, 0xd3u, 0x9au,
                0x21u, 0x92u, 0x99u, 0x2au, 0x27u, 0x4fu, 0xc1u, 0xa8u,
                0x36u, 0xbau, 0x3cu, 0x23u, 0xa3u, 0xfeu, 0xebu, 0xbdu,
                0x45u, 0x4du, 0x44u, 0x23u, 0x64u, 0x3cu, 0xe8u, 0x0eu,
                0x2au, 0x9au, 0xc9u, 0x4fu, 0xa5u, 0x4cu, 0xa4u, 0x9fu,
            };
            uint8_t digest32[32];
            uint8_t digest48[48];
            uint8_t digest64[64];
            if (qw_crypto_sha256((const uint8_t *)"abc", 3u, digest32) !=
                    QW_CRYPTO_OK ||
                memcmp(digest32, sha256_expected, sizeof(digest32)) != 0) return 17;
            if (qw_crypto_sha384((const uint8_t *)"abc", 3u, digest48) !=
                    QW_CRYPTO_OK ||
                memcmp(digest48, sha384_expected, sizeof(digest48)) != 0) return 18;
            if (qw_crypto_sha512((const uint8_t *)"abc", 3u, digest64) !=
                    QW_CRYPTO_OK ||
                memcmp(digest64, sha512_expected, sizeof(digest64)) != 0) return 19;
            memcpy(block, aes128_plain, sizeof(block));
            if (qw_crypto_aes128_ecb_encrypt(aes128_key, block, sizeof(block)) !=
                    QW_CRYPTO_OK ||
                memcmp(block, aes128_cipher, sizeof(block)) != 0) return 20;
        }
        memset(block, 0, sizeof(block));
        if (qw_crypto_tea64_ecb_decrypt(aes128_key, block, sizeof(block)) !=
            QW_CRYPTO_OK) return 8;
        if (qw_crypto_pdg_tea128_ecb_decrypt(aes128_key, block, sizeof(block)) !=
            QW_CRYPTO_OK) return 9;
        if (qw_crypto_pdg_deslike_schedule_init(aes128_key, &schedule) !=
            QW_CRYPTO_OK) return 10;
        if (qw_crypto_pdg_deslike_transform16(block, &schedule) != QW_CRYPTO_OK)
            return 11;
        qw_crypto_pdg_deslike_schedule_clear(&schedule);
        memset(block, 0, sizeof(block));
        if (qw_crypto_blowfish_ecb_decrypt(aes128_key, sizeof(aes128_key), block,
                                           sizeof(block)) != QW_CRYPTO_OK) return 12;
        if (qw_crypto_3des_ofb64_crypt(key, iv, block, sizeof(block)) !=
            QW_CRYPTO_OK) return 13;
        if (qw_crypto_founder_ceb_keystream(aes128_key, sizeof(aes128_key),
                                            keystream, sizeof(keystream)) !=
            QW_CRYPTO_OK) return 14;
        rsa_cipher[0] = 2u;
        rsa_exponent[0] = 1u;
        rsa_modulus[0] = 3u;
        if (qw_crypto_rsa_mod_exp_512_le(rsa_cipher, rsa_exponent, rsa_modulus,
                                         rsa_output) != QW_CRYPTO_OK) return 15;
        if (rsa_output[0] != 2u) return 16;
    }
    return 0;
}
