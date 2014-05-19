#ifndef _LDB_H_
#define _LDB_H_

#include <iostream>
#include "leveldb/db.h"

namespace ldb {
  void start_repl(leveldb::DB* db);
}

#endif // _LDB_H_

