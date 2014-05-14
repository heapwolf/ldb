using namespace std;

#define MAX_LINE_LEN 512

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

  struct Commands
  {
    int id;
    string name;
    string alias;
  }

  get { GET, "get", "g" },
  put { PUT, "put", "p" },
  del { DEL, "del", "d" },
  ls { LS, "ls" },
  start { START, "start", "gt" },
  end { END, "end", "lt" },
  limit { LIMIT, "limit", "l" };

  vector<Commands> cmds = {
    get,
    put,
    del,
    ls,
    start,
    end,
    limit
  };

  void range(leveldb::DB* db, string key_start, string key_end);
  void get_value(leveldb::DB* db, command cmd);
  void put_value(leveldb::DB* db, command cmd);
  command parse_cmd(string line, vector<Commands> cmds);
  vector<string> parse_rest(string rest);
}
