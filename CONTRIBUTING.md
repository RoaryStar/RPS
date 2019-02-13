# Contribution Guidelines

This program is written in a specific format in the interests of modularity and maintainability. As such, certain strict, enforced guidelines and recommended practices shall be imposed, as listed here.

## AI Players

All direct work on AI players _must_ be committed to the `/backdev` branch, which, pending review. will then be merged (published) to the main branch.

1. An AI player named _name_ (hereby referred to as [name]) shall be created in its own encapsulated C source file with the filename `RPS_[name].c` in the root folder `RPS/`.
2. The encapsulated C source file shall begin with a multi-line comment block with the following information:
   * Author(s)
   * Date first published (or _unpublished_ if not yet published)
   * Date of the last published revision, if applicable
   * Name of the AI player
   * Basic algorithmic structure of the AI player
   * Any notable references or notes
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
