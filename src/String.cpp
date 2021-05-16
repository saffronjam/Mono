#include "monopp/String.h"
#include "monopp/Domain.h"

namespace Mono
{
String::String(Object&& obj) :
	Object(std::move(obj))
{
}

String::String(const Domain& domain, const std::string& str) :
	Object(reinterpret_cast<MonoObject*>(mono_string_new(domain.GetInternalPtr(), str.c_str())))
{
}

auto String::AsUtf8() const -> std::string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	::MonoString* mono_str = mono_object_to_string(GetInternalPtr(), nullptr);

	const auto raw_str = mono_string_to_utf8(mono_str);
	std::string str = reinterpret_cast<std::string::value_type*>(raw_str);
	mono_free(raw_str);
	return str;
}

auto String::AsUtf16() const -> std::u16string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	::MonoString* mono_str = mono_object_to_string(GetInternalPtr(), nullptr);

	const auto raw_str = mono_string_to_utf16(mono_str);
	std::u16string str = reinterpret_cast<std::u16string::value_type*>(raw_str);
	mono_free(raw_str);
	return str;
}

auto String::AsUtf32() const -> std::u32string
{
	// TODO: This could be probably optimized by doing no additional
	// allocation though mono_string_chars and mono_string_length.
	::MonoString* mono_str = mono_object_to_string(GetInternalPtr(), nullptr);

	const auto raw_str = mono_string_to_utf32(mono_str);
	std::u32string str = reinterpret_cast<std::u32string::value_type*>(raw_str);
	mono_free(raw_str);
	return str;
}
}
