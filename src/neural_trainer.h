#ifndef _NEURAL_TRAINER_H_
#define _NEURAL_TRAINER_H_

#include "neural_network.h"

typedef struct nnet_trainer nnet_trainer_t;

typedef struct nnet_statistics
{
    int   iteration_number; /**/
    int   iterations; /* amount of iterations statistics is based of */
    
    float sum_acc; /* iterations' accuracity sum */
    float last_acc; /* last run accuracity */
    float first_acc; /* (current - iterations + 1) run accuracity */
    
    float old_avg; /* last calculated acc avg */
    float old_aac; /* previous run acc */
    
} nnet_statistics_t;


/* Creates trainer with dataset file.
 */
nnet_trainer_t* NNetTrainerCreate(nnet_t *restrict network, const char *restrict dataset);


/* Deallocate Trainer and all associated resources.
 */
void NNetTrainerDestroy(nnet_trainer_t *trainer);


/* Write statistics into a file instead of stdout.
 */
nnet_stat_t NNetTrainerStatsFile(nnet_trainer_t *restrict trainer, const char *restrict output_file);


/* Train iterations.
 */
nnet_stat_t NNetTrain(nnet_trainer_t *trainer, int iterations);



#endif/*_NEURAL_TRAINER_H_*/
