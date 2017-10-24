# kari.hpp

> Experimental library for currying in C++14

[![circleci][badge.circleci]][circleci]
[![appveyor][badge.appveyor]][appveyor]
[![license][badge.license]][license]
[![godbolt][badge.godbolt]][godbolt]
[![wandbox][badge.wandbox]][wandbox]

[badge.circleci]: https://circleci.com/gh/BlackMATov/kari.hpp.svg?style=shield
[badge.appveyor]: https://ci.appveyor.com/api/projects/status/github/blackmatov/kari.hpp?svg=true&passingText=master%20-%20OK
[badge.license]: https://img.shields.io/badge/license-MIT-blue.svg
[badge.godbolt]: https://img.shields.io/badge/try%20it-on%20godbolt-orange.svg
[badge.wandbox]: https://img.shields.io/badge/try%20it-on%20wandbox-5cb85c.svg

[circleci]: https://circleci.com/gh/BlackMATov/kari.hpp
[appveyor]: https://ci.appveyor.com/project/BlackMATov/kari-hpp-arict
[license]: https://github.com/BlackMATov/kari.hpp/blob/master/LICENSE
[godbolt]: https://godbolt.org/g/XPBgjY
[wandbox]: https://wandbox.org/permlink/l2PeuYUx2K2Yqbwj

## Examples

### Basic curring

```cpp
auto foo = [](int v1, int v2, int v3) {
    return v1 + v2 + v3;
};

auto c0 = kari::curry(foo); // curring of `foo` function
auto c1 = c0(10);           // apply to first argument
auto c2 = c1(20);           // apply to second argument
auto rr = c2(12);           // apply to third argument and call the `foo` function

// output: 42
std::cout << rr << std::endl;
```

### Partial application of curried functions

```cpp
auto foo = [](int v1, int v2, int v3, int v4) {
    return v1 + v2 + v3 + v4;
};

auto c0 = kari::curry(foo); // curring
auto c1 = c0(15, 20);       // partial application of two arguments
auto rr = c1(2, 5);         // partial application and call `foo(15,20,2,5)`

// output: 42
std::cout << rr << std::endl;
```

### Calling nested curried functions

```cpp
auto boo = [](int v1, int v2) {
  return v1 + v2;
};

auto foo = [boo](int v1, int v2) {
  return kari::curry(boo, v1 + v2);
};

auto c0 = kari::curry(foo)(38,3,1);
auto c1 = kari::curry(foo)(38,3)(1);
auto c2 = kari::curry(foo)(38)(3,1);

// output: 42,42,42
std::cout << c0 << "," << c1 << "," << c2 << std::endl;
```