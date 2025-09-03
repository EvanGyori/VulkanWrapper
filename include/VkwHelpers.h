/*
 * Provides helper stuff for the meat of the library
 *
 * In order to manipulate template parameter packs and pass them to another
 * class, the Apply alias templates can be used recursively. First specify
 * the types to use for the pack. So FunctionsTraits<Func>::ParamPack and
 * then use the Apply<T> alias where T is any class with only one template
 * parameter which is typename..., now the resulting type is T<Function parameters...>.
 * To apply other pack manipulations put ReverseTypes::Pack in place of T.
 * Then ::Apply<...> after the first Apply<...>.
 */

#pragma once

#include <tuple>

namespace vkw
{

template<size_t N, typename... T>
using NthType = std::tuple_element_t<N, std::tuple<T...>>;

template<auto Func, typename FuncType = decltype(Func)>
class FunctionTraits;

template<auto Func, typename Ret, typename... ParamTypes>
class FunctionTraits<Func, Ret (*)(ParamTypes...)>
{
public:
    using ReturnType = Ret;
    template<size_t N>
    using NthParam = NthType<N, ParamTypes...>;
    static constexpr size_t paramCount = sizeof...(ParamTypes);

    class ParamPack
    {
    public:
	template<template<typename...> class T>
	using Apply = T<ParamTypes...>;
    };
};

template<auto Func>
using ReturnType = typename FunctionTraits<Func>::ReturnType;

template<auto Func, size_t N>
using NthParam = typename FunctionTraits<Func>::template NthParam<N>;

template<auto Func>
constexpr size_t paramCount = FunctionTraits<Func>::paramCount;

template<size_t N, typename T1, typename... T2>
inline auto getNthArg(T1 arg, T2... args)
{
    if constexpr(N == 0) {
	return arg;
    } else {
	return getNthArg<N - 1, T2...>(args...);
    }
}

template<size_t N>
class RemoveFirstN
{
public:
    template<typename... Types>
    class Pack;

    template<typename First, typename... Types>
    class Pack<First, Types...>
    {
    public:
	template<template<typename...> class T>
	using Apply = typename RemoveFirstN<N - 1>::template Pack<Types...>::template Apply<T>;
    };
};

template<>
class RemoveFirstN<0>
{
public:
    template<typename... Types>
    class Pack
    {
    public:
	template<template<typename...> class T>
	using Apply = T<Types...>;
    };
};

class ReverseTypes
{
public:
    template<typename... Types>
    class Pack
    {
    private:
	template<typename...>
	class Helper;

	template<typename First, typename... Forward>
	class Helper<First, Forward...>
	{
	public:
	    template<typename... Backward>
	    class InnerHelper
	    {
	    public:
		template<template<typename...> class T>
		using Apply = typename Helper<Forward...>::template InnerHelper<First, Backward...>::template Apply<T>;
	    };
	};

	template<>
	class Helper<>
	{
	public:
	    template<typename... Backward>
	    class InnerHelper
	    {
	    public:
		template<template<typename...> class T>
		using Apply = T<Backward...>;
	    };
	};

    public:
	template<template<typename...> class T>
	using Apply = typename Helper<Types...>::template InnerHelper<>::template Apply<T>;
    };
};

// Holy moly
template<auto Func, size_t N, template<typename...> class T>
using ApplyRemoveLastNFunctionParams =
    FunctionTraits<Func>::ParamPack::
    template Apply<ReverseTypes::Pack>::
    template Apply<RemoveFirstN<N>::template Pack>::
    template Apply<ReverseTypes::Pack>::
    template Apply<T>;

}
