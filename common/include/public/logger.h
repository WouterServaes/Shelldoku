#pragma once
#define SD_JOURNAL_SUPPRESS_LOCATION
#include <systemd/sd-journal.h>

#include <string>
#include <string_view>

// loggign to systemd journal to avoid clutter in sudoku 
// see logs in journalctl -f | grep Shelldoku

namespace Log {
  
  static void Debug(const char* str) {
    sd_journal_send("PRIORITY=%d", LOG_DEBUG, "SYSLOG_IDENTIFIER=%s","Shelldoku", "MESSAGE=%s", str, NULL);
  }  

  static void Debug(const std::string& str) {
    Debug(str.c_str());
  }
}