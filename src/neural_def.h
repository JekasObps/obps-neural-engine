#ifndef _NEURAL_DEF_H_
#define _NEURAL_DEF_H_

/**
 * Contains common definitions to be used 
 *  by different neural modules.
 **/

#include <stddef.h>

#include "utils.h"

#define DEFAULT_ACT_FUNC logistic_sigmoid

/**
 * Status that is used as return value 
 *  for different API functions.
 **/ 
typedef enum nnet_stat 
{
    NNET_SUCCESS = 0,
    NNET_FAILURE = -1
} nnet_stat_t;


/**
 * Type for weights, biases and activations 
 *  stored in nnet. May be changed in the future 
 *  with flags.
 **/
typedef float nnet_data_t;


/**
 * Activation function of a neural network. 
 **/
typedef nnet_data_t (*act_func_t) (nnet_data_t);   


/**
 *  Default Activation Function.
 **/
nnet_data_t logistic_sigmoid(nnet_data_t wasum);



/**
 * Error that propagates 
 *  through network structure to be corrected.
 **/
typedef struct nnet_err nnet_err_t; 


#endif/*_NEURAL_DEF_H_*/
