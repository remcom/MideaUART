#pragma once
#include "Helpers/Platform.h"
#include <cstdarg>
#include <functional>

namespace dudanov {

using LoggerFn = std::function<void(int, const char *, int, const char *, va_list)>;
extern LoggerFn logger_;
void setLogger(LoggerFn logger);

}  // namespace dudanov
