#include "ldb.h"
#include "deps/docopt/docopt.h"

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
// don't try to print JSON
//
int ldb::json = 0;

//
// colors
//
string ldb::COLOR_RED = "\033[1;31m";
string ldb::COLOR_BLUE = "\033[1;34m";
string ldb::COLOR_YELLOW = "\033[1;33m";
string ldb::COLOR_GRAY = "\033[1;90m";
string ldb::COLOR_NONE = "\033[0m";

string ldb::hi_start = COLOR_BLUE;
string ldb::hi_end = COLOR_NONE;

//
// command line options
//
static const char USAGE[] =
R"(ldb

    Usage:
      ldb <path> [--create] [--error] [--size] [--nocompress]
      ldb <path> (del|get|getbin) <key>
      ldb <path> put <key> <value> [--nocompress]
      ldb <path> keys [--limit=<n>] [--lower=<lower>] [--upper=<upper>]
      ldb (-h | --help)
      ldb --version

    Options:
      -h --help        Show this screen.
      --create         Create the database if it does not exist.
      --error          Throw an error if the databse does not exist.
      --size           Get the size of the current range.
      --limit=<n>      Limit the number of records in the current range.
      --lower=<lower>  The lower bound value to end the range.
      --upper=<upper>  The upper bound value to start the range.
      --nocompress     Do not use compression.
      --version        Show version.
)";

//
// main determines if the REPL should be launched or
// if the user wants to perform a cli command.
//
int main(int argc, const char** argv)
{
  bool interactive = true;
  leveldb::Options options;
  options.compression = leveldb::kSnappyCompression;

  std::map<std::string, docopt::value> args = docopt::docopt(
    USAGE,
    { argv + 1, argv + argc },
    true,
    "ldb 2.1.0"
  );

  //for(auto const& arg : args) {
  //  std::cout << arg.first <<  arg.second << std::endl;
  //}

  if (args["--create"] && args["--create"].asBool()) {
    options.create_if_missing = true;
  }

  if (args["--error"] && args["--error"].asBool()) {
    options.error_if_exists = true;
  }

  if (args["--nocompress"] && args["--nocompress"].asBool()) {
    options.compression = leveldb::kNoCompression;
  }

  if (args["del"].asBool() ||
      args["put"].asBool() ||
      args["get"].asBool() ||
      args["getbin"].asBool() ||
      args["keys"].asBool() ||
      args["--size"].asBool()) {
    interactive = false;
  }

  string path = args["<path>"].asString();

  leveldb::Status status = leveldb::DB::Open(options, path, &ldb::db);

  if (!status.ok()) {
    cerr << status.ToString() << endl;
    return 1;
  }

  if (interactive) {
    // try to print JSON
    ldb::json = 2;
    ldb::startREPL();
    return 0;
  }

  if (args["--lower"]) {
    ldb::key_start = args["--lower"].asString();
  }

  if (args["--upper"]) {
    ldb::key_end = args["--upper"].asString();
  }

  if (args["--limit"] && args["--limit"].asLong()) {
    ldb::key_limit = args["--limit"].asLong();
  }

  if (args["--size"] && args["--size"].asBool()) {
    ldb::get_size();
    return 0;
  }

  if (args["get"] && args["get"].asBool()) {
    ldb::get_value(args["<key>"].asString());
  }
  else if (args["getbin"] && args["getbin"].asBool()) {
    ldb::get_value_bin(args["<key>"].asString());
  }
  else if (args["put"] && args["put"].asBool()) {

    string key = args["<key>"].asString();
    string value = args["<value>"].asString();

    if (key.size() == 0 || value.size() == 0) {
      return 1;
    }
    ldb::put_value(key, value);
  }
  else if (args["del"] && args["del"].asBool()) {
    ldb::del_value(args["<key>"].asString());
  }
  else if (args["keys"] && args["keys"].asBool()) {
    ldb::range("", false);
  }

  return 0;
}
