#ifndef _LDB_H_
#define _LDB_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <getopt.h>
#include <sys/ioctl.h>

extern "C" {
  #include "deps/linenoise/linenoise.h"
}

#include "leveldb/db.h"

using namespace std;

//
// Colors for formatting completions
//
#define BRIGHT 1
#define RED 31

namespace ldb {
  void start_repl(leveldb::DB* db);
}

#endif // _LDB_H_

