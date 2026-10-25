#pragma once
#include "easylogging++.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace venera {
inline void configure_console_output(){
#ifdef _WIN32
  DWORD mode = 0;
  if (GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode) ||
      GetConsoleMode(GetStdHandle(STD_ERROR_HANDLE), &mode))
    SetConsoleOutputCP(CP_UTF8);
#endif
  el::Configurations format;
  format.setGlobally(el::ConfigurationType::Format,"[%datetime{%Y-%M-%d %H:%m:%s.%g}]\t%thread\t%level\t%logger\t%loc\t%msg");
  el::Loggers::setDefaultConfigurations(format,true);
}
}
