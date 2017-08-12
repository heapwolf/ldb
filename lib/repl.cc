#include "../ldb.h"

#define GET 0
#define PUT 2
#define DEL 3
#define LS 4
#define LOWER 5
#define UPPER 6
#define LIMIT 7
#define JSON 8
#define SIZE 9
#define FIND 10
#define HELP 11

#define HISTORY_FILE ".ldb_history"

//
// provides all commands and their aliases.
// these are used by the repl.
//
vector<ldb::cDef> ldb::cmds = {
  { GET,   "get",   "g",   "get a key from the database <str>" },
  { PUT,   "put",   "p",   "put a key/value into the database <str>" },
  { DEL,   "del",   "rm",  "delete a key/value from the database <str>" },
  { LS,    "keys",  "ls",  "list the keys in the current range" },
  { LS,    "keys",  "ll",  "list the keys in the current range" },
  { LS,    "keys",  "la",  "list the keys in the current range" },
  { LOWER, "lower", "gt",  "set the lower bound of the current range <str>" },
  { UPPER, "upper", "lt",  "set the uppper bound of the current range <str>" },
  { LIMIT, "limit", "l",   "set the limit of the current range <n>" },
  { JSON,  "json",  "j",   "try to format values as json <n>" },
  { SIZE,  "size",  "s",   "determine the size of the current range (in bytes)" },
  { FIND,  "find",  "in",  "execute an expression against the current range" },
  { HELP,  "help",  "?",   "print this list of REPL commands" }
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

  int pos = m.position() + m.length();
  if (pos >= line.length()) return;
  string rest = line.substr(pos);
  string prefix = line.substr(0, pos);

  ldb::range(rest, false);

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
vector<string> ldb::parse_rest(const string& rest, const char& splitter)
{
  vector<string> parts;
  istringstream stream(rest);
  string part;

  while (getline(stream, part, splitter)) {
    parts.push_back(part);
  }

  return parts;
}

void ldb::startREPL() {

  char *line = NULL;

  linenoiseSetCompletionCallback(ldb::auto_completion);
  linenoiseHistoryLoad(HISTORY_FILE);

  ldb::range("", true);

  while ((line = linenoise(">"))) {

    string l = line;
    ldb::command cmd = ldb::parse_cmd(l, ldb::cmds);

    switch (cmd.id) {
      case GET: {
        ldb::get_value(cmd.rest);
        break;
      }

      case PUT: {
        vector<string> pair = parse_rest(cmd.rest, ' ');
        ldb::put_value(pair[0], pair[1]);
        break;
      }

      case DEL: {
        ldb::del_value(cmd.rest);
        break;
      }

      case LS:
        ldb::range("", false);
        break;

      case LOWER:
        cout << "LOWER set to " << cmd.rest << endl;
        key_start = cmd.rest;
        break;

      case UPPER:
        cout << "UPPER set to " << cmd.rest << endl;
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

        ldb::range("", true);
        break;
      }

      case JSON:
        json = atoi(cmd.rest.c_str());
        break;

      case SIZE:
        ldb::get_size();
        break;

      case FIND: {

        vector<string> parts = parse_rest(cmd.rest, ' ');

        if (parts.size() == 2) {
          ldb::find(parts[1], parts[0] == "keys" ? 0 : 1);
        } else {
          ldb::find(cmd.rest, 0);
        }

        break;
      }

      case HELP:
        cout << "Settings:" << endl << endl;

        cout << setw(16) << "upper " << hi_start << key_end << hi_end << endl;
        cout << setw(16) << "lower " << hi_start << key_start << hi_end << endl;
        cout << setw(16) << "limit " << hi_start << key_limit << hi_end << endl;
        cout << setw(16) << "json " << hi_start << json << hi_end << endl << endl;

        cout << "Commands:" << endl << endl;

        for (auto &cmd : cmds) {
          cout << ' ' << setw(16) << cmd.name;
          cout << ' ' << setw(6) << cmd.alias;
          cout << ' ' << cmd.desc << endl;
        }

        break;

      default:
        return;
    }

    linenoiseHistoryAdd(line);
    linenoiseHistorySave(HISTORY_FILE);

    free(line);
  }
}

