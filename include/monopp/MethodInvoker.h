#pragma once

#include "Config.h"

#include "MonoException.h"
#include "Method.h"
#include "Object.h"
#include "Type.h"
#include "TypeConversion.h"

#include <tuple>
#include <utility>
#include <vector>

namespace Mono
{
template <typename T>
auto IsCompatibleType(const Type& type) -> bool
{
	const auto& expected_name = type.GetFullname();
	return Types::IsCompatibleType<T>(expected_name);
}

template <typename Signature>
auto HasCompatibleSignature(const Method& method) -> bool
{
	constexpr auto arity = FunctionTraits<Signature>::arity;
	using ReturnType = typename FunctionTraits<Signature>::return_type;
	using ArgTypes = typename FunctionTraits<Signature>::arg_types_decayed;
	const auto expected_rtype = method.GetReturnType();
	auto expected_arg_types = method.GetParamTypes();

	bool compatible = arity == expected_arg_types.size();
	if (!compatible)
	{
		return false;
	}
	compatible &= IsCompatibleType<ReturnType>(expected_rtype);
	if (!compatible)
	{
		// allow cpp return type to be void i.e ignoring it.
		if (!IsCompatibleType<void>(expected_rtype))
		{
			return false;
		}
	}
	ArgTypes tuple;
	size_t idx = 0;
	ForEach(tuple, [&compatible, &idx, &expected_arg_types](const auto& arg)
	{
		Ignore(arg);
		const auto expected_arg_type = expected_arg_types[idx];
		using arg_type = decltype(arg);
		compatible &= IsCompatibleType<arg_type>(expected_arg_type);

		idx++;
	});

	return compatible;
}

template <typename T>
class MethodInvoker;

template <typename... Args>
class MethodInvoker<void(Args ...)> : public Method
{
public:
	void operator()(Args ... args)
	{
		Invoke(nullptr, std::forward<Args>(args)...);
	}

	void operator()(const Object& obj, Args ... args)
	{
		Invoke(&obj, std::forward<Args>(args)...);
	}

private:
	void Invoke(const Object* obj, Args ... args)
	{
		auto method = this->_method;
		MonoObject* object = nullptr;
		if (obj)
		{
			object = obj->GetInternalPtr();

			method = mono_object_get_virtual_method(object, method);
		}
		auto tup = std::make_tuple(ConvertMonoType<std::decay_t<Args>>::to_mono(std::forward<Args>(args))...);

		auto inv = [method, object](auto ... args)
		{
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			mono_runtime_invoke(method, object, argsv.data(), &ex);
			if (ex)
			{
				throw MonoThunkException(ex);
			}
		};

		Apply(inv, tup);
	}

	template <typename Signature>
	friend auto CreateMethodInvoker(const Method&, bool) -> MethodInvoker<Signature>;

	MethodInvoker(const Method& o) :
		Method(o)
	{
	}
};

template <typename RetType, typename... Args>
class MethodInvoker<RetType(Args ...)> : public Method
{
public:
	auto operator()(Args ... args)
	{
		return Invoke(nullptr, std::forward<Args>(args)...);
	}

	auto operator()(const Object& obj, Args ... args)
	{
		return Invoke(&obj, std::forward<Args>(args)...);
	}

private:
	auto Invoke(const Object* obj, Args ... args)
	{
		auto method = this->_method;
		MonoObject* object = nullptr;
		if (obj)
		{
			object = obj->GetInternalPtr();

			method = mono_object_get_virtual_method(object, method);
		}
		auto tup = std::make_tuple(ConvertMonoType<std::decay_t<Args>>::to_mono(std::forward<Args>(args))...);
		auto inv = [method, object](auto ... args)
		{
			std::vector<void*> argsv = {to_mono_arg(args)...};

			MonoObject* ex = nullptr;
			auto result = mono_runtime_invoke(method, object, argsv.data(), &ex);
			if (ex)
			{
				throw MonoThunkException(ex);
			}

			return result;
		};

		auto result = Apply(inv, tup);
		return ConvertMonoType<std::decay_t<RetType>>::from_mono_boxed(std::move(result));
	}


	template <typename Signature>
	friend auto CreateMethodInvoker(const Method&, bool) -> MethodInvoker<Signature>;

	explicit MethodInvoker(const Method& o) :
		Method(o)
	{
	}
};

template <typename Signature>
auto CreateMethodInvoker(const Method& method, bool check_signature = true) -> MethodInvoker<Signature>
{
	if (check_signature && !HasCompatibleSignature<Signature>(method))
	{
		throw MonoException("NATIVE::Method thunk requested with incompatible signature");
	}
	return MethodInvoker<Signature>(method);
}

template <typename Signature>
auto CreateMethodInvoker(const Type& type, const std::string& name) -> MethodInvoker<Signature>
{
	using ArgTypes = typename FunctionTraits<Signature>::arg_types;
	ArgTypes tup;
	auto args_result = Types::GetArgsSignature(tup);
	auto args = args_result.first;
	auto all_types_known = args_result.second;

	if (all_types_known)
	{
		auto func = type.GetMethod(name + "(" + args + ")");
		return CreateMethodInvoker<Signature>(func);
	}
	constexpr auto arg_count = FunctionTraits<Signature>::arity;
	auto func = type.GetMethod(name, arg_count);
	return CreateMethodInvoker<Signature>(func);
}
}
