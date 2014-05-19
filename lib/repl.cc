#include "../ldb.h"

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
#define SIZE 8

#define HISTORY_FILE ".ldb_history"

vector<string> key_cache;

struct command {
  size_t id;
  string rest;
};

struct cDef {
  int id;
  string name;
  string alias;
};

const vector<cDef> cmds = {
  { GET, "get", "g" },
  { PUT, "put", "p" },
  { DEL, "del", "d" },
  { LS, "ls" },
  { START, "start", "gt" },
  { END, "end", "lt" },
  { LIMIT, "limit", "l" },
  { SIZE, "size", "s" }
};

//
//
//
static void get_value(leveldb::DB* db, const command& cmd)
{
  string value;
  leveldb::Status status = db->Get(leveldb::ReadOptions(), cmd.rest, &value);
  if (!status.ok()) {
    cerr << status.ToString() << endl;
  }
  else {
    cout << value << endl;
  }
}

//
// duh, this is obviously too simple for most,
// cases, keys could easily have `;` in them.
//
static vector<string> parse_rest(const string& rest)
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
static void put_value(leveldb::DB* db, const command& cmd)
{
  vector<string> parts = parse_rest(cmd.rest);

  leveldb::WriteOptions writeOptions;

  ostringstream keyStream;
  keyStream << parts[0];

  ostringstream valueStream;
  valueStream << parts[1];

  leveldb::Status status;
  status = db->Put(writeOptions, keyStream.str(), valueStream.str());
  if (!status.ok()) {
    cerr << status.ToString() << endl;
  }
}

//
//
//
static void del_value(leveldb::DB* db, const command& cmd)
{
  vector<string> parts = parse_rest(cmd.rest);

  leveldb::WriteOptions writeOptions;

  ostringstream keyStream;
  keyStream << parts[0];

  leveldb::Status status;
  status = db->Delete(leveldb::WriteOptions(), cmd.rest);
  if (!status.ok()) {
    cerr << status.ToString() << endl;
  }
}

//
//
//
static void range(leveldb::DB* db, const string& start, const string& end,
                  bool surpress_output)
{
  struct winsize term;
  ioctl(0, TIOCGWINSZ, &term);

  key_cache.clear();

  int count = 0;
  int maxWidth = 0;
  int maxColumns = 0;
  int padding = 2;

  leveldb::Iterator* itr = db->NewIterator(leveldb::ReadOptions());

  for (itr->Seek(start); itr->Valid(); itr->Next()) {
    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    int len = key.ToString().length();
    if (len > maxWidth) {
      maxWidth = len;
    }
  }

  maxWidth += padding;
  maxColumns = term.ws_col / maxWidth;

  for (itr->Seek(start); itr->Valid(); itr->Next()) {
    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    string sKey = key.ToString();

    if (sKey == end) break;

    key_cache.push_back(sKey);

    if (surpress_output == false) {
      count++;
      cout << setw(maxWidth) << left << sKey;
      if (count == maxColumns - 1) {
        count = 0;
        cout << endl;
      }
    }
  }

  cout << endl;
  delete itr;
}

//
//
//
static void get_size(leveldb::DB* db, const string& start, const string& end)
{
  leveldb::Range ranges[1]; // we only keep one range, maybe allow more?
  ranges[0] = leveldb::Range(start, end);
  uint64_t sizes[1];

  // may be a bug in the docs, GetApproximateSizes returns void, not Status.
  db->GetApproximateSizes(ranges, 1, sizes);
  if (sizes[0]) {
    cout << sizes[0];
  }
}

//
//
//
static command parse_cmd(const string& line)
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
//
//
static void auto_completion(const char *buf, linenoiseCompletions *lc)
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

  for (auto & key : key_cache) {
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
void ldb::start_repl(leveldb::DB* db)
{
  string key_start = "";
  string key_end = "~";
  int key_limit = 1000;

  range(db, key_start, key_end, true);

  linenoiseSetCompletionCallback(auto_completion);
  linenoiseHistoryLoad(HISTORY_FILE);

  char *line = NULL;
  while ((line = linenoise("> "))) {

    if ('\0' == line[0]) cout << endl;

    string l = line;
    command cmd = parse_cmd(l);

    switch (cmd.id) {
      case GET:
        get_value(db, cmd);
        break;

      case PUT:
        put_value(db, cmd);
        break;

      case DEL:
        del_value(db, cmd);
        break;

      case LS:
        range(db, key_start, key_end, false);
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
        get_size(db, key_start, key_end);

      default:
        cout << l << endl;
    }

    linenoiseHistoryAdd(line);
    linenoiseHistorySave(HISTORY_FILE);

    free(line);
  }
}
