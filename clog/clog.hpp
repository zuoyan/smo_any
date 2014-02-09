#ifndef FILE_FC89E822_6811_44D4_ADB6_234098823E1E_H
#define FILE_FC89E822_6811_44D4_ADB6_234098823E1E_H
#include "clog/pp.hpp"
#include <cstring>
#include <cstdio>
#include <streambuf>
#include <mutex>
#include <iostream>

namespace clog {

struct Color {
  static constexpr const char* reset() {
    return "\x1b[0m";
  }

  static constexpr const char* red() {
    return "\x1b[31m";
  }

  static constexpr const char* green() {
    return "\x1b[32m";
  }

  static constexpr const char* yellow() {
    return "\x1b[33m";
  }

  static constexpr const char* blue() {
    return "\x1b[34m";
  }

  static constexpr const char* magenta() {
    return "\x1b[35m";
  }

  static constexpr const char* cyan() {
    return "\x1b[36m";
  }
};

struct log_streambuf : std::streambuf {
  char *pptr() const {
    return std::streambuf::pptr();
  }

  void setp(char *new_pbase, char *new_epptr) {
    std::streambuf::setp(new_pbase, new_epptr);
  }
};

struct log_ostream : std::ostream {
  typedef std::ostream base;
  using base::base;
};

struct message_buffer {
  int level;
  const char level_name;
  const char *file;
  int line;
  char buf[4096];
  log_streambuf sbuf;
  log_ostream os;
  size_t num_fields = 0;
  bool keep = true;

  message_buffer(int level, const char level_name, const char *file, int line)
      : level(level),
        level_name(level_name),
        file(file),
        line(line),
        os(&sbuf) {
    sbuf.setp(buf, buf + sizeof(buf));
  }

