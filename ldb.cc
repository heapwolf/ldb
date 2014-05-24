#include "ldb.h"
#include "deps/cmd-parser/include/parser.hh"

//
// start and end values define the current range, by 
// default, empty values will yield all values until
// the key_limit is reached.
//
string ldb::key_end = "";
string ldb::key_start = "";

//
// contains all of the keys so that they can be
// passed into the linenoise module for adhoc completion.
//
vector<string> ldb::key_cache;

//
// the total number of keys that can be listed
// in the current range as defined by the values
// in key_start and key_end.
//
int ldb::key_limit = 1000;

//
// the database instance
//
leveldb::DB* ldb::db;

//
// main determines if the REPL should be launched or
// if the user wants to perform a cli command.
//
int main(int argc, const char** argv)
{
  bool interactive = false;
  leveldb::Options options;
  optionparser::parser p;

  p.add_option("--interactive", "-i")
   .help("interactive mode (REPL)")
   .mode(optionparser::store_value);

  p.add_option("--get", "-g")
   .help("a string that represents a key")
   .mode(optionparser::store_value);

  p.add_option("--keys", "-k")
   .help("list the keys in the current range")
   .mode(optionparser::store_true);

  p.add_option("--start", "-s")
   .help("specify the start of the current range")
   .mode(optionparser::store_value);

  p.add_option("--limit", "-l")
   .help("limit the number of keys in the current range")
   .mode(optionparser::store_value);

  p.add_option("--end", "-e")
   .help("specify the end of the current range")
   .mode(optionparser::store_value);

  p.add_option("--put", "-p")
   .help("put a key and value pair into the database (requires --value <string>)")
   .mode(optionparser::store_value);

  p.add_option("--del", "-d")
   .help("delete a key and value from the database")
   .mode(optionparser::store_value);

  p.add_option("--value", "-v")
   .help("put a key and value into the database (requires --put <string>)")
   .mode(optionparser::store_value);

  p.add_option("--create", "-c")
   .help("create the database if one does not exist")
   .mode(optionparser::store_true);

  p.add_option("--error")
   .help("throw an error if the databse does not exist")
   .mode(optionparser::store_true);

  p.add_option("--size")
   .help("get the size of the current range")
   .mode(optionparser::store_true);

  string path;
  p.eat_arguments(argc, argv);

  if (p.get_value("interactive")) {
    interactive = true;
    path = p.get_value<string>("interactive");
  }
  else if (!argv[1] || argv[1][0] == '-') {
    cerr << "A path is required" << endl;
    return -1;
  }
  else {
    path = argv[1];
  }

  if (p.get_value("create")) {
    options.create_if_missing = true;
  }
  if (p.get_value("error")) {
    options.error_if_exists = true;
  }

  leveldb::Status status = leveldb::DB::Open(options, path, &ldb::db);

  if (!status.ok()) {
    cerr << status.ToString() << endl;
    return -1;
  }

  if (interactive) {
    ldb::startREPL();
    delete ldb::db;
    return 0;
  }

  if (p.get_value("start")) {
    ldb::key_start = p.get_value<string>("start");
  }

  if (p.get_value("end")) {
    ldb::key_end = p.get_value<string>("end");
  }

  if (p.get_value("limit")) {
    ldb::key_limit = p.get_value<int>("limit");
  }

  if (p.get_value("get")) {
    ldb::get_value(p.get_value<string>("get"));
  }
  else if (p.get_value("put") && p.get_value("value")) {

    string key = p.get_value<string>("put");
    string value = p.get_value<string>("value");
    ldb::put_value(key, value);
  }
  else if (p.get_value("del")) {
    ldb::del_value(p.get_value<string>("del"));
  }
  else if (p.get_value("keys")) {
    
    ldb::range("", false);
  }
  else if (p.get_value("size")) {
    ldb::get_size();
  }

  delete ldb::db;
  return 0;
}

