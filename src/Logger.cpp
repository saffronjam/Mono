#include "monopp/Logger.h"
#include <map>

namespace Mono
{
namespace detail
{
static std::map<std::string, LogHandler> LogCallbacks;
}

const LogHandler& GetLogHandler(const std::string& category)
{
	return detail::LogCallbacks[category];
}

void SetLogHandler(const std::string& category, const LogHandler& handler)
{
	detail::LogCallbacks[category] = handler;
}
}
