#pragma once
#include <REX/LOG.h>


// #define LOG_ERROR(a) REX::ERROR(a);

#define LOG_WARNING(a) REX::WARN(a);
#define LOG_INFO(a) REX::INFO(a);
#define LOG_CRITICAL(a) REX::CRITICAL(a;


#define LOG_TO_CONSOLE(a) if (auto* console = RE::ConsoleLog::GetSingleton()) console->AddString(a);
