#pragma once
#include "CL/cl.hpp"

typedef cl_uint COUNTERS_TYPE;
typedef cl_uint INPUT_TYPE;
typedef cl_float FITNES_TYPE;

static const cl_uint MAX_OUTPUTS = 3;
static const cl_uint STATES_NUMBER = 4;
static const cl_uint MAX_TRANSITIONS = 5;



typedef struct tl_test_size
{
	cl_uint inputLength;
	cl_uint outputLength;
	cl_uint offset;
} TestInfo;

typedef struct tl_sequence
{
	cl_uint values[MAX_OUTPUTS];
}Sequence;

typedef struct tl_transition
{
	cl_uint outputs[MAX_OUTPUTS];
	cl_uint outputsCount;
	cl_uint nextState;
	cl_uint input;
} Transition;

typedef struct tl_transition_list
{
	Transition list[MAX_TRANSITIONS];
	cl_uint count;
} TransitionList;

typedef struct tl_transition_list_automat
{
	TransitionList states[STATES_NUMBER];
	cl_uint firstState;
} TransitionListAutomat;

//typedef TransitionListAutomat AUTOMAT;

static const char* clFilePath = "../GeneticTests/scenariosBuild.cl";