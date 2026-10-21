
# Repair service WEB application

That's my coursework for DB&DBMS discipline

There is single-header [httpserver.h](https://github.com/jeremycw/httpserver.h)
from other GitHub dev, and my small superstructure on it.

Static http web with no JavaScript at all. Only my "CMS".
Using XHTML 1.0 Strict and CSS 2.1. Working almost at every
GUI and TUI browser.

Server written on C++ with STD C++98, using no any modern C++
libraries.

## Requirements

* Any C++ compiler (g++ f.e.)
* make
* Linux

For other operating system you need to change defined macro in
backend/http_server.cpp depending on
[httpserver.h](https://github.com/jeremycw/httpserver.h)
specification.

## Compiling

```bash
make
```

## Running
```bash
make run
```

And type localhost:8080 on your browser.

