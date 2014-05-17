#ifndef LDB_H_
#define LDB_H_

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <getopt.h>

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
 
//
// all commands and their aliases
//
#define GET 1
#define PUT 2
#define DEL 3
#define LS 4
#define START 5
#define END 6
#define LIMIT 7

namespace ldb {

  struct command {
    size_t id;
    string rest;
  };

  struct cDef
  {
    int id;
    string name;
    string alias;
  };

  extern vector<string> key_cache;

  void range(
    leveldb::DB *db,
    string key_start,
    string key_end,
    vector<string> &key_cache,
    bool surpress_output
  );

  void get_value(
    leveldb::DB* db,
    command cmd
  );

  void put_value(
    leveldb::DB* db,
    command cmd
  );

  command parse_cmd(string line, vector<cDef> cmds);
  vector<string> parse_rest(string rest);
  void auto_completion(const char *buf, linenoiseCompletions *lc);
}

#endif

