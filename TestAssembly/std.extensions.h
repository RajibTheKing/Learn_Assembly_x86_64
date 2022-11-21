#ifndef ANG_STD_EXTENSIONS_H
#define ANG_STD_EXTENSIONS_H
#include <iostream>
#include <tuple>
#include <variant>

/// TODO could be superseded with \see ANG::logging::print::into(ostream&, obj)

/// \see std::ostream extension for \see std::variant<T...>
template<typename _CharT, typename _Traits, typename T0, typename ... Ts>
std::basic_ostream<_CharT, _Traits>& operator<< (std::basic_ostream<_CharT, _Traits>& os, const std::variant<T0, Ts...>& v)
{
    std::visit([&os](auto&& arg){ os << arg; }, v);
    return os;
}

/// \see std::ostream extension for \see std::tuple<T...>
template<typename _CharT, typename _Traits, typename TupType, size_t... I>
void ___tuplePrinter(std::basic_ostream<_CharT, _Traits>& os, const TupType& _tup, std::index_sequence<I...>)
{
    os << "<";
    (..., (os << (I == 0? "" : ", ") << std::get<I>(_tup)));
    os << ">";
}

template<typename _CharT, typename _Traits, typename T0, typename ... Ts>
std::basic_ostream<_CharT, _Traits>& operator<< (std::basic_ostream<_CharT, _Traits>& os, const std::tuple<T0, Ts...>& t)
{
    ___tuplePrinter(os, t, std::make_index_sequence<1+sizeof...(Ts)>());
    return os;
}

namespace std
{
    template<typename T>        struct is_variant : false_type{};
    template<typename... Args>  struct is_variant<variant<Args...>> : true_type{};
    template<typename T> inline constexpr auto is_variant_v = is_variant<T>::value;
  //template<typename T> concept variant = std::is_variant_v<T>;

    template<typename T>        struct is_tuple : false_type{};
    template<typename... Args>  struct is_tuple<tuple<Args...>> : true_type{};
    template<typename T> inline constexpr auto is_tuple_v = is_tuple<T>::value;
  //template<typename T> concept tuple = std::is_tuple_v<T>;
}

#endif
