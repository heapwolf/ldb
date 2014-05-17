#include "../ldb.h"

namespace ldb {

  //
  //
  //
  void auto_completion(const char *buf, linenoiseCompletions *lc)
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
  void put_value(leveldb::DB* db, ldb::command cmd)
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
  void get_value(leveldb::DB* db, ldb::command cmd)
  {
    string value;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), cmd.rest, &value);
    cout << status.ToString() << endl;
    cout << value << endl;
  }

  //
  //
  //
  void range(
    leveldb::DB* db,
    string key_start,
    string key_end,
    vector<string>& key_cache,
    bool surpress_output)
  {
    key_cache.clear();

    leveldb::Slice start = key_start;
    leveldb::Slice end = key_end;

    leveldb::Iterator* itr = db->NewIterator(leveldb::ReadOptions());

    for (itr->Seek(start); itr->Valid(); itr->Next()) 
    {
      leveldb::Slice key = itr->key();
      leveldb::Slice value = itr->value();

      string sKey = key.ToString();

      if (sKey == key_end)
      {
        break;
      }

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
  command parse_cmd(string line, vector<cDef> cmds) 
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
  vector<string> parse_rest(string rest)
  {
    vector<string> parts;
    istringstream stream(rest);
    string part;

    while(getline(stream, part, ';')) {
      parts.push_back(part);
    }

    return parts;
  }

} // ns: ldb

