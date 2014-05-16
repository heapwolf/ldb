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
#include "ldb.h"

#define HISTORY ".ldb_history"

int main(int argc, char** argv)
{
  string key_start = "";
  string key_end = "~";
  int key_limit = 1000;

  ldb::Options options = ldb::get_cli_options(argc, argv);
  leveldb::DB* db;
  leveldb::Status status = leveldb::DB::Open(options, options.path, &db);

  if (false == status.ok()) {
    cerr << "Unable to open/create database './testdb'" << endl;
    cerr << status.ToString() << endl;
    return -1;
  }

  linenoiseSetCompletionCallback(ldb::auto_completion);
  linenoiseHistoryLoad(HISTORY);

  ldb::range(db, key_start, key_end, ldb::key_cache, true);

  char *line = NULL;
  while ((line = linenoise("> "))) {

    if ('\0' == line[0]) cout << endl;

    string l = line;
    ldb::command cmd = ldb::parse_cmd(l, ldb::cmds);

    switch (cmd.id) {
      case GET:
        ldb::get_value(db, cmd);
        break;

      case PUT:
        ldb::put_value(db, cmd);
        break;

      case DEL:
        break;

      case LS:
        ldb::range(db, key_start, key_end, ldb::key_cache, false);
        break;

      case START:
        cout << "START set to " << cmd.rest << endl;
        key_start = cmd.rest;
        break;

      case END:
        cout << "END set to " << cmd.rest << endl;
        key_end = cmd.rest;
        break;

      case LIMIT: {
        string msg = "LIMIT set to ";

        if (cmd.rest.length() == 0) {
          cout << msg << key_limit << endl;
          break;
        }
        cout << msg << cmd.rest << endl;
        key_limit = atoi(cmd.rest.c_str());
        break;
      }

      default:
        cout << l << endl;
    }

    linenoiseHistoryAdd(line);
    linenoiseHistorySave(HISTORY);

    free(line);
  }

   delete db;
}

//
//
//
void ldb::put_value(leveldb::DB* db, ldb::command cmd)
{
  vector<string> parts = parse_rest(cmd.rest);

  leveldb::WriteOptions writeOptions;

  ostringstream keyStream;
  keyStream << parts[0];

  ostringstream valueStream;
  valueStream << parts[1];

  db->Put(writeOptions, keyStream.str(), valueStream.str());
}

//
//
//
void ldb::get_value(leveldb::DB* db, ldb::command cmd)
{
  string value;
  leveldb::Status status = db->Get(leveldb::ReadOptions(), cmd.rest, &value);
  cout << status.ToString() << endl;
  cout << value << endl;
}

//
//
//
void ldb::range(leveldb::DB* db, string key_start, string key_end,
                vector<string>& key_cache, bool surpress_output)
{
  key_cache.clear();

  leveldb::Slice start = key_start;
  leveldb::Slice end = key_end;

  leveldb::Iterator* itr = db->NewIterator(leveldb::ReadOptions());

  for (itr->Seek(start); itr->Valid(); itr->Next()) {
    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    string sKey = key.ToString();

    if (sKey == key_end) break;

    key_cache.push_back(sKey);

    if (surpress_output == false) {
      cout << sKey << endl;
    }
  }
  delete itr;
}

//
//
//
ldb::command ldb::parse_cmd(string line, vector<Commands> cmds)
{
  int pos = line.find(' ');
  string name = line.substr(0, pos);
  command cmd;

  cmd.id = 0;
  cmd.rest = pos > 0 ? line.substr(pos + 1) : "";

  for (auto & c : cmds) {
    if (c.name == name || c.alias == name) {
      cmd.id = c.id;
    }
  }

  return cmd;
}

//
// duh, this is obviously too simple for most,
// cases, keys could easily have `;` in them.
//
vector<string> ldb::parse_rest(string rest)
{
  vector<string> parts;
  istringstream stream(rest);
  string part;

  while (getline(stream, part, ';')) {
    parts.push_back(part);
  }

  return parts;
}

//
//
//
void ldb::auto_completion(const char *buf, linenoiseCompletions *lc)
{
  string line(buf);
  //
  // this should actually search to find out if the thing
  // is a command that should actually have completion.
  //
  regex e("(\\s+)");
  smatch m;

  regex_search(line, m, e);
  if (m.empty()) return;

  int pos = m.position() + m.length();
  string rest = line.substr(pos + 1);
  string prefix = line.substr(0, pos);

  for (auto & key : key_cache) {
    if (key.find(rest) != string::npos) {
      string entry = prefix + key;
      linenoiseAddCompletion(lc, entry.c_str());
    }
  }
}

//
//
//
ldb::Options ldb::get_cli_options(int argc, char** argv)
{
  ldb::Options options;

  if (argc == 2) options.path = argv[1];

  int c;

  while((c = getopt(argc, argv, "i:ec:d")) != EOF) {
    switch (c) {
      case 'i':
        options.path = optarg;
        break;
      case 'e':
        options.error_if_exists = true;
        break;
      case 'c':
        options.path = optarg;
        options.create_if_missing = true;
        break;
    }
  }

  return options;
}
