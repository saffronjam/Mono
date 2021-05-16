#pragma once

#include "Config.h"

#include "Domain.h"
#include "String.h"
#include "Type.h"
#include "TypeTraits.h"

namespace Mono
{
template <typename T>
auto to_mono_arg(T& t)
{
	static_assert(IsMonoValuetype<T>::value, "Should not pass here for non-pod types");
	return std::addressof(t);
}

inline auto to_mono_arg(Object* t)
{
	return t;
}

template <typename T>
struct ConvertMonoType
{
	using CppType = T;
	using MonoUnboxedType = T;
	using MonoBoxedType = MonoObject*;

	static_assert(std::is_scalar<MonoUnboxedType>::value, "Specialize convertor for non-scalar types");

	static auto to_mono(const CppType& obj) -> MonoUnboxedType
	{
		return obj;
	}

	static auto from_mono_unboxed(const MonoUnboxedType& obj) -> CppType
	{
		return obj;
	}

	static auto from_mono_boxed(const MonoBoxedType& obj) -> CppType
	{
		const Object object(obj);
		const auto& type = object.GetType();
		const auto mono_sz = type.GetSizeof();
		const auto mono_align = type.GetAlignof();
		constexpr auto cpp_sz = sizeof(CppType);
		constexpr auto cpp_align = alignof(CppType);
		Ignore(mono_align, mono_sz, cpp_sz, cpp_align);
		assert(mono_sz <= cpp_sz && mono_align <= cpp_align && "Different type layouts");
		void* ptr = mono_object_unbox(obj);
		return *reinterpret_cast<CppType*>(ptr);
	}
};

template <>
struct ConvertMonoType<Object>
{
	using CppType = Object;
	using MonoUnboxedType = MonoObject*;
	using MonoBoxedType = MonoObject*;

	static auto to_mono(const CppType& obj) -> MonoUnboxedType
	{
		return obj.GetInternalPtr();
	}

	static auto from_mono_unboxed(const MonoUnboxedType& obj) -> CppType
	{
		return CppType(obj);
	}

	static auto from_mono_boxed(const MonoUnboxedType& obj) -> CppType
	{
		return CppType(obj);
	}
};

template <>
struct ConvertMonoType<std::string>
{
	using CppType = std::string;
	using MonoUnboxedType = MonoObject*;
	using MonoBoxed = MonoObject*;

	static auto to_mono(const CppType& obj) -> MonoUnboxedType
	{
		const auto& domain = Domain::GetCurrentDomain();
		return String(domain, obj).GetInternalPtr();
	}

	static auto from_mono_unboxed(const MonoUnboxedType& obj) -> CppType
	{
		return String(Object(obj)).AsUtf8();
	}

	static auto from_mono_boxed(const MonoUnboxedType& obj) -> CppType
	{
		return String(Object(obj)).AsUtf8();
	}
};
}
