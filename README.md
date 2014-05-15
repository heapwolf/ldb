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
Create a new database
```bash
./ldb -c ./testdb
```

Open an existing database
```bash
./ldb ./testdb
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

# LICENSE

The MIT License (MIT)

Copyright (c) 2014 Paolo Fragomeni

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
