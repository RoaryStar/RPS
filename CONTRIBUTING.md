# (GC-D) Contribution Guidelines and Documentation

Here, contributors can find guidelines and standards for their work, and prospective readers of this program can find details on specifications and rationale for this program's structure and details.

_This file is currently under construction._

## 1 Contribution Guidelines

This program is written in a specific format in the interests of modularity and maintainability. As such, certain strict, enforced guidelines and recommended practices shall be imposed, as listed here.

### 1.1 Front-end Documentation

This section concerns all work on any general-purpose front-end **documentation**; that is, the writing and revision of files like this file. 

1. All documentation _must_ be committed to the `/backdev` branch, which, pending review, will then be merged (published) to the main branch.
3. All documentation files shall begin with title consisting of an abbreviation for the file name in parenthesis followed by the file name.
2. All documentation files shall be sectioned for the purposes of quick referencing.
   For example, (GC-D 1.4.2) refers to the statement "An AI player named _name_ (hereby referred to as [name]) shall be created in its own encapsulated C source file with the filename `RPS_[name].c` in the root folder `RPS/`." in this document.
3. With the exception of `README.md`, all documentation files shall end with a single top-level section marked "Credit" listing, in chronological order,
   * the date and author of the creation of the document,
   * the date and author(s) of the first published version of the document, and
   * the date, author, and sections affected of any subsequent revisions of the document.
   
   The _Credit_ section may additionally list any other notable contributions.

### 1.2 In-code Documentation

This section concerns all work on any general-purpose **in-code documentation**; that is, the writing and revision of comments in C header and sources files that are _not_ encapsulating files to AI players, the terminal interface, or the ncurses interface.

1. All documentation _must_ be committed to the `/backdev` branch, which, pending review, will then be merged (published) to the main branch.
2. All documentation on a single function or data structure definition shall be preceded with a single-line comment consisting only of the username(s) of the contributor(s) to the writing of the code, followed in parenthesis by the username(s) of the contributor(s) to the writing of the documentation.
   For example, if Alice and Bob wrote the function `func()` and Alice and Carol wrote the documentation for `func()`, then immediately preceding the documentation to `func()` there shall be the line `// Alice, Bob (Alice, Carol)`.

### 1.3 General Development

This section concerns all work on anything that is _not_ an AI player, the terminal interface, or the `ncurses` interface. This includes, for instance, **general utility** structures and functions (eg. data strutures) and **interfacing functions** between parts of the program (eg. `post_error()`).

1. All general work _must_ be committed to the `/backdev` branch, which, pending reveiew, will then be merged (published) to the main branch.


### AI Players

All direct work on AI players _must_ be committed to the `/backdev` branch, which, pending review. will then be merged (published) to the main branch.

1. An AI player named _name_ (hereby referred to as [name]) shall be created in its own encapsulated C source file with the filename `RPS_[name].c` in the root folder `RPS/`.
2. The encapsulated C source file shall begin with a multi-line comment block with the following information:
   1. Author(s)
   2. Date first published (or _unpublished_ if not yet published)
   3. Date of the last published revision, if applicable
   4. Name of the AI player
   5. Basic algorithmic structure of the AI player
   6. Any notable references or notes
3. The encapsulated C source file shall `#include "RPS.h"` and have the following functions as specified in (4) to (7):
   * `player_t* make_[name]_player (player_t* p, int argc, char** args)`
   * `void destroy_[name]_player (pdata_t p)`
   * `toss_t choose_[name]_player (pdata_t p)`
   * `void learn_[name]_player (pdata_t p, toss_t m, toss_t o)`
4. The function `player_t* make_[name]_player(player_t* p, int argc, char** args)` shall:
   * parse the arguments in `char** args` (the third argument) and, if invalid, call `post_error()` with an appropriate error message and immediately return `NULL`; 
   * populate an **already-allocated, null-initialized** `player_t p` (the first argument) according to the parameters specified in `char** args` (the second argument) and to the specifications in (8); and
   * return `p` after leaving no side effects.
5. The function `void destroy_[name]_player (pdata_t p)` shall:
   * safely and efficiently deallocate everything stored in `pdata_t p` (the argument), leaving no side effects; and
   * be stored in the `void (*destory_f) (pdata_t)` field in the encapsulating `player_t` during initialization in `make_[name]_player()`.
6. The function `toss_t choose_[name]_player (pdata_t p)` shall
   * return, using data only from `pdata_t p` (the argument), a single choice of `ROCK`, `PAPER`, or `SCISSORS`, leaving no side effects; and
   * be stored in the `toss_t (*choose_f) (pdata_t)` field in the encapsulating `player_t` during initialization in `make_[name]_player()`.
7. The function `void learn_[name]_player (pdata_t p, toss_t m, toss_t o)` shall:
   * update the data in `pdata_t p` (the first argument) given the thrown hand from the player itself `toss_t m` (the second argument) and the thrown hand from the opposing player `toss_t o` (the thrid argument) in the previous round, leaving no other side effects; and
   * be stored in the `void (*learn_f) (pdata_t, toss_t, toss_t)` field in the encapsulating `player_t` during initialization in `make_[name]_player()`.
8. The arguments passed to `make_[name]_player()` as the third argument `char** args` should conform to the following:
   * the length of `args` as an array of `char*` is equal to the second argument `int argc`;
   * the first element of args `args[0]` is a _tag specifier_ where `args[0][0]` is always the character `-`;
   * the element `args[i]` following a _tag specifier_ `args[i-1]`, is either a new, unique _tag specifier_ or an argument to `args[i-1]`;
   * the element `args[i]` following an argument `args[i-1]` to a _tag specifier_ is either a new, unique _tag specifier_ or an argument to the same _tag specifier_ of `args[i]`; and
   * _tag specifiers_ may appear in any order, but the arguments bound to _tag specifiers_ must be after the bound _tag specifier_ and before the succeeding _tag specifier_.
   If `args` fails to conform, `make_[name]_player()` shall call `post_error()` with an appropriate error message and immediately return `NULL`, leaving no side effects.
9. The header file `RPS.h` shall contain a single declaration to `make_[name]_player()` where indicated and shall not contain any other declarations to functions, variables, or other declarations in `RPS_[name].c`.

### 3 Credit

This file was
* created 2019-02-06 by RoaryStar,
* last revised 2019-02-06 by RoaryStar.
