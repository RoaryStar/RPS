#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#inlcude "RPS.h"

int main()
{
}

int _wins_against (toss_t p1, toss_t p2)
{
    return (3+p2-p1)%3 == 1;
}

g_res winner (toss_t p1, toss_t p2)
{
    if (_wins_against (p1, p2)) return G_PONE;
    if (_wins_against (p2, p1)) return G_PTWO;
    return G_TIE;
}

