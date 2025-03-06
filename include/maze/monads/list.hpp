#ifndef INCLUDE_MAZE_MONADS_LIST_HPP
#define INCLUDE_MAZE_MONADS_LIST_HPP

#include <concepts>
#include <ranges>
#include <type_traits>
#include <utility>

namespace monads {

template <typename T>
concept Iterable = std::ranges::range<T>;

template <Iterable Container> class List {
private:
    Container mContainer;

public:
    template <typename C>
        requires std::constructible_from<Container, C>
    explicit List(C &&c) : mContainer(std::forward<C>(c)) {}

    const Container &get() const { return mContainer; }

    template<typename Functor> auto
    bind(Functor &&fun) -> List<std::decay_t<decltype(fun(
                            std::declval<std::decay_t<decltype(*std::begin(
                                std::declval<Container &>()))>>()))>>
    {
        using ElementType = std::decay_t<decltype(*std::begin(std::declval<Container&>()))>;
        using ReturnContainer = std::decay_t<decltype(fun(std::declval<ElementType>()))>;

        ReturnContainer result;
        for(const auto& elem : mContainer) {
            ReturnContainer intermediate = fun(elem);
            result.insert(std::end(result), std::begin(intermediate), std::end(intermediate));
        }
        return List<ReturnContainer>(std::move(result));
    }
};

template<typename U>
List(U&&) -> List<std::decay_t<U>>;

}; // namespace monads

#endif
