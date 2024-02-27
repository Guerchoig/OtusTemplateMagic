// Realises a universal print_ip function,
// which interprets integers, vectors, lists, strings and tuples
// as sequences of dot-separated groups, while cout-ing them
#pragma once
#include <type_traits>
#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include <list>
#include <initializer_list>
#include <tuple>

/// @brief Types to tell a string argument
/// @tparam T
template <typename T>
struct is_string : std::false_type
{
};

template <>
struct is_string<std::string> : std::true_type
{
};

template <typename T>
bool constexpr is_string_v = is_string<T>::value;

// Types to tell a container argument, include strings
template <typename...>
using void_t = void;

template <typename T, typename = void_t<>>
struct is_container : std::false_type
{
};

template <typename T>
struct is_container<T, void_t<typename T::iterator>> : std::true_type
{
};

template <typename T>
bool constexpr is_container_v = is_container<T>::value;

// Handy types for use in print_ip SFINAE templates
template <typename T>
using IsIntegral = std::enable_if_t<std::is_integral_v<T>, bool>;

template <typename T>
using IsContainer = std::enable_if_t<is_container_v<T>, bool>;

template <typename T>
using NotIsString = std::enable_if_t<!is_string_v<T>, bool>;

template <typename T>
using IsString = std::enable_if_t<is_string_v<T>, bool>;

template <typename T>
using IsTuple = std::tuple_element_t<0, T>;

// The print_ip SFINAE templates

// For any integrals
template <typename T, IsIntegral<T> = true>
void print_ip(T &&num)
{
    uint8_t *p = reinterpret_cast<uint8_t *>(&num);
    for (int i = sizeof(T) - 1; i >= 0; --i)
    {
        std::cout << +p[i] << (i == 0 ? "\n" : ".");
    }
}

// For containers, except strings
template <typename T, IsContainer<T> = true, NotIsString<T> = true>
void print_ip(const T &cont)
{
    std::size_t i = 1;
    for (auto n = cont.begin(); n != cont.end(); ++n, ++i)
        std::cout << *n << (i == cont.size() ? "\n" : ".");
}

// For strings
template <typename T, IsString<T> = true>
void print_ip(const T &str)
{
    std::cout << str << "\n";
}

// For tuples (https://www.cppstories.com/2022/tuple-iteration-basics/)
template <typename T, IsTuple<T> = true>
void print_ip(const T &tp)
{
    std::apply(
        [](const auto &first, const auto &...restArgs)
        {
            auto printElem = [](const auto &el)
            {
                std::cout << "." << el;
            };
            std::cout << first;
            (printElem(restArgs), ...);
        },
        tp);
    std::cout << "\n";
}
