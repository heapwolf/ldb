ldb(1) -- leveldb instance manager.
=================================

## SYNOPSIS

  `ldb` pathname \[options\]

## DESCRIPTION
  **ldb** allows you to explore, manage and create leveldb instances from a
  repl, the commandline or a script.

## OPTIONS

  `-h --help`
  Show this screen.

  `--create`
  Create the database if it does not exist.

  `--error`
  Throw an error if the databse does not exist.

  `--size`
  Get the size of the current range.

  `--nocompress`
  Do not use compression.

  `--version`
  Show version.

## EXAMPLES

  Open an existing database with interactive mode (REPL) and create the
  database if it does not exist with optional flag `--create`:

  ```
  $ ldb ./testdb --create
  ```

  Insert a key/value pair:
  ```
  $ ldb ./db put foo bar
  ```

  Get a value:
  ```
  $ ldb ./db get foo
  ```

  Delete a value:
  ```
  $ ldb ./db del foo
  ```

  List all keys in the database:
  ```
  $ ldb ./db keys
  ```

  List the ten first keys in the database:
  ```
  $ ldb ./db keys --limit=10
  ```

  List all keys starting with `foo`:
  ```
  $ ldb ./db keys --start=foo
  ```

  List all keys up to (not including) `bar`:
  ```
  $ ldb ./db keys --end=bar
  ```

  Obviously you can do combinations:
  ```
  $ ldb ./db keys --start=foo --end=bar --limit=5
  ```

## KEY AUTO-COMPLETE

  `ldb` will get the first n (as defined in `LIMIT`) records and make them available
  for autocomplete. If the limit is redefined, the cache will be refreshed to reflect the
  new range.

  When possible, hitting tab will allow you to cycle though keys that match your partial
  input. Also, if there is a partial match it will be highlighted in the results.

## REPL COMMANDS

  You can `get`, `put` or `del` a single key at any time. Many of the following commands
  help you to create ranges so that you can casually explore smaller sets of data.

  Get a key's value:

  ```
  >get foo
  ```

  Put a value to a key:

  ```
  >put foo 100
  ```

  List the keys in the curent range:

  ```
  >ls
  ```

  Set the start of the current range:

  ```
  >start <string>
  ```

  Set the end of the current range:

  ```
  >end <string>
  ```

  Set the limit for the current range:

  ```
  >limit <number>
  ```

  Get the size of the the current range (in bytes):

  ```
  >size
  ```


## AUTHOR

  * Paolo Fragomeni <<paolo@async.ly>>
  * Lars-Magnus Skog <<ralphtheninja@riseup.net>>
  * Joseph Werle <<joseph.werle@gmail.com>>

## REPORTING BUGS

  * <https://github.com/0x00a/ldb/issues>

## SEE ALSO

  * <https://github.com/google/leveldb>
  * <https://github.com/0x00a/lev>
  * <https://github.com/antirez/linenoise>
  * <https://github.com/docopt/docopt.cpp>

## LICENSE

Distributed with the MIT license.

