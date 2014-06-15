#pragma once
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include "CL/cl.hpp"

typedef cl_uint COUNTERS_TYPE;
typedef cl_uint INPUT_TYPE;
typedef cl_float FITNES_TYPE;

static const cl_uint MAX_OUTPUTS = 3;
static const cl_uint STATES_NUMBER = 4;
static const cl_uint MAX_TRANSITIONS = 5;
static const cl_uint MFE_COUNTERS_SIZE = 4;

#include <boost/property_tree/ptree.hpp>

struct GeneticSettings
{
	size_t populationSize;
	double desiredFitness;
	size_t stateNumber;
	double partStay;
	size_t timeSmallMutation;
	size_t timeBigMutation;
	double mutationProbability;

	size_t maxStateOutputCount;
	size_t maxStateTransitions;

	GeneticSettings();
	void readSettings(const boost::property_tree::ptree& pt);
};

/*class SequenceWrapper
{
public:
	SequenceWarpper
	size_t sizeofme() const;
};

class TransitionListAutomatWrapper
{

};*/

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

class seq_less
{
public:
	bool operator ()(const Sequence& s1, const Sequence& s2)
	{
		for (size_t i = 0; i < MAX_OUTPUTS; ++i)
		{
			if (s1.values[i] < s2.values[i])
				return true;
		}
		return false;
	}
};

typedef struct mfe_getter
{
	Sequence data[MFE_COUNTERS_SIZE];
	cl_uint counters[MFE_COUNTERS_SIZE];
} mfe_Getter;

typedef struct tl_transition
{
	mfe_Getter mfe;
	cl_uint outputs[MAX_OUTPUTS];
	cl_uint outputsCount;
	cl_uint nextState;
	cl_uint input;
	cl_uint obviousBest;
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