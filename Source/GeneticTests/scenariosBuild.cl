#define MAX_TEST_INPUT 8
#define MAX_TEST_OUTPUT 20
#define TESTS_NUMBER 10

#define MAX_OUTPUTS 3
#define STATES_NUMBER 4
#define MAX_TRANSITIONS 5
#define INPUTS_NUMBER 10
#define OUTPUTS_NUMBER 7

#define MUTATION_THRESHOLD 15

#include "GeneticTests/transitionList.clh"

float distSame(uint outSize, const uint* out, uint testSize, const uint* testOut)
{
    uint equal = (outSize == testSize);
    for (uint i=0; i<MAX_TEST_OUTPUT; ++i)
    {
        uint notSame = (out[i] != testOut[i]) && (i < outSize) ;
        equal = select(equal, 0, notSame);
    }
    float res = select(0.0, 1.0, equal);
    return res;
}

float distHamm(uint outSize, const uint* out, uint testSize, const uint* testOut)
{
    float dist = 0.0;
    for (uint i=0; i<MAX_TEST_OUTPUT; ++i)
    {
        uint notSame = (out[i] != testOut[i]) && (i < outSize) && (i < testSize);
        dist = select(dist, dist + 1.0, notSame);
    }
    dist += max(outSize, testSize) - min(outSize, testSize);
    return res;
}

float calcFitness(TransitionListAutomat* aut, const TestInfo* testInfo, const uint* tests)
{
    float pSum = 0.0;
    for (uint test=0; test<TESTS_NUMBER; ++test)
    {
        const uint lInput = testInfo[test].inputLength;
        const uint lOutput = testInfo[test].outputLength;
        const uint* curInput = tests + testInfo[test].offset;
        const uint* curOutput = tests + testInfo[test].offset + lInput;
        uint tmpOutput[MAX_TEST_OUTPUT];
        uint size = transform( aut, testInfo, tests, test, tmpOutput );
        float t = select( 1.0, distHamm(size, tmpOutput, lOutput, curOutput), size!=0xFFFFFFFF);
        pSum += 1.0 - t;
    }
}

void geneticAlgorithmElitismTrueGlobal(TransitionListAutomat* automats, const TestInfo* testInfo, const uint* tests, __global float* fr, uint rand)
{
    TransitionListAutomat* me = automats + get_global_id(0)*sizeof(TransitionListAutomat);
    doLabelling(me, testInfo, tests);
    float myFR = calcFitness(me, testInfo, tests);
    fr[get_global_id(0)] = myFR;
    barrier( CLK_GLOBAL_MEM_FENCE );
    
}

__kernel void genetic_1d( __check_space const TransitionListAutomat* autBuf1, __constant const uint* constSizes,
                         __constant const TestInfo* testInfo, __constant const uint* tests, __global float* resultCache )
{
	uint srand = constSizes[0];
    uint rand = nextInt( srand+(srand*get_global_id(0)) );
    geneticAlgorithm( automats, testInfo, tests, resultCache, rand);
}