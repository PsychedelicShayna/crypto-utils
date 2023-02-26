#include <gtest/gtest.h>

#include "source/include/obsidian/encoding.hpp"
#include "source/include/obsidian/random.hpp"
using namespace obsidian;

const std::string& QBF_TEST_STRING =
    "The quick brown fox jumps over the lazy dog!";

const std::vector<uint8_t> QBF_TEST_BYTES(QBF_TEST_STRING.begin(),
                                          QBF_TEST_STRING.end());

const std::string& QBF_KNOWN_ENCODE_B64 =
    "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZyE=";

const std::string& QBF_KNOWN_ENCODE_B16 =
    "54686520717569636B2062726F776E20666F78206A756D7073206F76657220746865206C61"
    "7A7920646F6721";

TEST(Encodings, Base64KnownEncode)
{
    ASSERT_EQ(encoding::b64_encode(QBF_TEST_BYTES), QBF_KNOWN_ENCODE_B64);
}

TEST(Encodings, Base64KnownDecode)
{
    ASSERT_EQ(encoding::b64_decode(QBF_KNOWN_ENCODE_B64), QBF_TEST_BYTES);
}

TEST(Encodings, Base64IsReversible1024RandomIterations)
{
    for(uint8_t i = 0; i < 0xFF; ++i) {
        const auto& encoded = encoding::b16_encode({i});
        const auto& decoded = encoding::b16_decode(encoded);

        ASSERT_EQ(decoded, std::vector<uint8_t> {i});
    }
}

TEST(Encodings, Base16KnownEncode)
{
    ASSERT_EQ(encoding::b16_encode(QBF_TEST_BYTES), QBF_KNOWN_ENCODE_B16);
}

TEST(Encodings, Base16KnownDecode)
{
    ASSERT_EQ(encoding::b16_decode(QBF_KNOWN_ENCODE_B16), QBF_TEST_BYTES);
}

TEST(Encodings, Base16IsReversible1024RandomIterations)
{
    for(int i = 0; i < 5; ++i) {
        for(size_t i2 = 0; i2 < 1024; ++i2) {
            const auto& sample  = random::bytes(i2);
            const auto& encoded = encoding::b16_encode(sample);
            const auto& decoded = encoding::b16_decode(encoded);
            ASSERT_EQ(sample, decoded);
        }
    }
}

bool pkcs7_validator(const std::vector<uint8_t>& bytes,
                     const uint32_t&             correct_difference)
{
    const uint8_t& last_byte = *bytes.cend();

    if(last_byte != correct_difference) {
        return false;
    }

    bool valid_padding = std::all_of(
        bytes.cend() - *bytes.cend(),
        bytes.cend(),
        [&](const auto& byte) -> bool { return byte == last_byte; });

    if(!valid_padding) {
        return false;
    }

    return true;
}

TEST(Encodings, Pkcs7KnownPad)
{
    const std::vector<uint8_t>& sample_15b {
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
    };

    const auto& padded = encoding::apply_pkcs7_padding(sample_15b, 16);

    ASSERT_EQ(pkcs7_validator(sample_15b, 1), true);
}
