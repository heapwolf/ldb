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

  struct command {
    size_t id;
    string rest;
  };

  struct cDef {
    int id;
    string name;
    string alias;
  };

  extern vector<string> key_cache;

  void auto_completion(const char *buf, linenoiseCompletions *lc);
  void put_value(leveldb::DB* db, const command& cmd);
  void del_value(leveldb::DB* db, const command& cmd);
  void get_size(leveldb::DB* db, const string& start, const string& end);
  void get_value(leveldb::DB* db, const command& cmd);
  void range(leveldb::DB *db, const string& start, const string& end,
             vector<string> &key_cache, bool surpress_output);
  command parse_cmd(const string& line, const vector<cDef>& cmds);
  vector<string> parse_rest(const string& rest);
}

#endif // _LDB_H_

