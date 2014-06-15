#define MAX_TEST_INPUT 8
#define MAX_TEST_OUTPUT 20
#define TESTS_NUMBER 10

#define MAX_OUTPUTS 3
#define STATES_NUMBER 4
#define MAX_TRANSITIONS 5
#define INPUTS_NUMBER 10
#define OUTPUTS_NUMBER 7

#define MUTATION_THRESHOLD 15

#define MFE_COUNTERS_SIZE 4

#include "GeneticTests/transitionList.clh"

#define DEBUG_ME (get_global_id(0)==0)

float distSame(uint outSize, const uint* out, uint testSize, __constant const uint* testOut)
{
    uint equal = (outSize == testSize);
    for (uint i=0; i<MAX_TEST_OUTPUT; ++i)
    {
        uint notSame = (out[i] != testOut[i]) && (i < outSize) ;
        equal = select(equal, (uint)0, notSame);
    }
    float res = select(0.0f, 1.0f, equal);
    return res;
}

float distHamm(uint outSize, const uint* out, uint testSize, __constant const uint* testOut)
{
    float dist = 0.0;
    for (uint i=0; i<MAX_TEST_OUTPUT; ++i)
    {
        uint notSame = (out[i] != testOut[i]) && (i < outSize) && (i < testSize);
        dist = select(dist, dist + 1.0f, notSame);
    }
    dist += max(outSize, testSize) - min(outSize, testSize);
    float len = (float)max(outSize, testSize);
    return dist/len;
}

float calcFitness(__global TransitionListAutomat* aut, __constant const TestInfo* testInfo, __constant const uint* tests)
{
    float pSum = 0.0;
    for (uint test=0; test<TESTS_NUMBER; ++test)
    {
        const uint lInput = testInfo[test].inputLength;
        const uint lOutput = testInfo[test].outputLength;
        __constant const uint* curInput = tests + testInfo[test].offset;
        __constant const uint* curOutput = tests + testInfo[test].offset + lInput;
        uint tmpOutput[MAX_TEST_OUTPUT];
        uint size = transform( aut, testInfo, tests, test, tmpOutput );
        float t = select( 1.0f, distHamm(size, tmpOutput, lOutput, curOutput), size!=0xFFFFFFFF);
        pSum += 1.0 - t;
    }
    return pSum/TESTS_NUMBER;
}

uint geneticAlgorithmElitismTrueGlobal( __global TransitionListAutomat* automats, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    __global TransitionListAutomat* me = automats + get_global_id(0);
    clearLabels(me);
    doLabelling(me, testInfo, tests);
    float myFR = calcFitness(me, testInfo, tests);
    fr[get_global_id(0)] = myFR;
    //barrier( CLK_GLOBAL_MEM_FENCE );
    return rand;
}

__kernel void genetic_1d( __global TransitionListAutomat* autBuf1, __global const uint* srandBuffer,
                         __constant const TestInfo* testInfo, __constant const uint* tests, __global float* resultCache )
{
    if (DEBUG_ME)
    {
        printf("OpenCL :: sizeof(TransitionListAutomat=%i)\n", sizeof(TransitionListAutomat));
        printf("OpenCL :: sizeof(TestInfo=%i)\n", sizeof(TestInfo));
    }
    //barrier( CLK_GLOBAL_MEM_FENCE );
    
	uint srand = srandBuffer[get_global_id(0)];
    uint rand = nextUINT( srand+(srand*get_global_id(0)) );
    rand = geneticAlgorithmElitismTrueGlobal( autBuf1, testInfo, tests, resultCache, rand);
}