#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <readline/readline.h>
#include <readline/history.h>

#include <getopt.h>
#include "leveldb/db.h"
#include "ldb.h"

int main(int argc, char** argv)
{
  string path;

  if (argc == 2) path = argv[1];

  string key_start = "";
  string key_end = "~";
  int key_limit = 1000;

  leveldb::DB* db;
  leveldb::Options options;

  int c;

  while((c = getopt(argc, argv, "i:ec:d")) != EOF)
  {
    switch (c)
    {
      case 'i':
        path = optarg;
      break;
      case 'e':
        options.error_if_exists = true;
      break;
      case 'c':
        path = optarg;
        options.create_if_missing = true;
      break;
    }
  }

  leveldb::Status status = leveldb::DB::Open(options, path, &db);

  if (false == status.ok())
  {
    cerr << "Unable to open/create database './testdb'" << endl;
    cerr << status.ToString() << endl;
    return -1;
  }

  while (1)
  {
    cout << "> ";
    char line[MAX_LINE_LEN];
    cin.getline(line, MAX_LINE_LEN);
    string l = line;
    ldb::command cmd = ldb::parse_cmd(l, ldb::cmds);

    switch (cmd.id)
    {
      case GET:
      {
        ldb::get_value(db, cmd);
        break;
      }

      case PUT:
      {
        ldb::put_value(db, cmd);
        break;
      }

      case DEL:
      {
        break;
      }

      case LS:
      {
        ldb::range(db, key_start, key_end);
        break;
      }

      case START:
      {
        cout << "START set to " << cmd.rest << endl;
        key_start = cmd.rest;
        break;
      }

      case END:
      {
        cout << "END set to " << cmd.rest << endl;
        key_end = cmd.rest;
        break;
      }

      case LIMIT:
      {
        string msg = "LIMIT set to ";

        if (cmd.rest.length() == 0) {
          cout << msg << key_limit << endl;
          break;
        }
        cout << msg << cmd.rest << endl;
        key_limit = atoi(cmd.rest.c_str());
        break;
      }

      default: {
        cout << l << endl;
      }
    }
  }

   delete db;
}


namespace ldb {

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
  void range(leveldb::DB* db, string key_start, string key_end) 
  {
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

      cout << key.ToString() << endl;
    }
    delete itr;
  }

  //
  //
  //
  command parse_cmd(string line, vector<Commands> cmds) 
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

