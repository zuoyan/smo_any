#define NONE NONE_redefined
#define INFO INFO_redefined
#define ERROR ERROR_redefined
#define WARNING WARNING_redefined
#define FATAL FATAL_redefined

#include "clog/clog.hpp"
#include <unistd.h>

#define MACRO_WITH_CLOG(A, B, C) \
  CLOG(INFO, "here", A);         \
  CLOG(INFO, "here", B);         \
  CLOG(INFO, "here", C);

void test_macro() { MACRO_WITH_CLOG(1, 2, 3); }

void test_every_n() {
  for (int i = 0; i < 100; ++i) {
    CLOG_EVERY_N(10, INFO, "every 10, i=" << i);
    CLOG_IF_EVERY_N(i & 1, 10, INFO, "every 10, i=" << i);
  }
}

void test_every_sec() {
  for (int i = 0; i < 100; ++i) {
    CLOG_EVERY_SEC(0.1, INFO, "every 0.1 sec i=" << i);
    usleep(1.e6 / 100);
  }
}

void test_verbose() {
  VCLOG_IF(10, false, "false");
  VCLOG_IF(10, true, "true");
  VCLOG(10, "verbose 10");
  VCLOG(100, "verbose 100");
  VCLOG(1000, "verbose 1000");
  for (int i = 0; i < 200; ++i) {
    VCLOG_EVERY_N(i, 10, "verbose " << i, "every 10");
    VCLOG_IF_EVERY_N(i, (i & 1), 10, "verbose " << i, "every 10 if odd");
    CLOG_IF_EVERY_N(::clog::log_verbose >= i, 10, INFO, "clog_if every 10 i=" << i);
  }
}

int main(int argc, char *argv[]) {
  clog::log_verbose = 100;
  CLOG(INFO, "start");
  test_macro();
  test_every_n();
  test_every_sec();
  test_verbose();
  CLOG_CHECK_EQ(1, 1, "1 == 1");
  for (int i = 0; i < 10000; ++i) {
    CLOG_CHECK_EQ(i, i, i, "==", i);
  }
  CLOG_CHECK_LT(1, 2, "1 < 2");
  CLOG_CHECK_EQ(1, 2, "1 == 2, fatal");
  {
    int i = 0;
    CLOG_CHECK_EQ(i, 0);
  }
}
