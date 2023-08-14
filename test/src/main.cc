#include "catch2/catch_test_macros.hpp"
#include "tb/scope_exit.h"

// exception class used for tests
class strong_exception : std::runtime_error {
 public:
  strong_exception() : std::runtime_error("") {}
  explicit strong_exception(std::string const& what)
      : std::runtime_error(what) {}
};

TEST_CASE("tb::scope_exit") {
  auto x = 0U;
  SECTION("tb::scope_exit nothrow") {
    REQUIRE_NOTHROW([&x]() -> void {
      auto scope_exit =
          tb::make_scope_exit([&x]() noexcept -> void { x = 42; });
      CHECK(x == 0U);
    }());
  }

  SECTION("tb::scope_exit throws") {
    REQUIRE_THROWS_AS(
        [&x]() -> void {
          auto scope_exit =
              tb::make_scope_exit([&x]() noexcept -> void { x = 42U; });
          CHECK(x == 0U);

          throw strong_exception();
        }(),
        strong_exception);
  }

  REQUIRE(x == 42U);
}

