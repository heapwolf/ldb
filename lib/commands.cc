#include "../ldb.h"

//
//
//
void ldb::put_value(leveldb::DB* db, string key, string value)
{
  leveldb::WriteOptions writeOptions;
  leveldb::Status status;
  status = db->Put(writeOptions, key, value);
  if (!status.ok()) {
    cerr << status.ToString() << endl;
  }
}

//
//
//
void ldb::get_value(leveldb::DB* db, string key)
{
  string value;
  leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &value);

  if (!status.ok()) {
    cerr << status.ToString() << key << endl;
  }
  else {
    cout << value << endl;
  }
}

//
//
//
void ldb::del_value(leveldb::DB* db, string key)
{
  leveldb::WriteOptions writeOptions;
  leveldb::Status status;

  status = db->Delete(leveldb::WriteOptions(), key);
  if (!status.ok()) {
    cerr << status.ToString() << endl;
  }
}

//
//
//
void ldb::range(leveldb::DB* db, bool surpress_output)
{
  struct winsize term;
  ioctl(0, TIOCGWINSZ, &term);

  key_cache.clear();

  int count = 0;
  int maxWidth = 0;
  int maxColumns = 0;
  int padding = 2;

  leveldb::Iterator* itr = db->NewIterator(leveldb::ReadOptions());

  for (itr->Seek(key_start); itr->Valid(); itr->Next()) {
    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    int len = key.ToString().length();
    if (len > maxWidth) {
      maxWidth = len;
    }
  }

  maxWidth += padding;
  maxColumns = term.ws_col / maxWidth;

  for (itr->Seek(key_start); itr->Valid(); itr->Next()) {

    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    string sKey = key.ToString();

    if (sKey == key_end) break;

    ldb::key_cache.push_back(sKey);

    if (surpress_output == false) {
      count++;

      if (count == key_limit + 1) break;

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
void ldb::get_size(leveldb::DB* db)
{
  leveldb::Range ranges[1]; // we only keep one range, maybe allow more?
  ranges[0] = leveldb::Range(key_start, key_end);
  uint64_t sizes[1];

  // may be a bug in the docs, GetApproximateSizes returns void, not Status.
  db->GetApproximateSizes(ranges, 1, sizes);
  if (sizes[0]) {
    cout << sizes[0];
  }
}

