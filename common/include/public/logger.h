#pragma once
#define SD_JOURNAL_SUPPRESS_LOCATION
#include <systemd/sd-journal.h>

#include <string>
#include <string_view>

// loggign to systemd journal to avoid clutter in sudoku 
// see logs in journalctl -f | grep Shelldoku

namespace Log {
  template <typename ...T>
  static void Debug(const char* str) {
    sd_journal_send("PRIORITY=%d", LOG_DEBUG, "SYSLOG_IDENTIFIER=%s","Shelldoku", "MESSAGE=%s", str, NULL);
  }  

  // template <typename ...T>
  // static void Info(const char* str, T... v) {
  //   sd_journal_print(LOG_INFO, str, v...);
  // }  
}