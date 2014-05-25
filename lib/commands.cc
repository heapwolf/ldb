#include "../ldb.h"

string hi_start = "\033[1;31m";
string hi_end = "\033[0m";

//
//
//
bool replace(std::string& str, const std::string& from, const std::string& to) {
  size_t start_pos = str.find(from);
  if (start_pos == std::string::npos) return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

//
//
//
void ldb::put_value(string key, string value)
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
void ldb::get_value(string key)
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
void ldb::del_value(string key)
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
void ldb::find(string exp, int type)
{
  cout << "\r\n";
  regex e(exp);

  leveldb::Iterator* itr = db->NewIterator(leveldb::ReadOptions());

  for (itr->Seek(key_start); itr->Valid(); itr->Next()) {
    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    string sKey = key.ToString();

    if (sKey == key_end) break;

    string sValue = value.ToString();
    string subject = type == 0 ? sKey : sValue;
    string output;
    smatch m;

    regex_search(subject, m, e);

    if (m.length()) {

      string s = subject.substr(m.position(), m.length());
      replace(subject, s, hi_start + s + hi_end);

      if (type == 1) { // fancy truncate value output

        int padding = hi_start.length() + hi_end.length() + 20;
        int substart = m.position() - padding;
        int matchlen = m.length() + padding * 2;
        int subend = matchlen;

        if (substart <= 0) substart = 0;
        if (subend >= subject.length()) subend = subject.length();

        string partial = subject.substr(substart, matchlen);
        output = sKey + ": ..." + partial + "...\r\n";
      }
      else {
        output = subject + "\r\n";
      }
      cout << output << endl;
    }
  }

  cout << "\r\n";
  delete itr;
}

//
//
//
void ldb::range(string prefix, bool surpress_output)
{
  cout << "\r\n";
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

    string sKey = key.ToString();

    if (sKey == key_end) break;

    int len = sKey.length();
    if (len > maxWidth) {
      maxWidth = len;
    }
  }

  maxWidth += padding;
  maxColumns = term.ws_col / maxWidth;
  int colSize = maxWidth + hi_start.length() + hi_end.length();

  for (itr->Seek(key_start); itr->Valid(); itr->Next()) {

    leveldb::Slice key = itr->key();
    leveldb::Slice value = itr->value();

    string sKey = key.ToString();

    if (sKey == key_end) break;

    ldb::key_cache.push_back(sKey);

    if (surpress_output == false) {
      count++;

      if (count == key_limit + 1) break;

      int replaced = replace(sKey, prefix, hi_start + prefix + hi_end);

      cout << setw(replaced ? colSize : maxWidth) << left << sKey;

      if (count == maxColumns - 1) {
        count = 0;
        cout << "\r\n";
      }
    }
  }

  cout << "\r\n";
  delete itr;
}

//
//
//
void ldb::get_size()
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

