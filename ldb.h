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

#define LDB_VERSION "0.0.2"

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
    string desc;
  };

  extern leveldb::DB *db;
  extern vector<cDef> cmds;

  extern vector<string> key_cache;
  extern string key_start;
  extern string key_end;
  extern int key_limit;

  extern string COLOR_RED;
  extern string COLOR_YELLOW;
  extern string COLOR_BLUE;
  extern string COLOR_GRAY;
  extern string COLOR_NONE;

  extern string hi_start;
  extern string hi_end;

  extern int json;
  string JSON(string str, int indent);

  void startREPL();
  void find(string exp, int type);
  void auto_completion(const char *buf, linenoiseCompletions *lc);
  void put_value(string key, string value);
  void get_value(string key);
  void del_value(string key);
  void get_size();
  void range(string prefix, bool surpress_output);
  command parse_cmd(const string& line, const vector<cDef>& cmds);
  vector<string> parse_rest(const string& rest, const char& splitter);
}

#endif // LDB_H_

