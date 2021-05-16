#pragma once
#include "Property.h"

#include "MethodInvoker.h"

namespace Mono
{
template <typename T>
class PropertyInvoker : public Property
{
public:
	void SetValue(const T& val) const;
	void SetValue(const Object& object, const T& val) const;

	auto GetValue() const -> T;
	auto GetValue(const Object& object) const -> T;

private:
	template <typename Signature>
	friend PropertyInvoker<Signature> CreatePropertyInvoker(const Property&);

	explicit PropertyInvoker(const Property& property) :
		Property(property)
	{
	}
};

template <typename T>
void PropertyInvoker<T>::SetValue(const T& val) const
{
	auto thunk = CreateMethodInvoker<void(const T&)>(GetSetMethod());
	thunk(val);
}

template <typename T>
void PropertyInvoker<T>::SetValue(const Object& object, const T& val) const
{
	auto thunk = CreateMethodInvoker<void(const T&)>(GetSetMethod());
	thunk(object, val);
}

template <typename T>
auto PropertyInvoker<T>::GetValue() const -> T
{
	auto thunk = CreateMethodInvoker<T()>(GetGetMethod());
	return thunk();
}

template <typename T>
auto PropertyInvoker<T>::GetValue(const Object& object) const -> T
{
	auto thunk = CreateMethodInvoker<T()>(GetGetMethod());
	return thunk(object);
}

template <typename T>
PropertyInvoker<T> CreatePropertyInvoker(const Property& property)
{
	// const auto& expected_name = type.get_fullname();
	// bool compatible = types::is_compatible_type<T>(expected_name);
	return PropertyInvoker<T>(property);
}

template <typename T>
PropertyInvoker<T> CreatePropertyInvoker(const Type& type, const std::string& name)
{
	const auto property = type.GetProperty(name);
	return CreatePropertyInvoker<T>(property);
}
}
