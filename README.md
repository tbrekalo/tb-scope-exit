# tb-scope-exit

![workflow](https://github.com/tbrekalo/tb-scope-exit/actions/workflows/ci.yml/badge.svg?branch=master)

C++20 scope guard implementation. Why another one? Life is too short not to have fun with cute side projects. :)

# Requirements

- a c++20 cimpiler supporting [lambdas in unevaluated context](https://wg21.link/P0315R4) and Concepts
  - gcc:10 or higher
  - clang:17 or higher

## Implementation fun facts

- c++20 concepts
- taking advantage of [lambdas in unevaluated context](https://wg21.link/P0315R4) allows for some new TMP trickery

```c++
using exit_policy = predicate_policy<
    decltype([]([[maybe_unused]] int n_past_exceptions) noexcept
             -> bool { return true; }),
    is_nothrow_exit_function>;
```

## Usage

Just copy & paste the header file `include/tb/scope_exit.h` into your project or use cmake fetch content.

```cmake
include(FetchContent)
FetchContent_Declare(
  tb_scope_exit
  GIT_REPOSITORY https://github.com/tbrekalo/tb-scope-exit
  GIT_TAG master)
FetchContent_MakeAvailable(tb_scope_exit)


target_link_libraries(your-target PRIVATE tb_scope_exit)
```

## Example

```c++
#include <semaphore>
#include <thread>
#include <vector>

#include "tb/scope_guard.h"

int main() {
    std::counting_semaphore<1> semaphore(1);
    std::uint32_t count{0};

    std::vector<std::jthread> jthreads;
    jthreads.reserve(3);

    for (auto i = 0U; i < 3; ++i) {
        jthreads.emplace_back(
            [&semaphore, &count](std::uint32_t idx) -> void {
                semaphore.acquire();
                auto sg = tb::make_scope_exit(
                    [&semaphore]() noexcept -> void { semaphore.release(); });

                ++count;
                if ((idx & 1) == 1) {
                    fmt::print("count: {}", count);
                } else {
                    function_throwing_an_exception();
                }
            },
            i);
    }
}
```

### References

- [n4189](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4189.pdf)
- [p0052r10](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0052r10)
