/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define NUM_THREADS 4
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

//struct to be passed into the forked workers as an argument
typedef struct {
    int nrows;
    int ncols;
    int thread_id;
} Args;

char *outboard;
char *inboard;

void golWorker(void * arg)
{
    Args *args = (Args*) arg; 
    int nrows = args->nrows;; 
    int ncols = args->ncols; 
    int tid = args->thread_id;
    int LDA = nrows;
    int i,j;
    int row_start = tid*nrows/NUM_THREADS;
    int row_end = row_start + nrows/NUM_THREADS;
    int inorth, isouth, jwest, jeast;
    char neighbor_count;

    for (j = 0; j < ncols; j++)
    {

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

			//Reordered to prefetch cache hits
            neighbor_count = 
                    BOARD (inboard, i, jwest) +
					BOARD (inboard, inorth, jwest) + 
                    BOARD (inboard, isouth, jwest) +
					BOARD (inboard, i, jeast) + 
					BOARD (inboard, inorth, j) + 
                    BOARD (inboard, inorth, jeast) + 
                    BOARD (inboard, isouth, j) + 
                    BOARD (inboard, isouth, jeast);

           BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));
      }
   }
}

char*
parallel_game_of_life (char* outboard_, 
        char* inboard_,
        const int nrows,
        const int ncols,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
    int i, j;
    outboard = outboard_;
    inboard = inboard_;
    pthread_t threads[NUM_THREADS];
    Args args[NUM_THREADS];

    for (i = 0; i < NUM_THREADS; i++)
    {
        args[i].nrows = nrows;
        args[i].ncols = ncols;
        args[i].thread_id = i;
    }

    for(i = 0; i < gens_max; i++)
    {
        pthread_create(&threads[0], NULL, golWorker,(void*) &args[0]);
        pthread_create(&threads[1], NULL, golWorker,(void*) &args[1]);
        pthread_create(&threads[2], NULL, golWorker,(void*) &args[2]);
        pthread_create(&threads[3], NULL, golWorker,(void*) &args[3]);

        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        pthread_join(threads[2], NULL);
        pthread_join(threads[3], NULL);

		//Next generation
        SWAP_BOARDS( outboard, inboard );
    }

    return inboard;
}
