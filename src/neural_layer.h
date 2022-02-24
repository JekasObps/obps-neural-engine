#ifndef _NEURAL_LAYER_H_
#define _NEURAL_LAYER_H_

#include "neural_def.h"

typedef struct nnet_layer nnet_layer_t;


/*
 * Initializes layer inplace on already allocated mem region.
 */
nnet_layer_t* NNetLayerInit(void *ptr, 
        size_t size,
        nnet_data_t *wht,
        nnet_data_t *bias,
        nnet_data_t *act,
        nnet_layer_t *prev, 
        nnet_layer_t *next);

/*
 * Return size of layer structure
 *
 */
size_t NNetSizeOfLayerHeader(void);


/*
 * Updates activation of a neuron in network structure and returns new value.
 */
nnet_data_t NNetCalcActForNeuron(nnet_layer_t *layer, size_t neuron_id, act_func_t act_func);


/*
 * Calculate sum of applied weights for specific neuron.
 */
nnet_data_t NNetCalcSumOfAppliedWeights(nnet_layer_t *layer, size_t neuron_id);


/*
 * Returns applied weight.
 */
nnet_data_t NNetApplyWeight(nnet_data_t act, nnet_data_t wht);


/*
 * Returns pointer to a weights linked with current layer neuron by id.
 */
nnet_data_t* NNetGetWeightsForNeuron(nnet_layer_t *layer, size_t neuron_id);


/*
 * Return Bias for specific neuron by id.
 */
nnet_data_t* NNetGetBiasForNeuron(nnet_layer_t *layer, size_t neuron_id);


/*
 * Return Activation for specific neuron by id. 
 */
nnet_data_t* NNetGetActForNeuron(nnet_layer_t *layer, size_t neuron_id);


/*
 * Returns layer size in neurons.
 */
size_t NNetLayerGetSize(nnet_layer_t *layer);


/*
 * Returns pointer to a buffer that contains weights for synapses 
 *  between current layer and next layer.
 */
nnet_data_t* NNetLayerGetWeights(nnet_layer_t *layer);


/*
 * Return pointer to a biases buffer.
 */
nnet_data_t* NNetLayerGetBiases(nnet_layer_t *layer);


/*
 * Returns pointer to an activations buffer of a layer.
 */
nnet_data_t* NNetLayerGetActivations(nnet_layer_t *layer);


/*
 * Returns prev layer.
 */
nnet_layer_t* NNetGetPrevLayer(nnet_layer_t *layer);


/*
 * Returns next layer.
 */
nnet_layer_t* NNetGetNextLayer(nnet_layer_t *layer);


/*
 * Setter for prev layer.
 */
void NNetSetPrevLayer(nnet_layer_t *restrict layer, nnet_layer_t *restrict prev);


/*
 * Setter for next layer.
 */
void NNetSetNextLayer(nnet_layer_t *restrict layer, nnet_layer_t *restrict next);


#endif/*_NEURAL_LAYER_H_*/
