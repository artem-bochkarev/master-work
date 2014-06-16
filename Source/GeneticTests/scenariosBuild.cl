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

uint transform2( __global TransitionListAutomat* aut, __constant const TestInfo* testInfo, __constant const uint* tests, uint test, uint* out)
{
    const uint lInput = testInfo[test].inputLength;
    __constant const uint* curInput = tests + testInfo[test].offset;
    uint currentState = aut->firstState;
    uint pOutput = 0;
    uint broken = 0;
    uint lenBeforeBroken = 0;
    for (uint i=0; i<lInput; ++i)
    {
        uint found = 0;
        uint k = 0;
        uint oldState = currentState;
        for (uint t=0; t<MAX_TRANSITIONS; ++t)
        {
            uint good = (t < aut->states[currentState].count) && (aut->states[currentState].list[t].input == curInput[i]);
            found = select(found, (uint)1, good);
            k = select(k, t, good);
            currentState = select(currentState, aut->states[currentState].list[t].nextState, good);
        }
        broken = select(broken, (uint)1, (found==0));
        
        for (uint x = 0; x<MAX_OUTPUTS; ++x)
        {
            uint outCount = aut->states[oldState].list[k].outputsCount;
            uint good1 = (x < outCount);
            out[pOutput] = aut->states[oldState].list[k].outputs[x];
            pOutput = select(pOutput, pOutput + 1, good1);
			pOutput = select(pOutput, (uint)MAX_TEST_OUTPUT-1, pOutput>=MAX_TEST_OUTPUT);
        }
        lenBeforeBroken = select(lenBeforeBroken, pOutput, !broken);
    }
    //pOutput = select(pOutput, (uint)0xFFFFFFFF, broken);
    pOutput = select(pOutput, lenBeforeBroken, broken);
    return pOutput;
}

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
        __constant const uint* curOutput = tests + testInfo[test].offset + lInput;
        uint tmpOutput[MAX_TEST_OUTPUT];
        uint size = transform2( aut, testInfo, tests, test, tmpOutput );
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
    /*if (DEBUG_ME)
    {
        printf("OpenCL :: sizeof(TransitionListAutomat=%i)\n", sizeof(TransitionListAutomat));
        printf("OpenCL :: sizeof(TestInfo=%i)\n", sizeof(TestInfo));
    }*/
    //barrier( CLK_GLOBAL_MEM_FENCE );
    
    uint srand = srandBuffer[get_global_id(0)];
    uint rand = nextUINT( srand+(srand*get_global_id(0)) );
    rand = geneticAlgorithmElitismTrueGlobal( autBuf1, testInfo, tests, resultCache, rand);
}