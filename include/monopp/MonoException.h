#pragma once

#include "Config.h"

BEGIN_MONO_INCLUDE
#include <mono/metadata/reflection.h>
END_MONO_INCLUDE

namespace Mono
{
class MonoException : public std::runtime_error
{
	using runtime_error::runtime_error;
};

class MonoThunkException : public MonoException
{
public:
	explicit MonoThunkException(MonoObject* ex);

	const std::string& ExceptionTypename() const;

	const std::string& Message() const;

	const std::string& StackTrace() const;

	struct MonoExceptionInfo
	{
		std::string ExceptionTypename;
		std::string Message;
		std::string StackTrace;
	};

private:
	explicit MonoThunkException(const MonoExceptionInfo& info);

	MonoExceptionInfo info_;
};
}
