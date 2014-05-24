# SYNOPSIS
A c++ repl for leveldb

# INSTALL

```bash
$ clib install hij1nx/ldb
```

# BUILD

```cli
$ git clone https://github.com/hij1nx/ldb.git
$ make install -C ldb
```

# USAGE
Open an existing database with interactive mode (REPL) and create the
database if it does not exist with optional flag `-c`.
```bash
ldb -i ./testdb -c
```

Use as a commandline interface
```bash
ldb -h
```


## KEY AUTO-COMPLETE
`ldb` will get the first n (as defined in `LIMIT`) records and make them
available for autocomplete. If the limit is redefined, the cache will be
refreshed when the `LS` command is used.

## COMMANDS

Get a key's value
```bash
>get <keyname>
```

Put a value to a key
```bash
>put <keyname>;<value>
```

List the keys in the curent range
```bash
>ls
```

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

# AUTOMATIC SUGGESTIONS
When possible, hitting tab will allow you to cycle though keys that match your partial imput.
Also, the partial match will be highlighted in the results above the current line.

![img](/man/colors.png)

