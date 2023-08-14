#ifndef TB_SCOPE_EXIT_H_
#define TB_SCOPE_EXIT_H_

#include <concepts>
#include <exception>
#include <type_traits>
#include <utility>

namespace tb::detail {

class exit_policy {};

template <class ExitFn, class ExitPolicy = exit_policy>
  requires(std::is_nothrow_invocable_r_v<void, ExitFn>)
class scope_guard {
  ExitFn exit_fn_;
  bool enabled_;

 public:
  template <class Fn>
  explicit scope_guard(Fn&& fn) noexcept
      : exit_fn_(std::forward<Fn>(fn)), enabled_(true) {}

  scope_guard(scope_guard const& that) = delete;
  auto operator=(scope_guard const& that) -> scope_guard& = delete;

  scope_guard(scope_guard&& that) noexcept
      : exit_fn_(std::move(that.exit_fn_)),
        enabled_(std::exchange(that.enabled_), false) {}
  auto operator=(scope_guard&& that) = delete;

  ~scope_guard() {
    if (enabled_) {
      exit_fn_();
    }
  }
};

}  // namespace tb::detail

namespace tb {

template <class Fn>
using scope_exit = detail::scope_guard<Fn>;

template <class ExitFn>
auto make_scope_exit(ExitFn&& exit_fn) {
  return scope_exit<std::remove_reference_t<ExitFn>>(
      std::forward<ExitFn>(exit_fn));
}

}  // namespace tb

#endif /* TB_SCOPE_EXIT_H_ */