  template <class T>
  friend message_buffer &operator<<(message_buffer &self, const T &v) {
    self.os << v;
    ++self.num_fields;
    return self;
  }
};

#define CLOG_SEVERITY_LEVEL_TRACE 1000
#define CLOG_SEVERITY_LEVEL_DEBUG 2000
#define CLOG_SEVERITY_LEVEL_INFO  3000
#define CLOG_SEVERITY_LEVEL_WARN  4000
#define CLOG_SEVERITY_LEVEL_ERROR 10000
#define CLOG_SEVERITY_LEVEL_FATAL 100000

#define CLOG_SEVERITY_NAME_TRACE 'T'
#define CLOG_SEVERITY_NAME_DEBUG 'D'
#define CLOG_SEVERITY_NAME_INFO  'I'
#define CLOG_SEVERITY_NAME_WARN  'W'
#define CLOG_SEVERITY_NAME_ERROR 'E'
#define CLOG_SEVERITY_NAME_FATAL 'F'

#ifndef NDEBUG
#define CLOG_SEVERITY_LEVEL_DFATAL CLOG_SEVERITY_LEVEL_FATAL
#define CLOG_SEVERITY_NAME_DFATAL CLOG_SEVERITY_NAME_FATAL
#else
#define CLOG_SEVERITY_LEVEL_DFATAL CLOG_SEVERITY_LEVEL_ERROR
#define CLOG_SEVERITY_NAME_DFATAL CLOG_SEVERITY_NAME_ERROR
#endif

bool log_severity(int level) {
#ifndef NDEBUG
  return true;
#else
  return level >= CLOG_SEVERITY_LEVEL_INFO;
#endif
}

void message_skip(message_buffer &mb) { mb.keep = false; }

inline message_buffer &operator<<(message_buffer &mb,
                                  void (*func)(message_buffer &)) {
  func(mb);
  return mb;
}

inline void message(message_buffer &mb) {
  static std::mutex mutex;
  std::lock_guard<std::mutex> lk(mutex);
  static bool last_keep = true;
  if (!last_keep && !mb.keep) {
    std::cerr << "\x1b[" << 1 << "A\x1b[K";
  }
  last_keep = mb.keep;
  const char *f = strrchr(mb.file, '/');
  if (!f) {
    f = mb.file;
  } else {
    f++;
  }
  auto d = std::chrono::high_resolution_clock::now().time_since_epoch();
  std::chrono::nanoseconds ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(d);
  std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(d);
  long ss = ns.count() - s.count() * 1000000000ull;
  char buf[10];
  snprintf(buf, sizeof(buf), "%09ld", ss);
  std::cerr << mb.level_name << s.count() << '.' << buf << ' ' << f << ':'
            << mb.line << "] ";
  std::cerr.write(mb.buf, mb.sbuf.pptr() - mb.buf);
  std::cerr << std::endl;
  if (mb.level >= CLOG_SEVERITY_LEVEL_FATAL) abort();
}

#define CLOG_PP_VAR(NAME) PP_CAT(CLOG_PP_VAR_, PP_CAT(NAME, __LINE__))

#define CLOG_OUTPUT(ignored, X)           \
  {                                       \
    if (CLOG_PP_VAR(buffer).num_fields) { \
      CLOG_PP_VAR(buffer) << ' ';         \
    }                                     \
    CLOG_PP_VAR(buffer) << X;             \
  }

#define CLOG_GUARD(GUARD, SEVERITY_NUM, SEVERITY_NAME, ...)                 \
  do {                                                                      \
    if ((GUARD) && clog::log_severity(SEVERITY_NUM)) {                      \
      clog::message_buffer CLOG_PP_VAR(buffer)(SEVERITY_NUM, SEVERITY_NAME, \
                                               __FILE__, __LINE__);         \
      PP_FOREACH(CLOG_OUTPUT, ~, __VA_ARGS__);                              \
      clog::message(CLOG_PP_VAR(buffer));                                   \
    }                                                                       \
  } while (0)

#ifndef NDEBUG
#define CLOG_DEBUG_GUARD true
#else
#define CLOG_DEBUG_GUARD false
#endif

#define CLOG(SEVERITY, ...)                        \
  CLOG_GUARD(true, CLOG_SEVERITY_LEVEL_##SEVERITY, \
             CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

#define DCLOG(SEVERITY, ...)                                   \
  CLOG_GUARD(CLOG_DEBUG_GUARD, CLOG_SEVERITY_LEVEL_##SEVERITY, \
             CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

#define CLOG_CHECK(EXPR, ...)                                                  \
  do {                                                                         \
    if (!(EXPR)) {                                                             \
      CLOG(FATAL, "Check " << clog::Color::red() << "failed"                   \
                           << clog::Color::reset() << ":",                     \
           #EXPR, ##__VA_ARGS__);                                              \
    } else {                                                                   \
      DCLOG(DEBUG, clog::message_skip, "Check "                                \
                                           << clog::Color::green() << "passed" \
                                           << clog::Color::reset() << ":",     \
            #EXPR, ##__VA_ARGS__);                                             \
    }                                                                          \
  } while (0)

#define CLOG_CHECK_BIN(L, BIN, R, ...) \
  CLOG_CHECK(L BIN R, "(" << L, #BIN, R << ")", ##__VA_ARGS__)

#define CLOG_CHECK_EQ(L, R, ...) CLOG_CHECK_BIN(L, ==, R, ##__VA_ARGS__)
#define CLOG_CHECK_NE(L, R, ...) CLOG_CHECK_BIN(L, !=, R, ##__VA_ARGS__)
#define CLOG_CHECK_LE(L, R, ...) CLOG_CHECK_BIN(L, <=, R, ##__VA_ARGS__)
#define CLOG_CHECK_LT(L, R, ...) CLOG_CHECK_BIN(L, <, R, ##__VA_ARGS__)
#define CLOG_CHECK_GE(L, R, ...) CLOG_CHECK_BIN(L, >=, R, ##__VA_ARGS__)
#define CLOG_CHECK_GT(L, R, ...) CLOG_CHECK_BIN(L, >, R, ##__VA_ARGS__)

#ifndef NDEBUG
#define DCLOG_CHECK CLOG_CHECK
#define DCLOG_CHECK_EQ CLOG_CHECK_EQ
#define DCLOG_CHECK_NE CLOG_CHECK_NE
#define DCLOG_CHECK_LE CLOG_CHECK_LE
#define DCLOG_CHECK_LT CLOG_CHECK_LT
#define DCLOG_CHECK_GE CLOG_CHECK_GE
#define DCLOG_CHECK_GT CLOG_CHECK_GT
#else
#define DCLOG_CHECK(...)
#define DCLOG_CHECK_EQ(...)
#define DCLOG_CHECK_NE(...)
#define DCLOG_CHECK_LE(...)
#define DCLOG_CHECK_LT(...)
#define DCLOG_CHECK_GE(...)
#define DCLOG_CHECK_GT(...)
#endif

}  // namespace clog
#endif // FILE_FC89E822_6811_44D4_ADB6_234098823E1E_H
