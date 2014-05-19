#include "ldb.h"

namespace ldb {
  struct Options : public leveldb::Options {
    string path;
  };

  leveldb::DB* create_database(int argc, char** argv);
  Options get_options(int argc, char** argv);
}

//
//
//
int main(int argc, char** argv)
{
  leveldb::DB* db = ldb::create_database(argc, argv);
  ldb::start_repl(db);
}

//
//
//
leveldb::DB* ldb::create_database(int argc, char** argv)
{
  leveldb::DB* db;
  ldb::Options options = ldb::get_options(argc, argv);
  leveldb::Status status = leveldb::DB::Open(options, options.path, &db);

  if (false == status.ok()) {
    cerr << "Unable to open/create database " << options.path << endl;
    cerr << status.ToString() << endl;
    exit(1);
  }

  return db;
}

//
//
//
ldb::Options ldb::get_options(int argc, char** argv)
{
  ldb::Options options;

  if (argc == 2) options.path = argv[1];

  int c;

  while ((c = getopt(argc, argv, "i:ec:d")) != EOF) {
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
