#include "neural_network.h"
#include "neural_layer.h"

#include <assert.h> /* assert */
#include <stdlib.h> /* malloc */
#include <math.h>   /* exp */

struct nnet 
{
    char _[1];
};

size_t CalcNetworkSize(size_t nlayers, size_t *layers_conf);

nnet_t* NNetCreate(size_t nlayers, 
        size_t *restrict layers_conf, 
        act_func_t func, 
        void *restrict param)
{
    assert(layers_conf);
    assert(func);
    
    UNUSED(param);

    size_t size = CalcNetworkSize(nlayers, layers_conf);
    nnet_t* net = (nnet_t*) malloc(size);

    if (net)
    {
        /* network initialization */
    }

    return net;
}

/**
 * Helper function used by NNetCreate to allocate memory for the network 
 **/
size_t CalcNetworkSize(size_t nlayers, size_t *layers_conf)
{
    assert(layers_conf);
    assert(nlayers >= 2);

    /* first layer has only an activation buffer */
    size_t size = sizeof(nnet_data_t) * layers_conf[0];
    
    /* foreach layer after first */
    for(size_t l = 1; l < nlayers; ++l)
    { 
        size_t act_buff_s = layers_conf[l] * sizeof(nnet_data_t);
        size_t bias_buff_s = act_buff_s;

        size_t wht_buff_s = layers_conf[l] * layers_conf[l-1] 
            * sizeof(nnet_data_t);
        
        size += act_buff_s + bias_buff_s + wht_buff_s;
    }
    
    size += nlayers * NNetSizeOfLayerHeader();

    return size;
}


void NNetDestroy(nnet_t *network)
{
    assert(network);
    free(network);
}


nnet_data_t logistic_sigmoid(nnet_data_t wasum)
{
    return 1.f / (1 + exp(-wasum));
}

