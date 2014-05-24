#include "../ldb.h"

#define GET 1
#define PUT 2
#define DEL 3
#define LS 4
#define START 5
#define END 6
#define LIMIT 7
#define SIZE 8
#define HELP 9

#define HISTORY_FILE ".ldb_history"

//
// provides all commands and their aliases.
// these are used by the repl.
//
vector<ldb::cDef> ldb::cmds = {
  { GET,   "get",   "g",  "get a key from the database" },
  { PUT,   "put",   "p",  "put a key/value into the database" },
  { DEL,   "del",   "d",  "delete a key/value from the database" },
  { LS,    "keys",  "ls", "list the keys in the current range" },
  { START, "start", "gt", "set the upper bound of the current range" },
  { END,   "end",   "lt", "set the lower bound of the current range" },
  { LIMIT, "limit", "l",  "set the limit of the current range" },
  { SIZE,  "size",  "s",  "determine the size of the current range (in bytes)" },
  { HELP,  "help",  "h",  "print this list of REPL commands" }
};

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
  if (rest.length() < 1) return;
  string prefix = line.substr(0, pos);

  for (auto & key : ldb::key_cache) {
    size_t index = key.find(rest);
    if (index != string::npos) {
      string entry = prefix + key;
      linenoiseAddCompletion(lc, entry.c_str());
    }
  }
}

//
//
//
ldb::command ldb::parse_cmd(const string& line, const vector<cDef>& cmds)
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
vector<string> ldb::parse_rest(const string& rest)
{
  vector<string> parts;
  istringstream stream(rest);
  string part;

  while (getline(stream, part, ';')) {
    parts.push_back(part);
  }

  return parts;
}

void ldb::startREPL(leveldb::DB* db) {

  char *line = NULL;

  linenoiseSetCompletionCallback(ldb::auto_completion);
  linenoiseHistoryLoad(HISTORY_FILE);

  ldb::range(db, true);

  while ((line = linenoise("> "))) {

    if ('\0' == line[0]) cout << endl;

    string l = line;
    ldb::command cmd = ldb::parse_cmd(l, ldb::cmds);

    switch (cmd.id) {
      case GET: {
        ldb::get_value(db, cmd.rest);
        break;
      }

      case PUT: {
        vector<string> pair = parse_rest(cmd.rest);
        leveldb::WriteOptions writeOptions;
        ldb::put_value(db, pair[0], pair[1]);
        break;
      }

      case DEL: {
        ldb::del_value(db, cmd.rest);
        break;
      }

      case LS:
        ldb::range(db, false);
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

      case SIZE:
        ldb::get_size(db);
        break;

      case HELP:

        for (auto &cmd : cmds) {
          cout << ' ' << setw(12) << cmd.name;
          cout << ' ' << setw(6) << cmd.alias;
          cout << ' ' << cmd.desc << endl;
        }

        break;

      default:
        cout << "unknown: " << l << endl;
        break;
    }

    linenoiseHistoryAdd(line);
    linenoiseHistorySave(HISTORY_FILE);

    free(line);
  }
}

