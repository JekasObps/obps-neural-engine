#include "test_utils.h"
TEST_SETUP

#include "neural_layer.h"

void TestLayerInit(void)
{
    char buff[256] = {0};
    
    size_t curr_size = 3, 
           next_size = 2;

    nnet_data_t *weights = (nnet_data_t*)((char*)buff + NNetSizeOfLayerHeader()),
                *biases = weights + curr_size * next_size,
                *acts = biases + curr_size;
    nnet_layer_t *prev = (nnet_layer_t*)0xDEADBEAFul;
    nnet_layer_t *next = (nnet_layer_t*)0xBADCAFFEul;

    nnet_layer_t *layer = NNetLayerInit(buff, curr_size, weights, biases, acts, prev, next);


    TEST_CASE((void*)layer, (void*)buff, EQ, 
        "Layer's address is correct", "Fail (exp:%p == given:%p)")

    TEST_CASE(NNetLayerGetSize(layer), curr_size, EQ, 
        "Size was correctly set", "Fail (exp:%lu == given:%lu)")

    TEST_CASE((void*)NNetLayerGetWeights(layer), (void*)weights, EQ, 
        "Weights pointer was correctly set", "Fail (exp:%p == given:%p)")

    TEST_CASE((void*)NNetLayerGetBiases(layer), (void*)biases, EQ, 
        "Biases pointer was correctly set", "Fail (exp:%p == given:%p)")

    TEST_CASE((void*)NNetLayerGetActivations(layer), (void*)acts, EQ, 
        "Activations pointer was correctly set", "Fail (exp:%p == given:%p)")
    
    TEST_CASE((void*)NNetGetNextLayer(layer), (void*)next, EQ, 
        "Next layer was correctly set", "Fail (exp:%p == given:%p)")

    TEST_CASE((void*)NNetGetPrevLayer(layer), (void*)prev, EQ, 
        "Next layer was correctly set", "Fail (exp:%p == given:%p)")
}

void TestApplyWeight(void)
{
    nnet_data_t act = 0.25, wht = 0.31;
    TEST_CASE(NNetApplyWeight(act, wht), act * wht, EQ, 
        "Success!", "Fail (exp:%4.4f == given:%4.4f)")
}

void TestGetForNeuron(void)
{
    char first_buff[256];
    char second_buff[256];
    
    const size_t first_size = 2, second_size = 2;
    nnet_data_t first_whts[2*2] = {0.1, 0.2,   0.3, 0.4}; 
    /* output layer has no weights */

    nnet_data_t second_biases[2] = {1.0, 2.0}; /* input layer has no biases */
    
    nnet_data_t first_acts[2] = {0.34, 0.45};
    nnet_data_t second_acts[2] = {0, 0};

    nnet_layer_t *l1 = NNetLayerInit(first_buff, first_size, first_whts, NULL, first_acts, NULL, NULL);
    nnet_layer_t *l2 = NNetLayerInit(second_buff, second_size, NULL, second_biases, second_acts, l1, NULL);
    NNetSetNextLayer(l1, l2); /* link two layers forward */
    
    TEST_CASE((void*)NNetGetWeightsForNeuron(l2, 0),
        (void*)(first_whts), EQ, "Success!", "Fail (exp:%p == given:%p)")

    TEST_CASE((void*)NNetGetWeightsForNeuron(l2, 1),
        (void*)(first_whts+2), EQ, "Success!", "Fail (exp:%p == given:%p)")

    TEST_CASE((void*)NNetGetBiasForNeuron(l2, 0), (void*)second_biases, EQ,
        "Success!", "Fail (exp:%p == given:%p)")

    TEST_CASE((void*)NNetGetActForNeuron(l1, 1), (void*)&first_acts[1], EQ,
        "Success!", "Fail (exp:%p == given:%p)")
    
    nnet_data_t sum[2];

    sum[0] = NNetApplyWeight(first_acts[0], first_whts[0]) 
           + NNetApplyWeight(first_acts[1], first_whts[1]);

    TEST_CASE(NNetCalcSumOfAppliedWeights(l2, 0), sum[0], EQ, 
            "Success!", "Fail (exp:%4.4f == given:%4.4f)")

    sum[1] = NNetApplyWeight(first_acts[0], first_whts[2]) 
           + NNetApplyWeight(first_acts[1], first_whts[3]);

    TEST_CASE(NNetCalcSumOfAppliedWeights(l2, 1), sum[1], EQ, 
            "Success!", "Fail (exp:%4.4f == given:%4.4f)")
    
    nnet_data_t newact = NNetCalcActForNeuron(l2, 0, DEFAULT_ACT_FUNC);
    
    TEST_CASE(newact, second_acts[0], EQ, 
            "Success! Return value matches with stored one.", "Fail (exp:%4.4f == given:%4.4f)")
    TEST_CASE(newact, second_biases[0] + DEFAULT_ACT_FUNC(sum[0]), EQ, 
            "Success! An activation calculated correctly!", "Fail (exp:%4.4f == given:%4.4f)")

}

int main()
{
    TestLayerInit();
    TestApplyWeight();
    TestGetForNeuron();

    TEST_RETURN();
}
