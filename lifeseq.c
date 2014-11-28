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

int nrows_g, ncols_g;

/*
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
} */

void golWorker2(void * arg)
{
    int tid = *((int*)arg); 
    int nrows = nrows_g; 
    int ncols = ncols_g; 
    int i,j;
    int row_start = tid*nrows/NUM_THREADS;
    int row_end = row_start + nrows/NUM_THREADS;
    int inorth, isouth, jwest, jeast;
    char neighbor_count;

    for (j = 0; j < ncols; j++)
    {
        for (i = row_start; i < row_end; i++)
        {
               if ( i == 0 )
               {
                    inorth = nrows-1;
                    isouth = i + 1;
               }
               else if ( i == nrows-1 )
               {
                 inorth = i - 1;
                 isouth = 0;
               }
               else
               {
                 inorth = i - 1;
                 isouth = i + 1;
               }

              if (j == 0) 
              {
                jwest = ncols - 1;
                jeast = j + 1;
              }
                else if (j == ncols - 1)
              {
                jwest = j - 1;
                jeast = 0;
              }
              else
              {
                jwest = j - 1;
                jeast = j + 1;
              }
                neighbor_count = 
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
}

char*
sequential_game_of_life (char* outboard_, 
        char* inboard_,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    nrows_g = nrows;
    ncols_g = ncols;
    outboard = outboard_;
    inboard = inboard_;
    
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    LDA = nrows;
    int i, j;
    int arg0  = 0;
    int arg1  = 1;
    int arg2  = 2;
    int arg3  = 3;

    for(i = 0; i < gens_max; i++)
    {
      /*
      for(j = 0; j < NUM_THREADS; j++)
      {
          int *arg = (int*)malloc(sizeof(int));
          *arg = j;
          pthread_create(&threads[j], NULL, golWorker2,(void*) arg);
      }*/

          pthread_create(&threads[0], NULL, golWorker2,(void*) &arg0);
          pthread_create(&threads[1], NULL, golWorker2,(void*) &arg1);
          pthread_create(&threads[2], NULL, golWorker2,(void*) &arg2);
          pthread_create(&threads[3], NULL, golWorker2,(void*) &arg3);

      pthread_join(threads[0], NULL);
      pthread_join(threads[1], NULL);
      pthread_join(threads[2], NULL);
      pthread_join(threads[3], NULL);

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


