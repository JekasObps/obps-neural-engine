#include "neural_layer.h"

#include <assert.h>

/* 
 * Layer must have access to a next and prev layers.
 */

struct nnet_layer
{
    size_t size; /* amount of neurons stored in layer */

    nnet_data_t *wht_buff;
    nnet_data_t *bias_buff;
    nnet_data_t *act_buff;

    struct nnet_layer *prev; /* pointer to a prev layer, if NULL -> curr layer is the first */
    struct nnet_layer *next; /* if NULL -> curr layer is the last */     
};

size_t NNetSizeOfLayerHeader(void)
{
    return sizeof(nnet_layer_t);
}


nnet_layer_t* NNetLayerInit(void *ptr, 
        size_t size,
        nnet_data_t *wht,
        nnet_data_t *bias,
        nnet_data_t *act,
        nnet_layer_t *prev, 
        nnet_layer_t *next)
{
    assert(ptr);
    assert(size != 0);
    assert(act); /* must in every layer */
    
    nnet_layer_t* curr_layer = (nnet_layer_t*) ptr;
    
    curr_layer->size = size;
    curr_layer->wht_buff = wht;
    curr_layer->bias_buff = bias;
    curr_layer->act_buff = act;
    curr_layer->prev = prev;
    curr_layer->next = next;

    return curr_layer;
}


nnet_data_t NNetCalcActForNeuron(nnet_layer_t *layer, size_t neuron_id, act_func_t act_func)
{
    assert(layer);
    assert(neuron_id < layer->size);
    assert(("`layer' is not first one", layer->prev));

    nnet_data_t sum_app_wht = NNetCalcSumOfAppliedWeights(layer, neuron_id);
    nnet_data_t * act = NNetGetActForNeuron(layer, neuron_id);
    nnet_data_t bias = *NNetGetBiasForNeuron(layer, neuron_id); 

    *act = bias + act_func(sum_app_wht);
    return *act;
}


nnet_data_t NNetCalcSumOfAppliedWeights(nnet_layer_t *layer, size_t neuron_id)
{
    assert(layer);
    assert(neuron_id < layer->size);
    assert(("`layer' is not first one", layer->prev));
    
    nnet_data_t sum = 0;
    nnet_data_t *weights = NNetGetWeightsForNeuron(layer, neuron_id);
    nnet_data_t *act = NNetLayerGetActivations(layer->prev);
    size_t prev_layer_size = NNetLayerGetSize(layer->prev);
    
    for(size_t i = 0; i < prev_layer_size; ++i)
    {
        sum += NNetApplyWeight(act[i], weights[i]);
    }

    return sum; 
}


nnet_data_t NNetApplyWeight(nnet_data_t act, nnet_data_t wht)
{
    return act * wht;
}


nnet_data_t* NNetGetWeightsForNeuron(nnet_layer_t *layer, size_t neuron_id)
{
    assert(layer);
    assert(neuron_id < layer->size);
    assert(("`layer' is not first one", layer->prev));
    
    nnet_data_t *weights = NNetLayerGetWeights(layer->prev);
    
    return weights + (NNetLayerGetSize(layer->prev) * neuron_id);
}


nnet_data_t* NNetGetBiasForNeuron(nnet_layer_t *layer, size_t neuron_id)
{
    assert(layer);
    assert(neuron_id < layer->size);
    assert(("`layer' is not first one", layer->prev));
    
    return NNetLayerGetBiases(layer)+neuron_id;
}


nnet_data_t* NNetGetActForNeuron(nnet_layer_t *layer, size_t neuron_id)
{
    assert(layer);
    assert(neuron_id < layer->size);
    
    return NNetLayerGetActivations(layer)+neuron_id;
}


size_t NNetLayerGetSize(nnet_layer_t *layer)
{
    assert(layer);
    return layer->size;
}


nnet_data_t* NNetLayerGetWeights(nnet_layer_t *layer)
{
    assert(layer);
    return layer->wht_buff;
}


nnet_data_t* NNetLayerGetBiases(nnet_layer_t *layer)
{
    assert(layer);
    return layer->bias_buff;
}


nnet_data_t* NNetLayerGetActivations(nnet_layer_t *layer)
{
    assert(layer);
    return layer->act_buff;
}


nnet_layer_t* NNetGetPrevLayer(nnet_layer_t *layer)
{
    assert(layer);
    return layer->prev;
}


nnet_layer_t* NNetGetNextLayer(nnet_layer_t *layer)
{
    assert(layer);
    return layer->next;
}


void NNetSetPrevLayer(nnet_layer_t *restrict layer, nnet_layer_t *restrict prev)
{
    assert(layer);
    layer->prev = prev;
}


void NNetSetNextLayer(nnet_layer_t *restrict layer, nnet_layer_t *restrict next)
{
    assert(layer);
    layer->next = next;
}

