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

  struct Options : public leveldb::Options {
    string path;
  };

  struct command {
    size_t id;
    string rest;
  };

  struct Commands {
    int id;
    string name;
    string alias;
  };

  vector<Commands> cmds = {
    { GET, "get", "g" },
    { PUT, "put", "p" },
    { DEL, "del", "d" },
    { LS, "ls" },
    { START, "start", "gt" },
    { END, "end", "lt" },
    { LIMIT, "limit", "l" }
  };

  vector<string> key_cache;

  leveldb::DB* create_database(int argc, char** argv);

  void put_value(leveldb::DB* db, command cmd);
  void get_value(leveldb::DB* db, command cmd);
  void range(leveldb::DB *db, string key_start, string key_end,
              vector<string> &key_cache, bool surpress_output);

  command parse_cmd(string line, vector<Commands> cmds);
  vector<string> parse_rest(string rest);
  void auto_completion(const char *buf, linenoiseCompletions *lc);
  Options get_cli_options(int argc, char** argv);

}

