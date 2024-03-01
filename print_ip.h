/// @brief This header-only library realises a universal print_ip function,
/// which outputs integers, vectors, lists, strings and tuples
/// as IP-like sequences of dot-separated groups
/// @file print_ip.h
/// @author alex-guerchoig
/// @date 2024 March 1

#pragma once
#include <type_traits>
#include <iostream>
#include <cstring>
#include <utility>
#include <vector>
#include <list>
#include <initializer_list>
#include <tuple>

/// @brief Types to SFINAE-tell a string argument from other types _
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

/// @brief Types to tell a container type, include strings
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

/// @brief Handy types for use in print_ip SFINAE templates
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

/// @brief The print_ip SFINAE templates

/// @brief For any integrals
template <typename T, IsIntegral<T> = true>
void print_ip(T &&num)
{
    uint8_t *p = reinterpret_cast<uint8_t *>(&num);
    for (int i = sizeof(T) - 1; i >= 0; --i)
    {
        std::cout << +p[i] << (i == 0 ? "\n" : ".");
    }
}

/// @brief For containers, except strings
template <typename T, IsContainer<T> = true, NotIsString<T> = true>
void print_ip(const T &cont)
{
    std::size_t i = 1;
    for (auto n = cont.begin(); n != cont.end(); ++n, ++i)
        std::cout << *n << (i == cont.size() ? "\n" : ".");
}

/// @brief For strings
template <typename T, IsString<T> = true>
void print_ip(const T &str)
{
    std::cout << str << "\n";
}

/// @brief For tuples (https://www.cppstories.com/2022/tuple-iteration-basics/)
// + mono-typed tuple
template <typename T, IsTuple<T> = true>
void print_ip(const T &tp)
{
    std::apply(
        [](const auto &first, const auto &...restArgs)
        {
            // The IsTuple<T> type of argument guarantees,
            // that all the tuple members are same type
            auto printElem = [](const IsTuple<T> &el)
            {
                std::cout << "." << el;
            };
            std::cout << first;
            (printElem(restArgs), ...);
        },
        tp);
    std::cout << "\n";
}
