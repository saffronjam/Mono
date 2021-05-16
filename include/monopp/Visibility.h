#pragma once

#include "Config.h"

namespace Mono
{
enum class Visibility
{
	Private,
	ProtectedInternal,
	Internal,
	Protected,
	Public
};

inline std::string ToString(Visibility vis)
{
	switch (vis)
	{
	case Visibility::Private: return "private";
	case Visibility::ProtectedInternal: return "protected internal";
	case Visibility::Internal: return "internal";
	case Visibility::Protected: return "protected";
	case Visibility::Public: return "public";
	}
	return "private";
}
}
