#ifndef FILE_FC89E822_6811_44D4_ADB6_234098823E1E_H
#define FILE_FC89E822_6811_44D4_ADB6_234098823E1E_H
#include "clog/pp.hpp"
#include <cstring>
#include <cstdio>
#include <streambuf>
#include <mutex>
#include <iostream>
#include <chrono>

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

#define CLOG_SEVERITY_LEVEL_NONE 0
#define CLOG_SEVERITY_LEVEL_TRACE 1000
#define CLOG_SEVERITY_LEVEL_DEBUG 2000
#define CLOG_SEVERITY_LEVEL_INFO  3000
#define CLOG_SEVERITY_LEVEL_WARN  4000
#define CLOG_SEVERITY_LEVEL_ERROR 10000
#define CLOG_SEVERITY_LEVEL_FATAL 100000

#define CLOG_SEVERITY_NAME_NONE 'N'
#define CLOG_SEVERITY_NAME_TRACE 'T'
#define CLOG_SEVERITY_NAME_DEBUG 'D'
#define CLOG_SEVERITY_NAME_INFO 'I'
#define CLOG_SEVERITY_NAME_WARN 'W'
#define CLOG_SEVERITY_NAME_ERROR 'E'
#define CLOG_SEVERITY_NAME_FATAL 'F'

#ifndef NDEBUG
#define CLOG_SEVERITY_LEVEL_DFATAL CLOG_SEVERITY_LEVEL_FATAL
#define CLOG_SEVERITY_NAME_DFATAL CLOG_SEVERITY_NAME_FATAL
#else
#define CLOG_SEVERITY_LEVEL_DFATAL CLOG_SEVERITY_LEVEL_ERROR
#define CLOG_SEVERITY_NAME_DFATAL CLOG_SEVERITY_NAME_ERROR
#endif

namespace {
#ifndef NDEBUG
int log_level = 1;
int log_verbose = 0;
#else
int log_level = CLOG_SEVERITY_LEVEL_INFO;
int log_verbose = 0;
#endif
}  // namespace

template <class T>
void avoid_never_used() {
  log_level = 0;
  log_verbose = 0;
}

inline bool log_severity(int level) {
  return level >= log_level;
}

inline void message_skip(message_buffer &mb) { mb.keep = false; }

inline message_buffer &operator<<(message_buffer &mb,
                                  void (*func)(message_buffer &)) {
  func(mb);
  return mb;
}

