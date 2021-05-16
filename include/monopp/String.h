#pragma once

#include "Config.h"
#include "Object.h"

namespace Mono
{
class Domain;

class String : public Object
{
public:
	explicit String(Object&& obj);
	explicit String(const Domain& domain, const std::string& str);

	auto AsUtf8() const -> std::string;
	auto AsUtf16() const -> std::u16string;
	auto AsUtf32() const -> std::u32string;
};
}
