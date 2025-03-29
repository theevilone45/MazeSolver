#ifndef INCLUDE_MAZE_MONADS_IDENTITY_HPP
#define INCLUDE_MAZE_MONADS_IDENTITY_HPP

#include <type_traits>
#include <utility>

namespace monads {

template <typename ValueType> class Identity {
private:
    ValueType mValue;

public:
    Identity() : mValue({}) {}
    explicit Identity(ValueType &&v) : mValue(std::move(v)) {}
    explicit Identity(const ValueType &v) : mValue(v) {}

    const ValueType &get() const { return mValue; }
    ValueType &get() { return mValue; }

    // clang-format off

    // use on r-value
    template <typename Functor, typename... Args>
    auto bind(Functor &&fun, Args &&...args)
     -> decltype(fun(std::move(mValue), std::forward<Args>(args)...)) {
        return fun(std::move(mValue), std::forward<Args>(args)...);
    }

    // use on l-value
    template <typename Functor, typename... Args>
    auto bind(Functor &&fun, Args &&...args)
     -> decltype(fun(mValue, std::forward<Args>(args)...)) {
        return fun(mValue, std::forward<Args>(args)...);
    }

    //clang-format on
};

template <typename U> Identity(U &&) -> Identity<std::decay_t<U>>;

} // namespace monads

#endif
