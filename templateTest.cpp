#include <iostream>

using namespace std;

namespace dd {
template <int n> struct FIB {
  enum { value = n + FIB<n - 1>::value };
};
template <> struct FIB<0> {
  enum { value = 1 };
};

template <typename... Args> auto getSum(Args &&... arg) {
  return (arg + ... + 0);
}

template <auto base, auto n> struct mmm {
  static constexpr auto value = base * mmm<base, n - 1>::value;
};
template <auto base> struct mmm<base, 0> {
  static constexpr auto value = base;
};

} // namespace dd

int main() {

  cout << dd::mmm<10, 5>::value << endl;

  cin.get();
  return 0;
}
