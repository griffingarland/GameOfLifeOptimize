/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS 4

typedef struct {
 int nrows;
 int ncols;
 int gens_max;
 int thread_id;
} Args;

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} 

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

char* outboard; 
char* inboard;

int LDA;

pthread_t threads[NUM_THREADS];
sem_t sem_start[NUM_THREADS];
sem_t sem_finish[NUM_THREADS];

void golWorker(void * arg)
{
    Args *args = (Args *) arg; 
    int tid = args->thread_id;
    int nrows = args->nrows;
    int ncols = args->ncols;
    int gen_max = args->gens_max;
    int i,j;
    int localgen;
    int row_start = tid*nrows/NUM_THREADS;
    int row_end = row_start + nrows/NUM_THREADS;

    for (localgen = 0; localgen < gen_max; localgen++)
    {
        sem_wait(&sem_start[tid]);

        for (i = row_start; i < row_end; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));
            }
        }

        sem_post(&sem_finish[tid]);
    }

    free(arg);
}

void golWorker2(void * arg)
{
    Args *args = (Args *) arg; 
    int tid = args->thread_id;
    int nrows = args->nrows;
    int ncols = args->ncols;
    int gen_max = args->gens_max;
    int i,j;
    int localgen;
    int row_start = tid*nrows/NUM_THREADS;
    int row_end = row_start + nrows/NUM_THREADS;


        for (i = row_start; i < row_end; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast) + 
                    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));
            }
        }

    free(arg);
}

char*
sequential_game_of_life (char* outboard_, 
        char* inboard_,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    outboard = outboard_;
    inboard = inboard_;
    
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    LDA = nrows;
    int i;
/*
    for(i = 0; i < NUM_THREADS; i++)
    {
        Args *args = (Args*)malloc(sizeof(Args));
        args->thread_id = i;
        args->nrows = nrows;
        args->ncols = ncols;
        args->gens_max = gens_max;

        pthread_create(&threads[i], NULL, golWorker,(void*) args);
        sem_init(&sem_start[i], 0, 1);
        sem_init(&sem_finish[i], 0, 0);
    }
*/
    int j;
    for(i = 0; i < gens_max; i++)
    {
      /*
        sem_wait(&sem_finish[0]);
        sem_wait(&sem_finish[1]);
        sem_wait(&sem_finish[2]);
        sem_wait(&sem_finish[3]);

        SWAP_BOARDS( outboard, inboard );
        
        sem_post(&sem_start[0]);
        sem_post(&sem_start[1]);
        sem_post(&sem_start[2]);
        sem_post(&sem_start[3]);
        */

    for(j = 0; j < NUM_THREADS; j++)
    {
        Args *args = (Args*)malloc(sizeof(Args));
        args->thread_id = j;
        args->nrows = nrows;
        args->ncols = ncols;
        args->gens_max = gens_max;

        pthread_create(&threads[j], NULL, golWorker2,(void*) args);
    }

    for (j = 0; j < NUM_THREADS; j++)
    {
      pthread_join(threads[j], NULL);
    }

        SWAP_BOARDS( outboard, inboard );
    }

        /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}


