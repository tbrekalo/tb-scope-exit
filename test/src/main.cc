#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

// exception class used for tests
class strong_exception : std::runtime_error {
 public:
  strong_exception() : std::runtime_error("") {}
  explicit strong_exception(std::string const& what)
      : std::runtime_error(what) {}
};

TEST_CASE("tb::scope_exit active", "[scope_exit][active]") {
  auto x = 0U;
  SECTION("nothrow") {
    REQUIRE_NOTHROW([&x]() -> void {
      auto scope_exit =
          tb::make_scope_exit([&x]() noexcept -> void { x = 42; });
      CHECK(x == 0U);
    }());
  }

  SECTION("throws") {
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

TEST_CASE("tb::scope_exit released", "[scope_exit][released]") {
  auto x = 0U;
  SECTION("nothrow") {
    REQUIRE_NOTHROW([&x]() -> void {
      auto scope_exit =
          tb::make_scope_exit([&x]() noexcept -> void { x = 42; });
      scope_exit.release();

      CHECK(x == 0U);
    }());
  }

  SECTION("throws") {
    REQUIRE_THROWS_AS(
        [&x]() -> void {
          auto scope_exit =
              tb::make_scope_exit([&x]() noexcept -> void { x = 42U; });
          scope_exit.release();

          CHECK(x == 0U);
          throw strong_exception();
        }(),
        strong_exception);
  }

  REQUIRE(x == 0U);
}
