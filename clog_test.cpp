#include "clog/clog.hpp"

int main(int argc, char *argv[]) {
  CLOG(INFO, "start");
  CLOG_CHECK_EQ(1, 1, "1 == 1");
  for (int i = 0; i < 10000; ++i) {
    CLOG_CHECK_EQ(i, i, i, "==", i);
  }
  CLOG_CHECK_LT(1, 2, "1 < 2");
  CLOG_CHECK_EQ(1, 2, "1 == 2, fatal");
}
