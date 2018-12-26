📜 Bookwyrm
---

bookwyrm(1) is a ncurses utility for downloading ebooks and other publications that are available on the Internet.
Given some input data via command line options, bookwyrm will search for any matches and present them to you in a text user interface.
Items are found by the help of plugins, that parses sources of these items. Currently, the following sources are queried:
* Library Genesis

Selected items can be viewed for details, printing all bookwyrm knows about the item. When it is implemented,
additional details will be fetched from some database (unless the source itself holds enough data to satisfy),
such as the Open Library or WorldCat.

Both bookwyrm and plugins may print logs during run-time. These can be viewed by pressing TAB.
All unread logs are printed to stdout upon program termination.

For example, one might run bookwyrm as follows:

    bookwyrm --author "Naomi Novik" --series Temeraire --year >=2005 .

[![asciicast](https://asciinema.org/a/6uHcYEo2ERS6nSWzNbzjYCtNt.png)](https://asciinema.org/a/6uHcYEo2ERS6nSWzNbzjYCtNt)

# Dependencies

Aside from a C++17-compliant compiler and CMake 3.4.3, bookwyrm depends on a few libraries:
* [fmtlib](http://fmtlib.net/latest/index.html), for a lot of string formatting;
* **ncurses**, for the TUI;
* **[pybind11](https://github.com/pybind/pybind11)**, for Python plugins, and
* **[fuzzywuzzy](https://github.com/Tmplt/fuzzywuzzy)**, for fuzzily matching found items with what's wanted.
* **Python 3**, for Python plugin support.
* **libcurl**, for downloading items over HTTP.

All libraries that do not use a bold font are non-essential and may be subject to removal later in development.
Some dependencies are submodules in `lib/`; external C++ dependencies are **ncurses**, **Python 3** and **libcurl**.

Furthermore, the Python plugins depend on some external packages.
These are listed in [etc/requirements.txt](etc/requirements.txt).

If you're using Nix(OS), all dependencies are declared in [etc/default.nix](etc/default.nix).
A suitable development environment can be created by sourcing [etc/shell.nix](etc/shell.nix) with nix-shell(1).

# Building instructions

```sh
$ git clone git@github.com:Tmplt/bookwyrm.git && cd bookwyrm
$ git submodule update --init --recursive
$ mkdir build && cd build
$ cmake .. && make
```

# Installation instructions

If you're using Arch Linux, bookwyrm is available in the AUR under `bookwyrm-git`.

---

If you have any insights, questions, or wish to contribute,
create a PR/issue on GitHub at <https://github.com/Tmplt/bookwyrm>.
You can also contact me via email at `echo "dG1wbHRAZHJhZ29ucy5yb2Nrcwo=" | base64 -d`.
