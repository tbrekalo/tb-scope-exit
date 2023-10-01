#pragma once

#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

namespace tb {

namespace detail {

template <class T>
concept NothrowMoveCopyDtor = std::is_nothrow_copy_constructible_v<T> &&
                              std::is_nothrow_move_constructible_v<T> &&
                              std::is_nothrow_destructible_v<T>;
}

template <class T>
struct is_exit_function {
  static constexpr auto value =
      detail::NothrowMoveCopyDtor<T> && std::is_invocable_r_v<void, T>;
};

template <class T>
struct is_nothrow_exit_function {
  static constexpr auto value =
      detail::NothrowMoveCopyDtor<T> && std::is_nothrow_invocable_r_v<void, T>;
};

namespace detail {

/* clang-format off */
template <class T>
concept ScopeGuardExitPolicy = requires(T t, T const c) {
  { t.release() } noexcept -> std::same_as<void>;
  { c.should_execute() } noexcept -> std::same_as<bool>;
};
/* clang-format on */

template <template <class T> class ExitFnTraitPredicate>
class exit_policy_base {
  bool enabled_;

 public:
  template <class V>
  struct exit_function_traits {
    static constexpr auto is_valid = ExitFnTraitPredicate<V>::value;
    static constexpr auto is_noexcept = std::is_nothrow_invocable_r_v<void, V>;
  };

  explicit exit_policy_base(bool enabled) noexcept : enabled_(enabled) {}

  void release() noexcept { enabled_ = false; }

  bool should_execute() const noexcept { return enabled_; }
};

template <class Predicate, template <class T> class ExitFunctionTraits>
  requires(std::is_default_constructible_v<Predicate>,
           std::is_nothrow_invocable_r_v<bool, Predicate, int>)
class predicate_policy : public exit_policy_base<ExitFunctionTraits> {
  using base_t = exit_policy_base<ExitFunctionTraits>;

  int n_past_exceptions_;
  [[no_unique_address]] Predicate predicate_;

 public:
  explicit predicate_policy(bool enabled) noexcept
      : base_t(enabled), n_past_exceptions_(std::uncaught_exceptions()) {}

  bool should_execute() const noexcept {
    return base_t::should_execute() && predicate_(n_past_exceptions_);
  }
};

using exit_policy = predicate_policy<
    decltype([]([[maybe_unused]] int n_past_exceptions) noexcept
             -> bool { return true; }),
    is_nothrow_exit_function>;

using fail_policy =
    predicate_policy<decltype([](int n_past_exceptions) noexcept -> bool {
                       return n_past_exceptions < std::uncaught_exceptions();
                     }),
                     is_nothrow_exit_function>;

using success_policy =
    predicate_policy<decltype([](int n_past_exceptions) noexcept -> bool {
                       return n_past_exceptions >= std::uncaught_exceptions();
                     }),
                     is_exit_function>;

template <class ExitFn, ScopeGuardExitPolicy ExitPolicy>
class scope_guard {
  using exit_function_traits =
      typename ExitPolicy::template exit_function_traits<ExitFn>;

  ExitFn exit_fn_;
  ExitPolicy policy_;

  static_assert(exit_function_traits::is_valid);

 public:
  template <class T>
  explicit scope_guard(T&& callback) noexcept
    requires(std::is_same_v<std::remove_reference_t<T>, ExitFn>)
  : exit_fn_(std::forward<T>(callback)), policy_(true) {}

  scope_guard(const scope_guard& that) = delete;
  scope_guard& operator=(const scope_guard& that) = delete;

  scope_guard(scope_guard&& that) noexcept
      : exit_fn_(std::move(that.exit_fn_)),
        policy_(that.policy_.should_execute()) {
    that.policy_.release();
  }

  scope_guard operator=(scope_guard&& that) = delete;

  void release() noexcept { policy_.release(); }

  ~scope_guard() noexcept(exit_function_traits::is_noexcept) {
    if (policy_.should_execute()) {
      exit_fn_();
    }
  }
};

}  // namespace detail

template <class ExitFn>
using scope_exit = detail::scope_guard<ExitFn, detail::exit_policy>;

template <class ExitFn>
scope_exit<ExitFn> make_scope_exit(ExitFn&& exit_fn) noexcept {
  return scope_exit<std::remove_reference_t<ExitFn>>(
      std::forward<ExitFn>(exit_fn));
}

template <class ExitFn>
using scope_fail = detail::scope_guard<ExitFn, detail::fail_policy>;

template <class ExitFn>
scope_fail<ExitFn> make_scope_fail(ExitFn exit_fn) noexcept {
  return scope_fail<std::remove_reference_t<ExitFn>>(
      std::forward<ExitFn>(exit_fn));
}

template <class ExitFn>
using scope_success = detail::scope_guard<ExitFn, detail::success_policy>;

template <class ExitFn>
scope_success<ExitFn> make_scope_success(ExitFn&& exit_fn) noexcept {
  return scope_success<std::remove_reference_t<ExitFn>>(
      std::forward<ExitFn>(exit_fn));
}

}  // namespace tb