inline void message(message_buffer &mb) {
  static std::mutex mutex;
  std::lock_guard<std::mutex> lk(mutex);
  static bool last_keep = true;
  if (!last_keep) {
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

#define CLOG_PP_VAR(NAME, COUNTER) PP_CAT(CLOG_PP_VAR_, PP_CAT(NAME, COUNTER))

#define CLOG_OUTPUT(COUNTER, X)                    \
  {                                                \
    if (CLOG_PP_VAR(buffer, COUNTER).num_fields) { \
      CLOG_PP_VAR(buffer, COUNTER) << ' ';         \
    }                                              \
    CLOG_PP_VAR(buffer, COUNTER) << X;             \
  }

#define CLOG_(COUNTER, SEVERITY_NUM, SEVERITY_NAME, ...)    \
  do {                                                      \
    if (::clog::log_severity(SEVERITY_NUM)) {               \
      ::clog::message_buffer CLOG_PP_VAR(buffer, COUNTER)(  \
          SEVERITY_NUM, SEVERITY_NAME, __FILE__, __LINE__); \
      PP_FOREACH(CLOG_OUTPUT, COUNTER, ##__VA_ARGS__);      \
      clog::message(CLOG_PP_VAR(buffer, COUNTER));          \
    }                                                       \
  } while (0)

#define CLOG(SEVERITY, ...)                          \
  CLOG_(__COUNTER__, CLOG_SEVERITY_LEVEL_##SEVERITY, \
        CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

#ifndef NDEBUG
#define DCLOG CLOG
#else
#define DCLOG(...)
#endif

struct EveryN {
  int counter = 0;

  bool should_log(int n) { return n != 0 && (counter++ % n) == 0; }
};

#define CLOG_EVERY_N_(COUNTER, N, SEVERITY_NUM, SEVERITY_NAME, ...) \
  do {                                                              \
    static ::clog::EveryN CLOG_PP_VAR(every_n, COUNTER);            \
    if (CLOG_PP_VAR(every_n, COUNTER).should_log(N)) {              \
      CLOG_(COUNTER, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__);   \
    }                                                               \
  } while (0)

#define CLOG_EVERY_N(N, SEVERITY, ...)                          \
  CLOG_EVERY_N_(__COUNTER__, N, CLOG_SEVERITY_LEVEL_##SEVERITY, \
                CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

#ifndef NDEBUG
#define DCLOG_EVERY_N CLOG_EVERY_N
#else
#define DCLOG_EVERY_N(...)
#endif

struct FirstN {
  int counter = 0;

  bool should_log(int n) { return counter++ < n; }
};

#define CLOG_FIRST_N_(COUNTER, N, SEVERITY_NUM, SEVERITY_NAME, ...) \
  do {                                                              \
    static ::clog::FirstN CLOG_PP_VAR(first_n, COUNTER);            \
    if (CLOG_PP_VAR(first_n, COUNTER).should_log(N)) {              \
      CLOG_(COUNTER, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__);   \
    }                                                               \
  } while (0)

#define CLOG_FIRST_N(N, SEVERITY, ...)                       \
  CLOG_FIRST_N_(__COUNTER__, CLOG_SEVERITY_LEVEL_##SEVERITY, \
                CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

struct EverySec {
  std::chrono::time_point<std::chrono::steady_clock> next_log_time;

  bool should_log(double n) {
    auto now = std::chrono::steady_clock::now();
    if (now >= next_log_time) {
      next_log_time =
          now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::duration<double>(n));
      return true;
    }
    return false;
  }
};

#define CLOG_EVERY_SEC_(COUNTER, N, SEVERITY_NUM, SEVERITY_NAME, ...) \
  do {                                                                \
    static ::clog::EverySec CLOG_PP_VAR(every_sec, COUNTER);          \
    if (CLOG_PP_VAR(every_sec, COUNTER).should_log(N)) {              \
      CLOG_(COUNTER, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__);     \
    }                                                                 \
  } while (0)

#define CLOG_EVERY_SEC(N, SEVERITY, ...)                          \
  CLOG_EVERY_SEC_(__COUNTER__, N, CLOG_SEVERITY_LEVEL_##SEVERITY, \
                  CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

#ifndef NDEBUG
#define DCLOG_EVERY_SEC CLOG_EVERY_SEC
#else
#define DCLOG_EVERY_SEC(...)
#endif

struct EveryProgress {
  std::chrono::time_point<std::chrono::steady_clock> next_log_time;
  size_t next_log_index = 0;
  size_t counter = 0;

  bool should_log(double percent, double sec) {
    auto now = std::chrono::steady_clock::now();
    if (counter++ >= next_log_index || next_log_time <= now) {
      size_t ni = next_log_index * (1.0 + percent);
      if (ni == next_log_index) ni ++;
      next_log_index = ni;
      next_log_time =
          now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                    std::chrono::duration<double>(sec));
      return true;
    }
    return false;
  }
};

#define CLOG_EVERY_PROGRESS_(COUNTER, PERCENT, SEC, SEVERITY_NUM,            \
                             SEVERITY_NAME, ...)                             \
  do {                                                                       \
    static ::clog::EveryProgress CLOG_PP_VAR(every_progress, COUNTER);       \
    if (CLOG_PP_VAR(every_progress, COUNTER).should_log((PERCENT), (SEC))) { \
      CLOG_(COUNTER, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__);            \
    }                                                                        \
  } while (0)

#define CLOG_EVERY_PROGRESS(PERCENT, SEC, SEVERITY, ...) \
  CLOG_EVERY_PROGRESS_(__COUNTER__, PERCENT, SEC,        \
                       CLOG_SEVERITY_LEVEL_##SEVERITY,   \
                       CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__)

#define CLOG_IF_(COUNTER, COND, SEVERITY_NUM, SEVERITY_NAME, ...)     \
  do {                                                                \
    if (COND) {                                                       \
      CLOG_(__COUNTER__, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__); \
    }                                                                 \
  } while (0)

#define CLOG_IF(COND, SEVERITY, ...)                          \
  CLOG_IF_(__COUNTER__, COND, CLOG_SEVERITY_LEVEL_##SEVERITY, \
           CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__);

#ifndef NDEBUG
#define DCLOG_IF CLOG_IF
#else
#define DCLOG_IF(...)
#endif

#define CLOG_IF_EVERY_N_(COUNTER, COND, N, SEVERITY_NUM, SEVERITY_NAME, ...) \
  do {                                                                       \
    if (COND) {                                                              \
      CLOG_EVERY_N_(COUNTER, N, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__); \
    }                                                                        \
  } while (0)

#define CLOG_IF_EVERY_N(COND, N, SEVERITY, ...)                          \
  CLOG_IF_EVERY_N_(__COUNTER__, COND, N, CLOG_SEVERITY_LEVEL_##SEVERITY, \
                   CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__);

#ifndef NDEBUG
#define DCLOG_IF_EVERY_N CLOG_IF_EVERY_N
#else
#define DCLOG_IF_EVERY_N(...)
#endif

#define CLOG_IF_EVERY_SEC_(COUNTER, COND, N, SEVERITY_NUM, SEVERITY_NAME, ...) \
  do {                                                                         \
    if (COND) {                                                                \
      CLOG_EVERY_SEC_(COUNTER, N, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__); \
    }                                                                          \
  } while (0)

#define CLOG_IF_EVERY_SEC(COND, N, SEVERITY, ...)                          \
  CLOG_IF_EVERY_SEC_(__COUNTER__, COND, N, CLOG_SEVERITY_LEVEL_##SEVERITY, \
                     CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__);

#ifndef NDEBUG
#define DCLOG_IF_EVERY_SEC CLOG_IF_EVERY_SEC
#else
#define DCLOG_IF_EVERY_SEC(...)
#endif

#define CLOG_IF_FIRST_N_(COUNTER, COND, N, SEVERITY_NUM, SEVERITY_NAME, ...) \
  do {                                                                       \
    if (COND) {                                                              \
      CLOG_FIRST_N_(COUNTER, N, SEVERITY_NUM, SEVERITY_NAME, ##__VA_ARGS__); \
    }                                                                        \
  } while (0)

#define CLOG_IF_FIRST_N(COND, N, SEVERITY, ...)                          \
  CLOG_IF_FIRST_N_(__COUNTER__, COND, N, CLOG_SEVERITY_LEVEL_##SEVERITY, \
                   CLOG_SEVERITY_NAME_##SEVERITY, ##__VA_ARGS__);

#ifndef NDEBUG
#define DCLOG_IF_FIRST_N CLOG_IF_FIRST_N
#else
#define DCLOG_IF_FIRST_N(...)
#endif

#define VCLOG(V, ...) CLOG_IF(::clog::log_verbose >= (V), INFO, ##__VA_ARGS__);
#define VCLOG_IF(V, COND, ...) \
  CLOG_IF(::clog::log_verbose >= (V) && (COND), INFO, ##__VA_ARGS__);

#ifndef NDEBUG
#define DVCLOG VCLOG
#else
#define DVCLOG(...)
#endif

#define VCLOG_EVERY_N(V, N, ...) \
  CLOG_IF_EVERY_N(::clog::log_verbose >= (V), N, INFO, ##__VA_ARGS__);

#define VCLOG_FIRST_N(V, N, ...) \
  CLOG_IF_FIRST_N(::clog::log_verbose >= (V), N, INFO, ##__VA_ARGS__);

#define VCLOG_EVERY_SEC(V, N, ...) \
  CLOG_IF_EVERY_SEC(::clog::log_verbose >= (V), N, INFO, ##__VA_ARGS__);

#define VCLOG_IF_EVERY_N(V, COND, N, ...) \
  CLOG_IF_EVERY_N(::clog::log_verbose >= (V) && (COND), N, INFO, ##__VA_ARGS__);

#define VCLOG_IF_FIRST_N(V, COND, N, ...) \
  CLOG_IF_FIRST_N(::clog::log_verbose >= (V) && (COND), N, INFO, ##__VA_ARGS__);

#define VCLOG_IF_EVERY_SEC(V, COND, N, ...)                        \
  CLOG_IF_EVERY_SEC(::clog::log_verbose >= (V) && (COND), N, INFO, \
                    ##__VA_ARGS__);

#ifndef NDEBUG
#define DVCLOG_EVERY_N VCLOG_EVERY_N
#define DVCLOG_FIRST_N VCLOG_FIRST_N
#define DVCLOG_EVERY_SEC VCLOG_EVERY_SEC
#define DVCLOG_IF VCLOG_IF
#define DVCLOG_IF_EVERY_N VCLOG_IF_EVERY_N
#define DVCLOG_IF_FIRST_N VCLOG_IF_FIRST_N
#define DVCLOG_IF_EVERY_SEC VCLOG_IF_EVERY_SEC
#else
#define DVCLOG_EVERY_N(...)
#define DVCLOG_FIRST_N(...)
#define DVCLOG_EVERY_SEC(...)
#define DVCLOG_IF(...)
#define DVCLOG_IF_EVERY_N(...)
#define DVCLOG_IF_FIRST_N(...)
#define DVCLOG_IF_EVERY_SEC(...)
#endif

#define CLOG_GEN_CHECK(EXPR, PASS_LEVEL, FAIL_LEVEL, ...)                      \
  do {                                                                         \
    if (!(EXPR)) {                                                             \
      CLOG_(__COUNTER__, CLOG_SEVERITY_LEVEL_##FAIL_LEVEL,                     \
            CLOG_SEVERITY_NAME_##FAIL_LEVEL,                                   \
            "Check " << clog::Color::red() << "failed" << clog::Color::reset() \
                     << ":",                                                   \
            #EXPR, ##__VA_ARGS__);                                             \
    } else {                                                                   \
      CLOG_(__COUNTER__, CLOG_SEVERITY_LEVEL_##PASS_LEVEL,                     \
            CLOG_SEVERITY_NAME_##PASS_LEVEL, clog::message_skip,               \
            "Check " << clog::Color::green() << "passed"                       \
                     << clog::Color::reset() << ":",                           \
            #EXPR);                                                            \
    }                                                                          \
  } while (0)

#ifndef NDEBUG
#define CLOG_CHECK(EXPR, ...) CLOG_GEN_CHECK(EXPR, NONE, FATAL, ##__VA_ARGS__)
#else
#define CLOG_CHECK(EXPR, ...) CLOG_GEN_CHECK(EXPR, NONE, FATAL, ##__VA_ARGS__)
#endif

#ifndef NDEBUG
#define CLOG_DCHECK(EXPR, ...) CLOG_GEN_CHECK(EXPR, NONE, FATAL, ##__VA_ARGS__)
#else
#define CLOG_DCHECK(EXPR, ...) CLOG_GEN_CHECK(EXPR, NONE, ERROR, ##__VA_ARGS__)
#endif

#define CLOG_CHECK_BIN(L, BIN, R, ...) \
  CLOG_CHECK(L BIN R, "(" << L, #BIN, R << ")", ##__VA_ARGS__)

#define CLOG_DCHECK_BIN(L, BIN, R, ...) \
  CLOG_DCHECK(L BIN R, "(" << L, #BIN, R << ")", ##__VA_ARGS__)

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
