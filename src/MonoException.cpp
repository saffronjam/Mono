#include "monopp/MonoException.h"
#include "monopp/Object.h"
#include "monopp/PropertyInvoker.h"
#include "monopp/Type.h"

namespace Mono
{
auto get_exception_info(MonoObject* ex) -> MonoThunkException::MonoExceptionInfo
{
	auto obj = Object(ex);
	const auto& type = obj.GetType();
	const auto& exception_typename = type.GetFullname();

	auto message_prop = type.GetProperty("Message");
	auto mutable_msg_prop = CreatePropertyInvoker<std::string>(message_prop);

	auto stacktrace_prop = type.GetProperty("StackTrace");
	auto mutable_stack_prop = CreatePropertyInvoker<std::string>(stacktrace_prop);

	auto message = mutable_msg_prop.GetValue(obj);
	auto stacktrace = mutable_stack_prop.GetValue(obj);
	return {exception_typename, message, stacktrace};
}

MonoThunkException::MonoThunkException(MonoObject* ex) :
	MonoThunkException(get_exception_info(ex))
{
}

auto MonoThunkException::ExceptionTypename() const -> const std::string&
{
	return info_.ExceptionTypename;
}

auto MonoThunkException::Message() const -> const std::string&
{
	return info_.Message;
}

auto MonoThunkException::StackTrace() const -> const std::string&
{
	return info_.StackTrace;
}

MonoThunkException::MonoThunkException(const MonoExceptionInfo& info) :
	MonoException(info.ExceptionTypename + "(" + info.Message + ")\n" + info.StackTrace),
	info_(info)
{
}
}
