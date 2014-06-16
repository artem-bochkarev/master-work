#define MAX_TEST_INPUT 8
#define MAX_TEST_OUTPUT 20
#define TESTS_NUMBER 10

#define MAX_OUTPUTS 3
#define STATES_NUMBER 4
#define MAX_TRANSITIONS 5
#define INPUTS_NUMBER 10
#define OUTPUTS_NUMBER 7

#define MFE_COUNTERS_SIZE 4

//elitism vars
#define MAX_LOCAL_SIZE 256
#define GLOBAL_SIZE 1024
#define GO_VALUE 16

#define TOURNAMENTS_COUNT 8

#include "GeneticTests/transitionList.clh"

#define DEBUG_ME (get_global_id(0)==0)

#define GO_VALUE_RANDOMIZED 1
#define CHECK_COUNT 32


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
    float dist = 0.0f;
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
    float pSum = 0.0f;
    for (uint test=0; test<TESTS_NUMBER; ++test)
    {
        const uint lInput = testInfo[test].inputLength;
        const uint lOutput = testInfo[test].outputLength;
        __constant const uint* curOutput = tests + testInfo[test].offset + lInput;
        uint tmpOutput[MAX_TEST_OUTPUT];
        uint size = transform2( aut, testInfo, tests, test, tmpOutput );
        float t = select( 1.0f, distHamm(size, tmpOutput, lOutput, curOutput), size!=0xFFFFFFFF);
        pSum += 1.0f - t;
    }
    return pSum/TESTS_NUMBER;
}

void countFitnessAll( __global TransitionListAutomat* automats, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr )
{
    __global TransitionListAutomat* me = automats + get_global_id(0);
    clearLabels(me);
    doLabelling(me, testInfo, tests);
    float myFR = calcFitness(me, testInfo, tests);
    uint myGlobalId = get_global_id(0);
    fr[myGlobalId] = 0.0f;//myFR;
}

uint nextGenerationElitismTrueGlobal( __global TransitionListAutomat* automats, __global TransitionListAutomat* newAutomats, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    countFitnessAll(automats, testInfo, tests, fr);
    barrier( CLK_GLOBAL_MEM_FENCE );

    uint myGlobalId = get_global_id(0);
    __global TransitionListAutomat* me = automats + myGlobalId;
    uint greater = 0;
    
    for (uint i=0; i<GLOBAL_SIZE; ++i)
    {
        uint isGreater = select(fr[myGlobalId] < fr[i], myGlobalId >= i, fr[myGlobalId] == fr[i]);
        greater = select(greater, greater + 1, isGreater);
    }
    uint take = (greater <= GO_VALUE);
    if (!take)
    {
        uint ids[TOURNAMENTS_COUNT];
        uint first = 0;
        uint second = 0;
        #pragma unroll
        for (uint i=0; i<TOURNAMENTS_COUNT; ++i)
        {
            ids[i] = rand % GLOBAL_SIZE;
            rand = nextUINT(rand);
            second = select(second, first, fr[ids[i]] > fr[ids[first]]);
            first = select(first, i, fr[ids[i]] > fr[ids[first]]);
        }
        uint motherId = ids[first];
        uint fatherId = ids[second];
        __global TransitionListAutomat* mother = automats + motherId;
        __global TransitionListAutomat* father = automats + fatherId;
        rand = crossover1(mother, father, newAutomats + myGlobalId, rand);
        rand = mutateMe( newAutomats + myGlobalId, rand );
    }else
    {
        copyTListAutomat(newAutomats + myGlobalId, me);
    }
    return rand;
}

uint nextGenerationElitismRandomized( __global TransitionListAutomat* automats, __global TransitionListAutomat* newAutomats, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    countFitnessAll(automats, testInfo, tests, fr);
    barrier( CLK_GLOBAL_MEM_FENCE );

    uint myGlobalId = get_global_id(0);
    __global TransitionListAutomat* me = automats + myGlobalId;
    uint greater = 0;
    
    for (uint t=0; t<CHECK_COUNT; ++t)
	{
        uint i = getValue1024(rand, GLOBAL_SIZE);
        rand = nextUINT(rand);
		uint isGreater = select(fr[myGlobalId] < fr[i], myGlobalId >= i, fr[myGlobalId] == fr[i]);
		greater = select(greater, greater + 1, isGreater);
	}
    
    uint take = (greater <= GO_VALUE_RANDOMIZED);
    if (!take)
    {
        uint ids[TOURNAMENTS_COUNT];
        uint first = 0;
        uint second = 0;
        #pragma unroll
        for (uint i=0; i<TOURNAMENTS_COUNT; ++i)
        {
            ids[i] = rand % GLOBAL_SIZE;
            rand = nextUINT(rand);
            second = select(second, first, fr[ids[i]] > fr[ids[first]]);
            first = select(first, i, fr[ids[i]] > fr[ids[first]]);
        }
        uint motherId = ids[first];
        uint fatherId = ids[second];
        __global TransitionListAutomat* mother = automats + motherId;
        __global TransitionListAutomat* father = automats + fatherId;
        rand = crossover1(mother, father, newAutomats + myGlobalId, rand);
        rand = mutateMe( newAutomats + myGlobalId, rand );
    }else
    {
        copyTListAutomat(newAutomats + myGlobalId, me);
    }
    return rand;
}

uint geneticAlgorithmElitismTrueGlobal( __global TransitionListAutomat* automats1, __global TransitionListAutomat* automats2, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    //__global TransitionListAutomat* me = automats1 + get_global_id(0);
    
    for (uint i=0; i<10; ++i)
    {
        rand = nextGenerationElitismTrueGlobal( automats1, automats2, testInfo, tests, fr, rand );
        rand = nextGenerationElitismTrueGlobal( automats2, automats1, testInfo, tests, fr, rand );
    }
    countFitnessAll(automats1, testInfo, tests, fr);
    return rand;
}

uint geneticAlgorithmElitismRandomized( __global TransitionListAutomat* automats1, __global TransitionListAutomat* automats2, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    //__global TransitionListAutomat* me = automats1 + get_global_id(0);
    
    for (uint i=0; i<10; ++i)
    {
        rand = nextGenerationElitismRandomized( automats1, automats2, testInfo, tests, fr, rand );
        rand = nextGenerationElitismRandomized( automats2, automats1, testInfo, tests, fr, rand );
    }
    countFitnessAll(automats1, testInfo, tests, fr);
    return rand;
}

__kernel void genetic_1d( __global TransitionListAutomat* autBuf1, __global const uint* srandBuffer,
                         __constant const TestInfo* testInfo, __constant const uint* tests, __global float* resultCache, __global TransitionListAutomat* autBuf2 )
{
    /*if (DEBUG_ME)
    {
        printf("OpenCL :: sizeof(TransitionListAutomat=%i)\n", sizeof(TransitionListAutomat));
        printf("OpenCL :: sizeof(TestInfo=%i)\n", sizeof(TestInfo));
    }*/
    //barrier( CLK_GLOBAL_MEM_FENCE );
    
    uint srand = srandBuffer[get_global_id(0)];
    uint rand = nextUINT( srand );
    rand = geneticAlgorithmElitismTrueGlobal( autBuf1, autBuf2, testInfo, tests, resultCache, rand);
    //rand = geneticAlgorithmElitismRandomized( autBuf1, autBuf2, testInfo, tests, resultCache, rand);
}