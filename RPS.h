#ifndef RPS_H
#define RPS_H

double wrkbuffer [1024];

#define ROCK        0
#define PAPER       1
#define SCISSORS    2

#define G_PONE      '1'
#define G_PTWO      '2'
#define G_TIE       't'

typedef char gres_t;     // one of G_PONE, G_PTWO, G_TIE
typedef int toss_t;      // one of ROCK, PAPER, or SCISSORS

// winner : toss, toss -> g_res
// Results the winer in a game where the lhs player
// throws p1 and the rhs player throws p2.
gres_t winner (toss_t p1, toss_t p2);

typedef void* pdata_t;

// data encapsulation for a player
typedef struct
{
    // stored data (histories etc.)
    pdata_t data;

    // choose_f : pdata -> toss
    toss_t (*choose_f) (pdata_t);

    // learn_f : pdata, toss, toss | updates pdata
    void (*learn_f) (pdata_t, toss_t, toss_t);
} player_t;

player_t* make_champion_player (player_t* p, int n);

#endif
