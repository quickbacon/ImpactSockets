/**
 * Created by TekuConcept on January 30, 2019
 */

#include <gtest/gtest.h>
#include <rfc/md5.h>

using namespace impact;

#include <iostream>
TEST(test_md5, digest)
{
    EXPECT_EQ(md5::digest(""), std::string(
        "\xd4\x1d\x8c\xd9"
        "\x8f\x00\xb2\x04"
        "\xe9\x80\x09\x98"
        "\xec\xf8\x42\x7e", 16)
    );
    
    EXPECT_EQ(md5::digest("a"), std::string(
        "\x0c\xc1\x75\xb9"
        "\xc0\xf1\xb6\xa8"
        "\x31\xc3\x99\xe2"
        "\x69\x77\x26\x61", 16)
    );
    
    EXPECT_EQ(md5::digest("abc"), std::string(
        "\x90\x01\x50\x98"
        "\x3c\xd2\x4f\xb0"
        "\xd6\x96\x3f\x7d"
        "\x28\xe1\x7f\x72", 16)
    );
    
    EXPECT_EQ(md5::digest("message digest"), std::string(
        "\xf9\x6b\x69\x7d"
        "\x7c\xb7\x93\x8d"
        "\x52\x5a\x2f\x31"
        "\xaa\xf1\x61\xd0", 16)
    );
    
    EXPECT_EQ(md5::digest("abcdefghijklmnopqrstuvwxyz"), std::string(
        "\xc3\xfc\xd3\xd7"
        "\x61\x92\xe4\x00"
        "\x7d\xfb\x49\x6c"
        "\xca\x67\xe1\x3b", 16)
    );
    
    EXPECT_EQ(md5::digest("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz0123456789"), std::string(
        "\xd1\x74\xab\x98"
        "\xd2\x77\xd9\xf5"
        "\xa5\x61\x1c\x2c"
        "\x9f\x41\x9d\x9f", 16)
    );
    
    EXPECT_EQ(md5::digest("12345678901234567890"
        "123456789012345678901234567890"
        "123456789012345678901234567890"), std::string(
        "\x57\xed\xf4\xa2"
        "\x2b\xe3\xc9\x55"
        "\xac\x49\xda\x2e"
        "\x21\x07\xb6\x7a", 16)
    );
}
