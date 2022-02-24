#ifndef _NEURAL_NETWORK_H_
#define _NEURAL_NETWORK_H_

#include "neural_def.h"

/**
 * Internal data type for a network structure.
 **/
typedef struct nnet nnet_t;


/* Allocates buffer and initializes nnet_t structure 
 *  nlayers      - layers amount in the net
 *  layers_conf  - pointer to an array, that contains layers' sizes(nodes)
 *  sigmoid      - function that calculates activation
 *  param        - for future extension
 */
nnet_t* NNetCreate(size_t nlayers, 
        size_t *restrict layers_conf, 
        act_func_t func, 
        void *restrict param);


/* Deallocates network structure
 */
void NNetDestroy(nnet_t *network);


/* Save network model to a file
 */
nnet_stat_t NNetSerialize(nnet_t *restrict network, const char *restrict file);


/* Restore network from a file
 */
nnet_stat_t NNetDeserialize(nnet_t *restrict network, const char *restrict file);
  

/* Sets network's weights to a random values [0.0 - 1.0]
 */
void NNetRandomizeWeights(nnet_t *network);


/* Returns Input Buffer to be filled with input data
 */
nnet_data_t* NNetGetInputBuffer(nnet_t *network);


/* Perform calculation with data inserted into an Input Buffer 
 */
void NNetProcessData(nnet_t *network);


/* Returns last layer activations buffer (output) 
 */
const nnet_data_t* NNetGetOutputBuffer(nnet_t *network);


/* Propagate Error
 */
void NNetPropagateError(nnet_t *restrict network, const nnet_data_t *restrict expected);


#endif/*_NEURAL_NETWORK_H_*/
