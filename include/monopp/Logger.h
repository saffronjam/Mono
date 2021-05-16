#pragma once

#include "Config.h"
#include <functional>

namespace Mono
{
using LogHandler = std::function<void(const std::string&)>;
void SetLogHandler(const std::string& category, const LogHandler& handler);
const LogHandler& GetLogHandler(const std::string& category);
}
