#include "RPS.h"

#include <math.h>
#include <stdlib.h>

// A history record. Can store a single
// floating-point value (0) or a
// recursive array. The value of n indicates this.
typedef struct _hr
{
    int n;
    union
    {
        double d;
        struct _hr* inner;
    };
} histrecord_t;

// The champion. This encapsulates all the
// information the champion requires and
// also allows for some customization of parameters.
typedef struct
{
    int hist_len;
    toss_t * history;
    int h_index;
    histrecord_t coeffs;
} p_champion_t;

// info : probability -> information
// Takes the chance of an event p and results
// the information in bits afforded should the
// event occur.
double info (double p)
{
    if (p == 0.0) return 0.0;
    return -p*log2(p);
}

// entropy : event space -> information
// Takes an n-length array of the chances of
// each event in a partition of the event space
// and gives the information-theoretic entropy
// of the event space
double entropy (double* evs, int n)
{
    double res = 0.0;
    for (int i = 0; i < n; ++i)
        res += info (evs[i]);
    return res;
}

// adj_ent : event space -> vector<R>
// Takes a histrecord h with lwi+1 inner records, the first
// lwi interpreted as an event space and the final as the
// entropy of the event space, and a pointer to a vector of
// appropriate length to be used for storing calculations.
// Adjusts probabilities by entropy; that is, the lower the
// entropy, the higher the confidence, and so the more weight
// applied on that record.
// The heuristic used is engineered to preserve the strict
// ordering of individual records and preserving a strong
// correlation between predictability and weight while
// preventing similarly-predictable (linearly by entropy)
// from being too far apart (as would be the case by a
// basic harmonic mean) after adjustment and more pressingly
// the possibility that a too-certain record could cause
// out-of-bounds errors during further calculation.
// This takes event spaces and weights them up to a factor
// of 10.
double* adj_ent (histrecord_t* h, double* out)
{
#define ASYMPTOT 10
    int lwi = h->n - 1; //last_weight_index
    double pfactor = 1.0/(h->inner[lwi].d * h->inner[lwi].d);
    double factor = ASYMPTOT -
                        ASYMPTOT * exp(-pfactor/ASYMPTOT);

    for (int i = 0; i < lwi; ++i)
        out[i] = factor * h->inner[i].d;
    return out;
}

// aggregate : history, recursive conditional sample
//             spaces -> vector<R>[3]
// Takes:
// * hist : {rock, paper, scissors}^n. A list of the previous
//              play history;
// * h : (R^4)^n. Histrecords detailing the sample spaces
//              corresponding to all possible histories of
//              {rock, paper, scissors, ignore}^n;
// * res : R^3. External calls must call with {0.0, 0.0, 0.0}
//              for correct results;
// * work : R^3. Dummy array to be passed to adj_ent;
// * n : N. Length of hist;
// and recursively iterates over all the records with
// relevant history, adding the entropy-weight-adjusted
// probabilities to res and returning it.
double* aggregate (toss_t* hist,
                   histrecord_t* h,
                   double* res,
                   double* work,
                   int n)
{
    if (n == 0)
    {
        adj_ent(h, work);
        for (int i = 0; i < 3; ++i)
            res[i] += work[i];
    }
    else
    {
        if (hist[0] > -1)
            aggregate (hist + 1, h->inner + hist[0], res, work, n-1);
        aggregate (hist + 1, h->inner + 3, res, work, n-1);
    }

    return res;
}

// make_hr_recurse : N^2, R -> R^(n*d)
// Takes:
// * h : space to store the result;
// * n : number of recursive branches;
// * d : remaining recursive depth;
// * s : seed value in the leaves;
// and makes a recursive histrecord.
histrecord_t* make_hr_recurse (histrecord_t* h,
                             int n,
                             int d,
                             double s)
{
    if (d > 0)
    {
        h->n = n;
        h->inner = (histrecord_t*) malloc (sizeof (histrecord_t)
                                         * n);
        for (int i = 0; i < n; ++i)
            make_hr_recurse (h->inner + i,
                             n,
                             d-1,
                             s);
    }
    else
    {
        h->n = 0;
        h->d = s;
    }
    return h;
}

// force_entropy_calc : histrecord -> histrecord
// Takes histrecord h and its depth d and updates
// all entries to have the correct entropy value.
histrecord_t* force_entropy_calc (histrecord_t* h, int d)
{
    if (d == 1)
    {
        for (int i = 0; i < h->n - 1; ++i)
            wrkbuffer[i] = h->inner[i].d;
        h->inner[h->n - 1].d = entropy (wrkbuffer, h->n - 1);
    }
    else
    {
        for (int i = 0; i < h->n - 1; ++i)
            force_entropy_calc (h->inner + i, d - 1);
    }
    return h;
}

// make_champion : N -> p_champion
// Takes space p to store the result and n the number
// of moves to look back on and returns a properly-
// instantiated chamption RPS AI.
p_champion_t* make_champion (p_champion_t* p, int n)
{
    p->hist_len = n * 2;
    p->history = (int*) malloc (sizeof (toss_t) * n * 4);
    p->h_index = 0;
    for (int i = 0; i < n * 2; ++i)
        p->history[i] = -1;
    make_hr_recurse (&(p->coeffs), 4, n * 2, 1.0/3.0);
}

