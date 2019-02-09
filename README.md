# RPS

A program that simulates Rock-Paper-Scissors tournaments.

This program is written exclusively in C99.

## Usage

Compile on command line with ``gcc -std=c99 RPS.c RPS_champion.c -o RPS`` and run with ``./RPS``.

## Credit

* RoaryStar
  * Opened repository and set program requirements
  * Wrote the "champion" AI player

## Features

* One "champion" AI player, whose development started this entire project

## To-do

* A terminal interface with commands for the following features:
  * Creating an AI player
  * Playing a game of Rock-Paper-Scissors, user against AI
  * Playing a game of Rock-Paper-Scissors, AI against AI
  * Conducting a tournament between two AI players
  * Conducting a tournament between multiple AI players
  * (Stretch) Conducting an evolutionary tournament between multiple AI player types
* Numerous types of AI players:
  * Fully-random player
  * Human-random player
  * Skewed random player
  * Deterministic n-cycle player
  * Deterministic 1-cycle responsive player
  * (Stretch) Various learning players
* (Stretch) An ncurses interface with support for the above features, as well as
  * A graphical user interface
  * Visualization support for AI player decision-making
  * Statistics visualizations over games and tournaments

