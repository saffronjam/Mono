#pragma once
#include "Field.h"

#include "Domain.h"
#include "Object.h"
#include "TypeConversion.h"

namespace Mono
{
template <typename T>
class FieldInvoker : public Field
{
public:
	void SetValue(const T& val) const;
	void SetValue(const Object& object, const T& val) const;

	auto GetValue() const -> T;
	auto GetValue(const Object& object) const -> T;

private:
	template <typename signature_t>
	friend auto CreateFieldInvoker(const Field&) -> FieldInvoker<signature_t>;

	explicit FieldInvoker(const Field& field) :
		Field(field)
	{
	}

private:
	void SetValueImpl(const Object* object, const T& val) const;
	auto GetValueImpl(const Object* object) const -> T;
};

template <typename T>
void FieldInvoker<T>::SetValue(const T& val) const
{
	SetValueImpl(nullptr, val);
}

template <typename T>
void FieldInvoker<T>::SetValue(const Object& object, const T& val) const
{
	SetValueImpl(&object, val);
}

template <typename T>
void FieldInvoker<T>::SetValueImpl(const Object* object, const T& val) const
{
	assert(_field);

	auto mono_val = ConvertMonoType<T>::to_mono(val);
	auto arg = to_mono_arg(mono_val);

	if (object)
	{
		auto obj = object->GetInternalPtr();
		assert(obj);
		mono_field_set_value(obj, _field, arg);
	}
	else
	{
		mono_field_static_set_value(_owningTypeVtable, _field, arg);
	}
}

template <typename T>
auto FieldInvoker<T>::GetValue() const -> T
{
	return GetValueImpl(nullptr);
}

template <typename T>
auto FieldInvoker<T>::GetValue(const Object& object) const -> T
{
	return GetValueImpl(&object);
}

template <typename T>
auto FieldInvoker<T>::GetValueImpl(const Object* object) const -> T
{
	T val{};
	assert(_field);
	MonoObject* refvalue = nullptr;
	auto arg = reinterpret_cast<void*>(&val);
	if (!IsValuetype())
	{
		arg = &refvalue;
	}
	if (object)
	{
		const auto obj = object->GetInternalPtr();
		assert(obj);
		mono_field_get_value(obj, _field, arg);
	}
	else
	{
		mono_field_static_get_value(_owningTypeVtable, _field, arg);
	}

	if (!IsValuetype())
	{
		val = ConvertMonoType<T>::from_mono_boxed(refvalue);
	}
	return val;
}

template <typename T>
auto CreateFieldInvoker(const Field& field) -> FieldInvoker<T>
{
	// const auto& expected_name = type.get_fullname();
	// bool compatible = types::is_compatible_type<T>(expected_name);
	return FieldInvoker<T>(field);
}

template <typename T>
auto CreateFieldInvoker(const Type& type, const std::string& name) -> FieldInvoker<T>
{
	const auto field = type.GetField(name);
	return CreateFieldInvoker<T>(field);
}
}
