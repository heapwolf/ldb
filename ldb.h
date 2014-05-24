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
#include <sys/ioctl.h>

extern "C" {
  #include "deps/linenoise/linenoise.h"
}

#include "leveldb/db.h"

using namespace std;

#define GET 1
#define PUT 2
#define DEL 3
#define LS 4
#define START 5
#define END 6
#define LIMIT 7
#define SIZE 8

#define HISTORY_FILE ".ldb_history"

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

  extern vector<cDef> cmds;

  extern vector<string> key_cache;
  extern string key_start;
  extern string key_end;
  extern int key_limit;

  void startREPL(leveldb::DB* db);
  void auto_completion(const char *buf, linenoiseCompletions *lc);
  void put_value(leveldb::DB* db, string key, string value);
  void get_value(leveldb::DB* db, const command& cmd);
  void del_value(leveldb::DB* db, const command& cmd);
  void get_size(leveldb::DB* db);
  void range(leveldb::DB *db, bool surpress_output);
  command parse_cmd(const string& line, const vector<cDef>& cmds);
  vector<string> parse_rest(const string& rest);
}

#endif // LDB_H_

