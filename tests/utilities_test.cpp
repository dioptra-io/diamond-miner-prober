#include <arpa/inet.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <dminer/utilities.hpp>

using dminer::Utilities::format_addr;
using dminer::Utilities::parse_addr;

inline in6_addr parse_addr(const std::string& src) {
  in6_addr dst;
  parse_addr(src, dst);
  return dst;
}

TEST_CASE("parse_addr") {
  REQUIRE(format_addr(parse_addr("192.168.123.254")) == "192.168.123.254");
  REQUIRE(format_addr(parse_addr("134743044")) == "8.8.4.4");
  REQUIRE(format_addr(parse_addr("8.8.4.4")) == "8.8.4.4");
  REQUIRE(format_addr(parse_addr("::ffff:8.8.4.4")) == "8.8.4.4");
  REQUIRE(format_addr(parse_addr("2001:4860:4860::8888")) ==
          "2001:4860:4860::8888");
}