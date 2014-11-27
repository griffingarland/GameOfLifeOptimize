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

char* outboard; 
char* inboard;
int nrows;
int ncols;
int gens_max;

int LDA;
int curgen;

pthread_t *threads; 
sem_t sem[NUM_THREADS];
/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

void golWorker(void * arg)
{
    int row = *((int*) arg);
    int i,j;
    for (curgen = 0; curgen < gens_max; curgen++)
    {
        sem_wait(&sem[row]);
        /* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
        for (i = 0; i < nrows/4+row*nrows; i++)
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
        sem_post(&sem[row]);
    }
}


    char*
sequential_game_of_life (char* outboard_, 
        char* inboard_,
        const int nrows_,
        const int ncols_,
        const int gens_max_)
{
    outboard = outboard_;
    inboard= inboard_;
    nrows =  nrows_;
    ncols = ncols_;
    gens_max = gens_max_;
    
    sem_init(&sem[0],0,1);
    sem_init(&sem[1],0,1);
    sem_init(&sem[2],0,1);
    sem_init(&sem[3],0,1);
    
    threads = (pthread_t*) malloc(sizeof(pthread_t)*NUM_THREADS);
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    LDA = nrows;
    int i;

    
    for(i = 0; i < NUM_THREADS; i++)
    {
        int *arg = (int*)malloc(sizeof(int));
        * arg = i;
        pthread_create(&threads[i], NULL, golWorker,(void*) arg);
    }

    for( i = 0; i < gens_max; i++)
    {
        printf("%d\n",i);

       fflush(stdout); 
        sem_wait(&sem[0]);
        sem_wait(&sem[1]);
        sem_wait(&sem[2]);
        sem_wait(&sem[3]);

        SWAP_BOARDS( outboard, inboard );
        
        sem_post(&sem[0]);
        sem_post(&sem[1]);
        sem_post(&sem[2]);
        sem_post(&sem[3]);
        
    }

        /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}


