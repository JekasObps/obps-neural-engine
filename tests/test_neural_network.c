#include "test_utils.h"
TEST_SETUP

#include "neural_network.h"

size_t CalcNetworkSize(size_t, size_t *);

void TestCreateDestroy()
{
    size_t layers[3] = {3, 3, 3};
    nnet_t *network = NNetCreate(3, layers, DEFAULT_ACT_FUNC, NULL);
    
    TEST_CASE((void*)network, NULL, NE, 
        "Network successfully allocated!", "Fail (exp:%p != given:%p)")

    NNetDestroy(network);
    network = NULL;
}

void TestCalcNetworkSize()
{
    size_t layers[3] = {3, 3, 3};
    size_t size = CalcNetworkSize(ARRLEN(layers), layers);
    TEST_CASE(size, 0ul, NE, "Success!", "Fail (exp:%lu != given:%lu)")
}

int main()
{
    TestCreateDestroy();
    TestCalcNetworkSize();

    TEST_RETURN();
}
