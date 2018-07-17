// #include <functional>
// #include <future>
// #include <iostream>
// #include <mutex>
// #include <queue>
// #include <vector>

// namespace dd {
// template <typename T> class signal final {
//   private:
//     std::function<T> task;

//   public:
//     signal(const signal &) = delete;
//     signal(signal &&other) = default;
//     signal &operator=(const signal &) = delete;
//     signal &operator=(signal &&other) = default;
//     explicit signal(T st) { task = st; }
//     ~signal() {}
//     template <typename... Args>
//     constexpr auto operator()(Args &&... args) noexcept ->
//         typename std::result_of<std::function<T>(Args &&...)>::type {
//         return task(std::forward<Args>(args)...);
//     }
// };
// }
#include <functional>
#include <vector>
namespace pp{
    template<typename T>
    class signal final {
        using FT=std::function<T>;
    private:
        std::vector<FT> slots;
    public:
        signal(const signal &) = delete;

        signal(signal &&other) = default;

        signal &operator=(const signal &) = delete;

        signal &operator=(signal &&other) = default;

        signal &operator=(FT &&t) {
            return this->connect(std::forward<FT>(t));
        }

        signal &connect(FT &&t) {
            slots.emplace_back(std::forward<FT>(t));
            return *this;
        }

        signal() {}

        ~signal() {}

        template<typename... Args>
        constexpr auto operator()(Args &&... args) noexcept ->
        void {
            for (const auto &s : slots) {
                s(std::forward<Args>(args)...);
            }
        }
    };
}
