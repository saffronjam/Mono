#pragma once
#include <cstddef>
#include <functional>
#include <map>
#include <tuple>
#include <type_traits>
#include <vector>
#include <string>

namespace Mono
{
template <typename T>
using IsMonoValuetype = std::is_pod<T>;

#pragma region FunctionTraits
template <typename Functor>
struct FunctionTraits : public FunctionTraits<decltype(&Functor::operator())>
{
};

template <typename R, typename... Args>
struct FunctionTraits<R(Args ...)>
{
	using return_type = R;
	using function_type = R(Args ...);
	constexpr static std::size_t arity = sizeof...(Args);
	using arg_types = std::tuple<Args...>;
	using arg_types_decayed = std::tuple<std::decay_t<Args>...>;
};

template <typename R, typename... Args>
struct FunctionTraits<R (*)(Args ...)> : public FunctionTraits<R(Args ...)>
{
};

template <typename C, typename R, typename... Args>
struct FunctionTraits<R (C::*)(Args ...)> : public FunctionTraits<R(Args ...)>
{
	using owner_type = C&;
};

template <typename C, typename R, typename... Args>
struct FunctionTraits<R (C::*)(Args ...) const> : public FunctionTraits<R(Args ...)>
{
	using owner_type = const C&;
};

template <typename C, typename R, typename... Args>
struct FunctionTraits<R (C::*)(Args ...) volatile> : public FunctionTraits<R(Args ...)>
{
	using owner_type = volatile C&;
};

template <typename C, typename R, typename... Args>
struct FunctionTraits<R (C::*)(Args ...) const volatile> : public FunctionTraits<R(Args ...)>
{
	using owner_type = const volatile C&;
};

template <typename Functor>
struct FunctionTraits<std::function<Functor>> : public FunctionTraits<Functor>
{
};

template <typename T>
struct FunctionTraits<T&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<const T&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<volatile T&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<const volatile T&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<T&&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<const T&&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<volatile T&&> : public FunctionTraits<T>
{
};

template <typename T>
struct FunctionTraits<const volatile T&&> : public FunctionTraits<T>
{
};

#pragma endregion

/////////////////////////////////////////////////////////////////////

template <typename... Args>
void Ignore(Args&&...)
{
}

template <typename F, typename Tuple, std::size_t... I>
decltype(auto) ApplyImpl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
	Ignore(t);
	return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple>
decltype(auto) Apply(F&& f, Tuple&& t)
{
	using indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
	return ApplyImpl(std::forward<F>(f), std::forward<Tuple>(t), indices());
}

template <typename Tuple, typename F, std::size_t... Indices>
void ForEachImpl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>)
{
	Ignore(tuple, f);
	using swallow = int[];
	(void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...};
}

template <typename Tuple, typename F>
void ForEach(Tuple&& tuple, F&& f)
{
	ForEachImpl(std::forward<Tuple>(tuple), std::forward<F>(f),
	              std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}

namespace Types
{
using Index = size_t;

inline Index& get_counter()
{
	static Index value = 0;
	return value;
}

template <typename T>
Index id()
{
	static Index sid = get_counter()++;
	return sid;
}

struct TypeName
{
	std::string name;
	std::string fullname;
};

inline const std::map<Index, TypeName>& get_types()
{
	// valid shortcuts are
	// char, bool, byte, sbyte, uint16,
	// int16, uint, int, ulong, long, uintptr,
	// intptr, single, double, string and object.

	// clang-format off
	static const std::map<Index, TypeName> types = {
		{id<std::int8_t>(), {"sbyte", "System.SByte"}}, {id<std::uint8_t>(), {"byte", "System.Byte"}},
		{id<std::int16_t>(), {"short", "System.Int16"}}, {id<std::uint16_t>(), {"ushort", "System.UInt16"}},
		{id<std::int32_t>(), {"int", "System.Int32"}}, {id<std::uint32_t>(), {"uint", "System.UInt32"}},
		{id<std::int64_t>(), {"long", "System.Int64"}}, {id<std::uint64_t>(), {"ulong", "System.UInt64"}},
		{id<bool>(), {"bool", "System.Boolean"}}, {id<float>(), {"single", "System.Single"}},
		{id<double>(), {"double", "System.Double"}}, {id<char16_t>(), {"char", "System.Char"}},
		{id<std::string>(), {"string", "System.String"}}, {id<void>(), {"void", "System.Void"}}
	};
	// clang-format on

	return types;
}

template <typename T>
TypeName GetName(bool& found)
{
	const auto& types = get_types();
	auto it = types.find(id<std::decay_t<T>>());
	if (it == types.end())
	{
		found |= false;
		return {"unknown", "Unknown"};
	}
	found |= true;
	return it->second;
}

template <typename... Args>
std::pair<std::string, bool> GetArgsSignature(const std::tuple<Args...>& tup)
{
	bool all_types_known = false;
	auto inv = [&all_types_known](auto ... args)
	{
		std::vector<std::string> argsv = {Types::GetName<decltype(args)>(all_types_known).name...};
		Ignore(args...);
		std::string result;
		size_t i = 0;
		for (const auto& tp : argsv)
		{
			if (i++ > 0)
			{
				result += ',';
			}

			result += tp;
		}

		return result;
	};

	auto args = Apply(inv, tup);
	return std::make_pair(args, all_types_known);
}

template <typename T>
bool IsCompatibleType(const std::string& expected_name)
{
	bool found = false;

	auto name = Types::GetName<T>(found).fullname;

	if (found)
	{
		return name == expected_name;
	}

	return true;
}
} // namespace types
}
