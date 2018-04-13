![img](/man/logo.png)

# SYNOPSIS
A c++ repl and cli tool for leveldb

# BUILD

## Mac OS X

```cli
$ brew install snappy cmake
$ git clone https://github.com/0x00a/ldb.git
$ make install -C ldb
```

## Linux (Debian / others?)

```cli
$ apt-get install libsnappy-dev cmake
$ git clone https://github.com/0x00a/ldb.git
$ sudo make install
```


# USAGE
Open an existing database with interactive mode (REPL) and create the
database if it does not exist with optional flag `--create`.
```bash
ldb ./testdb --create
```

Use as a commandline interface
```bash
ldb -h
```


## KEY AUTO-COMPLETE
`ldb` will get the first n (as defined in `LIMIT`) records and make them
available for autocomplete. If the limit is redefined, the cache will be
refreshed to reflect the new range.

When possible, hitting tab will allow you to cycle though keys that match
your partial input. Also, if there is a partial match it will be highlighted
in the results.

![img](/man/colors.png)

## COMMANDS
You can `get`, `put` or `del` a single key at any time. Many of the following
commands help you to create `ranges` so that you can casually explore smaller
sets of data. Type `?` to print more help.

Get a key's value
```bash
>get foo
```

Put a value to a key
```bash
>put foo 100
```

List the keys in the curent range
```bash
>ls
```

Find string in keys or values using a regex
```bash
>in keys <regex>
>in values <regex>
```

![img](/man/find.png)

Set the start of the current range
```bash
>start <string>
```

Set the end of the current range
```bash
>end <string>
```

Set the limit for the current range
```bash
>limit <number>
```

Get the size of the the current range (in bytes)
```bash
>size
```

Try to format the output of values as JSON. Use `<number>` is the indent)
and `0` to turn off JSON formatting.

```bash
>json <number>
```

![img](/man/json.png)
