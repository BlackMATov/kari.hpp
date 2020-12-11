# kari.hpp

> Experimental library for currying in C++17

[![travis][badge.travis]][travis]
[![appveyor][badge.appveyor]][appveyor]
[![codecov][badge.codecov]][codecov]
[![language][badge.language]][language]
[![license][badge.license]][license]
[![paypal][badge.paypal]][paypal]

[badge.travis]: https://img.shields.io/travis/BlackMATov/kari.hpp/main.svg?logo=travis
[badge.appveyor]: https://img.shields.io/appveyor/ci/BlackMATov/kari-hpp/main.svg?logo=appveyor
[badge.codecov]: https://img.shields.io/codecov/c/github/BlackMATov/kari.hpp/main.svg?logo=codecov
[badge.language]: https://img.shields.io/badge/language-C%2B%2B17-yellow.svg
[badge.license]: https://img.shields.io/badge/license-MIT-blue.svg
[badge.paypal]: https://img.shields.io/badge/donate-PayPal-orange.svg?logo=paypal&colorA=00457C

[travis]: https://travis-ci.org/BlackMATov/kari.hpp
[appveyor]: https://ci.appveyor.com/project/BlackMATov/kari-hpp
[codecov]: https://codecov.io/gh/BlackMATov/kari.hpp
[language]: https://en.wikipedia.org/wiki/C%2B%2B17
[license]: https://en.wikipedia.org/wiki/MIT_License
[paypal]: https://www.paypal.me/matov

[kari]: https://github.com/BlackMATov/kari.hpp

## Requirements

- [gcc](https://www.gnu.org/software/gcc/) **>= 7**
- [clang](https://clang.llvm.org/) **>= 5.0**
- [msvc](https://visualstudio.microsoft.com/) **>= 2017**

## Installation

[kari.hpp][kari] is a header-only library. All you need to do is copy the headers files from `headers` directory into your project and include them:


```cpp
#include "kari.hpp/kari.hpp"
```

Also, you can add the root repository directory to your [cmake](https://cmake.org) project:

```cmake
add_subdirectory(external/kari.hpp)
target_link_libraries(your_project_target kari.hpp)
```

## Examples

### Basic currying

```cpp
using namespace kari_hpp;

auto foo = [](int v1, int v2, int v3) {
  return v1 + v2 + v3;
};

auto c0 = curry(foo); // currying of `foo` function
auto c1 = c0(10);     // apply to first argument
auto c2 = c1(20);     // apply to second argument
auto rr = c2(12);     // apply to third argument and call the `foo` function

// output: 42
std::cout << rr << std::endl;
```

### Partial application of curried functions

```cpp
using namespace kari_hpp;

curry_t c0 = [](int v1, int v2, int v3, int v4) {
  return v1 + v2 + v3 + v4;
};

auto c1 = c0(15, 20); // partial application of two arguments
auto rr = c1(2, 5);   // partial application and call `(15,20,2,5)`

// output: 42
std::cout << rr << std::endl;
```

### Calling nested curried functions

```cpp
using namespace kari_hpp;

curry_t boo = [](int a, int b) {
  return a + b;
};

curry_t foo = [boo](int a, int b) {
  return boo(a + b);
};

auto c0 = foo(38,3,1);
auto c1 = foo(38,3)(1);
auto c2 = foo(38)(3,1);

// output: 42,42,42
std::cout << c0 << "," << c1 << "," << c2 << std::endl;
```

### Binding member functions and member objects

```cpp
using namespace kari_hpp;

struct foo_t {
  int v = 40;
  int add_v(int add) {
    v += add;
    return v;
  }
} foo;

auto c0 = curry(&foo_t::add_v);
auto c1 = curry(&foo_t::v);

auto r0 = c0(std::ref(foo))(2);
auto r1 = c1(std::cref(foo));

// output: 42,42
std::cout << r0 << "," << r1 << std::endl;
```

## API

```cpp
namespace kari_hpp {
  template < typename F, typename... Args >
  constexpr auto curry(F&& f, Args&&... args);

  template < typename F >
  struct is_curried;

  template < typename F >
  inline constexpr bool is_curried_v = is_curried<F>::value;

  template < typename F, typename... Args >
  class curry_t {
    constexpr curry_t(F f);

    template < typename... As >
    constexpr auto operator()(As&&... as) const;
  };
}
```

---

### `kari_hpp::curry(F&& f, Args&&... args)`

Returns a curried function **`f`** or copy the function result with **`args`** arguments.

---

### `kari_hpp::is_curried<F>, kari_hpp::is_curried_v<F>`

Checks whether F is a curried function type.

```cpp
using namespace kari_hpp;

constexpr curry_t c = [](int a, int b){
  return a + b;
};

// output: is `l` curried? no
std::cout
  << "is `l` curried? "
  << (is_curried<decltype(l)>::value ? "yes" : "no")
  << std::endl;

// output: is `c` curried? yes
std::cout
  << "is `c` curried? "
  << (is_curried_v<decltype(c)> ? "yes" : "no")
  << std::endl;
```

---

### `kari_hpp::curry_t::operator()(As&&... as)`

Calling operator of curried function for partial application or full application. Returns a new curried function with added new arguments or copy of the function result.

```cpp
using namespace kari_hpp;

curry_t c0 = [](int a, int b, int c, int d) {
  return a + b + c + d;
};

auto c1 = c0(15, 20); // partial application
auto rr = c2(2, 5);   // function call - foo(15,20,2,5)

// output: 42
std::cout << rr << std::endl;
```

---

## Free syntactic sugar

### Section of operators

```cpp
using namespace kari_hpp::ext::underscore;
std::vector v{1, 2, 3, 4};

// result: 10
std::accumulate(v.begin(), v.end(), 0, _ + _);

// v = {2, 4, 6, 8}
std::transform(v.begin(), v.end(), v.begin(), _ * 2);

// v = {-2, -4, -6, -8}
std::transform(v.begin(), v.end(), v.begin(), - _);
```

### Function composition

#### Pipe operator

```cpp
using namespace kari_hpp::ext;
using namespace kari_hpp::ext::underscore;

auto r0 = (_*2) | (_+2) | 4; // (4 * 2) + 2 = 10
auto r1 = 4 | (_*2) | (_+2); // (4 * 2 + 2) = 10

// output: 10,10
std::cout << r0, << "," << r1 << std::endl;
```

#### Compose operator

```cpp
using namespace kari_hpp::ext;
using namespace kari_hpp::ext::underscore;

auto r0 = (_*2) * (_+2) * 4; // (4 + 2) * 2 = 12
auto r1 = 4 * (_*2) * (_+2); // (4 * 2 + 2) = 10

// output: 12,10
std::cout << r0, << "," << r1 << std::endl;
```

### Point-free style for Haskell maniacs

```cpp
using namespace kari_hpp::ext;
using namespace kari_hpp::ext::underscore;

// (. (+2)) (*2) $ 10 == 24 // haskell analog
auto r0 = (_*(_+2))(_*2) * 10;

// ((+2) .) (*2) $ 10 == 22 // haskell analog
auto r1 = ((_+2)*_)(_*2) * 10;

// output: 24,22
std::cout << r0, << "," << r1 << std::endl;
```

## [License (MIT)](./LICENSE.md)
