#pragma once

#include "Config.h"

#include "Assembly.h"
#include "TypeConversion.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/loader.h>
END_MONO_INCLUDE

namespace Mono
{
template <typename F>
void AddInternalCall(const std::string& name, F&& func)
{
	mono_add_internal_call(name.c_str(), reinterpret_cast<const void*>(func));
}

template <typename signature_t, signature_t& signature>
struct JitInternalCallWrapper;

template <typename... args_t, void (&func)(args_t ...)>
struct JitInternalCallWrapper<void(args_t ...), func>
{
	static void wrapper(typename ConvertMonoType<std::decay_t<args_t>>::MonoUnboxedType ... args)
	{
		func(ConvertMonoType<std::decay_t<args_t>>::from_mono_unboxed(std::move(args))...);
	}
};

template <typename return_t, typename... args_t, return_t (&func)(args_t ...)>
struct JitInternalCallWrapper<return_t(args_t ...), func>
{
	static auto wrapper(
		typename ConvertMonoType<std::decay_t<args_t>>::MonoUnboxedType ... args) -> typename ConvertMonoType<
		std::decay_t<return_t>>::MonoUnboxedType
	{
		return ConvertMonoType<std::decay_t<return_t>>::to_mono(
			func(ConvertMonoType<std::decay_t<args_t>>::from_mono_unboxed(std::move(args))...));
	}
};

/*!
 * Wrap a non-static function for Mono::AddInternalCall, where automatic type
 * converstion is done through convert_mono_type. Add your own specialisation implementation
 * of this class to support more types.
 */
#define internal_call(func) &Mono::JitInternalCallWrapper<decltype(func), func>::wrapper
}
