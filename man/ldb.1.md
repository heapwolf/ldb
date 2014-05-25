ldb(1) -- Explore and manage leveldb instances
=================================

## SYNOPSIS

`ldb` \[-h|V\] \[-i\] pathname \[options\]

## DESCRIPTION

  `--help, -h`
      output help information

  `--version, -V`
      prints out current version of ldb

  `--interactive, -i`
      interactive mode (REPL)

  `--get, -g`
      a string that represents a key

  `--put, -p`
      put a key and value pair into the database (requires --value <string>)

  `--value, -v`
      put a key and value into the database (requires --put <string>)

  `--del, -d`
      delete a key and value from the database

  `--keys, -k`
      list the keys in the current range

  `--start, -s`
      specify the start of the current range

  `--end, -e`
      specify the end of the current range

  `--limit, -l`
      limit the number of keys in the current range

  `--create, -c`
      create the database if one does not exist

  `--error`
      throw an error if the databse does not exist

  `--size`
      get the size of the current range


## EXAMPLES

  Open an existing database with interactive mode (REPL) and create the database if it
  does not exist with optional flag `-c`:

  ```
  $ ldb -i ./testdb -c
  ```

  Insert a key/value pair:
  ```
  $ ldb ./db --put foo --value bar
  ```

  Get a value:
  ```
  $ ldb ./db --get foo
  ```

  Delete a value:
  ```
  $ ldb ./db --del foo
  ```

  List all keys in the database:
  ```
  $ ldb ./db --keys
  ```

  List the ten first keys in the database:
  ```
  $ ldb ./db --keys --limit 10
  ```

  List all keys starting with `foo`:
  ```
  $ ldb ./db --keys --start foo
  ```

  List all keys up to (not including) `bar`:
  ```
  $ ldb ./db --keys --end bar
  ```

  Obviously you can do combinations:
  ```
  $ ldb ./db --keys --start foo --end bar --limit 5
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
  >get <keyname>
  ```

  Put a value to a key:

  ```
  >put <keyname>;<value>
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

  * <https://github.com/hij1nx/ldb/issues>

## SEE ALSO

  * <http://code.google.com/p/leveldb/>
  * <https://github.com/hij1nx/lev>
  * <https://github.com/antirez/linenoise>
  * <https://github.com/lukedeo/cmd-parser>

## LICENSE

Distributed with the MIT license.

