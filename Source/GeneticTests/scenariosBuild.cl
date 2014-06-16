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

#include "GeneticTests/transitionList.clh"

#define DEBUG_ME (get_global_id(0)==0)

uint mutateMe(__global TransitionListAutomat* input, uint rand)
{
    uint condition = randomProb1024( rand, MUTATION_THRESHOLD_1024); rand = nextUINT(rand);
    input->firstState = select( input->firstState, getValue1024(rand, STATES_NUMBER), condition);
    rand = nextUINT(rand);
    
    for (uint state=0; state<STATES_NUMBER; ++state)
    {
        for (uint transition=0; transition<MAX_TRANSITIONS; ++transition)
        {
            rand = mutateTransition(&(input->states[state].list[transition]), rand);
        }
    }
    /*for (uint state=0; state<STATES_NUMBER; ++state)
    {
        uint enabledAll = randomProb256(rand, MUTATION_THRESHOLD); rand = nextUINT(rand);
        uint who = randomBool(rand); rand = nextUINT(rand);
        uint enabledDelete = (who==0) && enabledAll && (input->states[state].count > 0);
        uint enabledAdd = (who==1) && enabledAll && (input->states[state].count < MAX_TRANSITIONS);
        rand = deleteTransition(&(input->states[state]), rand, enabledDelete );
        rand = addTransition(&(input->states[state]), rand, enabledAdd );
        
        removeDuplicates(&(input->states[state]));
    }*/
    return rand;
}

uint mutate(__global TransitionListAutomat* result, __global const TransitionListAutomat* input, uint rand)
{
    copyTListAutomat(result, input);
    rand = mutateMe(result, rand);
    return rand;
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
    fr[myGlobalId] = myFR;
}

uint nextGenerationElitismTrueGlobal(__global TransitionListAutomat* automats, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    countFitnessAll(automats, testInfo, tests, fr);
    barrier( CLK_GLOBAL_MEM_FENCE );

    __global TransitionListAutomat* me = automats + get_global_id(0);
    uint greater = 0;
    uint myGlobalId = get_global_id(0);
    for (uint i=0; i<GLOBAL_SIZE; ++i)
    {
        uint isGreater = select(fr[myGlobalId] < fr[i], myGlobalId >= i, fr[myGlobalId] == fr[i]);
        greater = select(greater, greater + 1, isGreater);
    }
    uint take = (greater <= GO_VALUE);
    if (!take)
    {
        rand = mutateMe( me, rand);
    }
    return rand;
}

uint geneticAlgorithmElitismTrueGlobal( __global TransitionListAutomat* automats, __constant const TestInfo* testInfo, __constant const uint* tests, __global float* fr, uint rand)
{
    __global TransitionListAutomat* me = automats + get_global_id(0);
    rand = nextGenerationElitismTrueGlobal( automats, testInfo, tests, fr, rand );
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
    uint rand = nextUINT( srand );
    rand = geneticAlgorithmElitismTrueGlobal( autBuf1, testInfo, tests, resultCache, rand);
}