// normalize_L1 : vector<R> -> vector<R>
// Takes an array of double interpreted as a vector
// and normalizes it by the L-1 norm so the sum
// of the values are 1. This allows v to be
// interpreted as a probability distribution.
double* normalize_L1 (double* v, int n)
{
    double tot = 0.0;

    for (int i = 0; i < n; ++i)
        tot += abs(v[i]);

    if (tot == 0.0) return v;

    for (int i = 0; i < n; ++i)
        v[i] /= tot;

    return v;
}

// temperature : information -> R
// Takes entropy and heuristically comes up with a temperature
// adjust the event space by so that at high entropy the
// champion plays to learn and at low entropy the champion
// plays to win.
// The temperature function must be decreasing on entropy.
// Above 1, probabilities are magnified; below 1, they
// are dulled.
double temperature (double entropy)
{
    return 1.0 + 2 * (1.0 / entropy - 1.0);
}

// adj_temp : vector<R>, information -> vector<R>
// Adjusts the values in the vector arr of length n
// by the given entropy according to the temperature
// metric above.
double* adj_temp (double* arr, int n, double entropy)
{
    double t = temperature (entropy);
    for (int i = 0; i < n; ++i)
        arr[i] = pow (arr[i], t);
    return arr;
}

// next_throw_champion : champion -> toss
// Takes a champion c and, based on the information it has,
// chooses one of {ROCK, PAPER, SCISSORS} to throw for its
// next move.
toss_t next_throw_champion (p_champion_t* c)
{
    double weights[3] = {0.0, 0.0, 0.0};
    double work[3];

    aggregate (c->history + c->h_index,
               &(c->coeffs), weights, work, c->hist_len);

    normalize_L1 (weights, 3);

    adj_temp(weights, 3, entropy (weights, 3));

    normalize_L1 (weights, 3);

    double choice = (double) rand() / (double) RAND_MAX;

    if ((choice -= weights[0]) < 0.0) return ROCK;
    if ((choice -= weights[1]) < 0.0) return PAPER;
    return SCISSORS;
}

toss_t choose_champion_player (pdata_t d)
{
    next_throw_champion ((p_champion_t*) d);
}

histrecord_t* update_rec (histrecord_t* rec,
                          toss_t op_play,
                          double mult)
{
    for (int i = 0; i < 3; ++i)
    {
        switch (winner (i, op_play))
        {
        case G_PONE:
            rec->inner[i].d =
                ((mult - 1.0) * rec->inner[i].d + 1.0) / mult;
            break;
        case G_PTWO:
            rec->inner[i].d =
                ((mult - 1.0) * rec->inner[i].d + 0.0) / mult;
            break;
        case G_TIE:
            rec->inner[i].d =
                ((mult - 1.0) * rec->inner[1].d + 1.0/3.0) / mult;
            break;
        }
    }

    wrkbuffer[0] = rec->inner[0].d;
    wrkbuffer[1] = rec->inner[1].d;
    wrkbuffer[2] = rec->inner[2].d;

    normalize_L1(wrkbuffer, 3);

    rec->inner[3].d = entropy(wrkbuffer, 3);

    return rec;
}

histrecord_t* rec_update (histrecord_t* rec,
                          toss_t* hist,
                          toss_t op_play,
                          double mult,
                          int d)
{
    if (d == 1)
        update_rec (rec, op_play, 81.0 / mult);
    else
    {
        if (hist[0] > -1)
            rec_update (rec->inner + hist[0], hist + 1, op_play, mult * sqrt (3), d - 1);
        rec_update (rec->inner + 3, hist + 1, op_play, mult, d - 1);
    }
    return rec;
}

void learn_champion_player (pdata_t p, toss_t mp, toss_t op)
{
    p_champion_t* c = (p_champion_t*) p;
    rec_update(&c->coeffs,
               c->history + c->h_index,
               op, 1.0, c->hist_len);
    c->h_index = (c->h_index + 2) % c->hist_len;
    c->history[c->hist_len - 2] = op;
    c->history[c->hist_len - 1] = mp;
}

void destory_hr_recurse (histrecord_t* rec)
{
    if (rec->n > 0)
	for (int i = 0; i < rec->n; ++i)
	    destroy_hr_recurse (rec->inner + i);
    free (rec);
}
void destory_champion_player (pdata p)
{
    p_champion_t* data = (p_champion_t*) p;
    
    free (data->history);
    for (int i = 0; i < data->coeffs.n; ++i)
	destroy_hr_recurse (data->coeffs.inner + i);
}

player_t* make_champion_player (player_t* p, int argc, char** args)
{
    int n = 3;
    if (argc > 1 && 0 == strcmp (args[0], "-lkbk"))
    {
	n = strtoi (args[1], -1);
	
	if (n < 0)
	{
	    post_error ("Invalid argument for lookback value; "
			    "must be a nonnegative integer.\n");
	    return NULL;
	}
    }
    else if (argc > 0)
    {
	post_error ("Invalid argument for make_champion_player; "
			"only accepts -lkbk [n] for nonnegative [n].\n");
	return NULL;
    }

    p_champion_t* data = (p_champion_t*) malloc (sizeof (p_champion_t));
    make_champion (data, n);

    p->data = data;
    p->choose_f = choose_champion_player;
    p->learn_f = update_champion_player;
    p->destroy_f = destroy_champion_player;
}
