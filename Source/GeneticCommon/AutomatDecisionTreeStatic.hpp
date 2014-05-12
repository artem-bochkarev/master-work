#pragma once
#include "AutomatDecisionTreeStatic.h"
#include "Tools\binFunc.hpp"
#include <boost\assert.hpp>

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::CAutomatDecisionTreeStatic()
{
	memset(buffer, 0, sizeof(COUNTERS_TYPE)*(bufferSize));
	buffer[0] = 666;
	buffer[1] = 666;
	buffer[2] = 666;
	buffer[3] = 666;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::CAutomatDecisionTreeStatic(CAntCommon<COUNTERS_TYPE>* pAntCommon)
:pAntCommon(pAntCommon)
{
	memset(buffer, 0, sizeof(COUNTERS_TYPE)*(bufferSize));
	buffer[0] = 666;
	buffer[1] = 666;
	buffer[2] = 666;
	buffer[3] = 666;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::CAutomatDecisionTreeStatic(const CAutomatDecisionTreeStatic& automat)
:pAntCommon(automat.pAntCommon)
{
	memcpy(buffer, automat.buffer, bufferSize*sizeof(COUNTERS_TYPE));
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::~CAutomatDecisionTreeStatic()
{

}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>& CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::operator = (const CAutomatDecisionTreeStatic& automat)
{
	pAntCommon = automat.pAntCommon;
	memcpy(buffer, automat.buffer, bufferSize*sizeof(COUNTERS_TYPE));
	return *this;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::generateRandom(CRandom* rand)
{
	for (size_t i = 0; i < STATES_COUNT; ++i)
	{
		std::set<size_t> was;
		nextFreePosition(i);
		addRandomNode(rand, i, 0, was, 1);
	}
	buffer[0] = rand->nextUINT() % STATES_COUNT;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::nextFreePosition(size_t stateNumber)
{
	size_t result = 0;
	if (m_freePositions.find(stateNumber) != m_freePositions.end())
		result = m_freePositions[stateNumber];
	m_freePositions[stateNumber] = result + 1;
	return result;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::addRandomNode
	(CRandom* rand, size_t stateNumber, size_t position, std::set<size_t>& was, size_t curDepth)
{
	size_t a = rand->nextUINT() & 255;
	COUNTERS_TYPE* curPtr = buffer + commonDataSize + stateNumber*TREE_SIZE + NODE_SIZE * position;
	if ( curDepth == MAX_DEPTH || (a > 200))
	{
		//make a leaf
		*curPtr = 0;
		++curPtr;
		*curPtr = pAntCommon->randomState(rand);
		++curPtr;
		*curPtr = pAntCommon->randomAction(rand);
	}
	else
	{
		//make node
		std::set<size_t> free;
		for (size_t i = 1; i <= INPUT_PARAMS_COUNT; ++i)
			free.insert(i);
		for (size_t a : was)
			free.erase(a);
		size_t choosed = rand->nextUINT() % free.size();
		size_t k = 0;
		for (size_t a : free)
		{
			++k;
			if (k == choosed)
			{
				k = a;
				break;
			}
		}
		*curPtr = k;
		++curPtr;
		was.insert(k);
		
		size_t left = nextFreePosition(stateNumber);
		*curPtr = left;
		++curPtr;
		addRandomNode(rand, stateNumber, left, was, curDepth + 1);


		size_t right = nextFreePosition(stateNumber);
		*curPtr = right;
		++curPtr;
		addRandomNode(rand, stateNumber, right, was, curDepth + 1);
		
		was.erase(k);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
COUNTERS_TYPE CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	const COUNTERS_TYPE* treePtr = buffer + commonDataSize + currentState*TREE_SIZE;
	size_t position = 0;
	size_t depth = 0;
	while (depth < MAX_DEPTH)
	{
		const COUNTERS_TYPE* curPtr = treePtr + position*NODE_SIZE;
		if (*curPtr == 0)
			break;
		if (input[(*curPtr) - 1])
			position = curPtr[1];
		else
			position = curPtr[2];
		++depth;
	}
	BOOST_ASSERT(treePtr[position*NODE_SIZE] == 0);
	return treePtr[position*NODE_SIZE + 1];
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
COUNTERS_TYPE CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	const COUNTERS_TYPE* treePtr = buffer + commonDataSize + currentState*TREE_SIZE;
	size_t position = 0;
	size_t depth = 0;
	while (depth < MAX_DEPTH)
	{
		const COUNTERS_TYPE* curPtr = treePtr + position*NODE_SIZE;
		if (*curPtr == 0)
			break;
		if (input[(*curPtr) - 1])
			position = curPtr[1];
		else
			position = curPtr[2];
		++depth;
	}
	BOOST_ASSERT(treePtr[position*NODE_SIZE] == 0);
	return treePtr[position*NODE_SIZE + 2];
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
COUNTERS_TYPE CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getStartState() const
{
	return buffer[0];
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getDepthAndWas(size_t curPos, size_t curDepth, std::set<size_t>& curWas, const COUNTERS_TYPE* treePtr, size_t newPosition, size_t& resultDepth, std::set<size_t>& resultWas) const
{
	if (curPos == newPosition)
	{
		resultWas = curWas;
		resultDepth = curDepth;
	}
	else
	{
		if (treePtr[curPos*NODE_SIZE] == 0)
			return;
		curWas.insert(curPos);

		getDepthAndWas(treePtr[curPos*NODE_SIZE + 1], curDepth + 1, curWas, treePtr, newPosition, resultDepth, resultWas);
		getDepthAndWas(treePtr[curPos*NODE_SIZE + 2], curDepth + 1, curWas, treePtr, newPosition, resultDepth, resultWas);

		curWas.erase(curPos);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getDepthAndWas(const COUNTERS_TYPE* treePtr, size_t newPosition, size_t& depth, std::set<size_t>& was) const
{
	std::set<size_t> tempWas;
	getDepthAndWas(0, 1, tempWas, treePtr, newPosition, depth, was);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getRealTreeSize(size_t curPos, const COUNTERS_TYPE* treePtr) const
{
	if (treePtr[curPos*NODE_SIZE] == 0)
		return 1;
	return 1 + getRealTreeSize(treePtr[curPos*NODE_SIZE + 1], treePtr) + getRealTreeSize(treePtr[curPos*NODE_SIZE + 2], treePtr);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getRealTreeSize(const COUNTERS_TYPE* treePtr) const
{
	return getRealTreeSize(0, treePtr);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::recreateTree(size_t curPos, size_t stateNumber, const COUNTERS_TYPE* treePtr, size_t nodePosition, COUNTERS_TYPE* tempTree, size_t& newPosition )
{
	size_t newPos = nextFreePosition(stateNumber);
	tempTree[newPos*NODE_SIZE] = treePtr[curPos*NODE_SIZE];
	tempTree[newPos*NODE_SIZE + 1] = treePtr[curPos*NODE_SIZE + 1];
	tempTree[newPos*NODE_SIZE + 2] = treePtr[curPos*NODE_SIZE + 2];
	if (curPos == nodePosition)
	{
		newPosition = newPos;
		return;
	}
	if (treePtr[curPos*NODE_SIZE] > 0)
	{
		recreateTree(treePtr[curPos*NODE_SIZE + 1], stateNumber, treePtr, nodePosition, tempTree, newPosition);
		recreateTree(treePtr[curPos*NODE_SIZE + 2], stateNumber, treePtr, nodePosition, tempTree, newPosition);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::recreateTree(size_t stateNumber, const COUNTERS_TYPE* treePtr, size_t nodePosition, COUNTERS_TYPE* tempTree)
{
	m_freePositions[stateNumber] = 0;
	size_t newPos;
	recreateTree(0, stateNumber, treePtr, nodePosition, tempTree, newPos);
	return newPos;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::mutate(CRandom* rand)
{
	size_t stateNumber = rand->nextUINT() % STATES_COUNT;
	COUNTERS_TYPE* treePtr = buffer + commonDataSize + stateNumber*TREE_SIZE;
	size_t treeSize = getRealTreeSize( treePtr );
	size_t nodePosition = rand->nextUINT() % treeSize;
	COUNTERS_TYPE tempTree[TREE_SIZE];
	size_t newPosition = recreateTree(stateNumber, treePtr, nodePosition, tempTree);
	memcpy(treePtr, tempTree, TREE_SIZE * sizeof(COUNTERS_TYPE));

	std::set<size_t> was;
	size_t depth;
	getDepthAndWas(treePtr, newPosition, depth, was);
	addRandomNode(rand, stateNumber, newPosition, was, depth + 1);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getHeight(const COUNTERS_TYPE* fatherTreePtr, size_t fatherNode) const
{
	if (fatherTreePtr[fatherNode*NODE_SIZE] == 0)
		return 0;
	else
		return 1 + std::max(getHeight(fatherTreePtr, fatherNode*NODE_SIZE + 1), getHeight(fatherTreePtr, fatherNode*NODE_SIZE + 2));
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::addSubTree(size_t stateNumber, COUNTERS_TYPE* dstTreePtr, size_t node, const COUNTERS_TYPE* srcTreePtr, size_t srcNode)
{
	memcpy(dstTreePtr + node, srcTreePtr + srcNode, NODE_SIZE * sizeof(COUNTERS_TYPE));
	if (srcTreePtr[srcNode * NODE_SIZE] > 0)
	{
		dstTreePtr[node*NODE_SIZE + 1] = nextFreePosition(stateNumber);
		dstTreePtr[node*NODE_SIZE + 2] = nextFreePosition(stateNumber);
		addSubTree(stateNumber, dstTreePtr, dstTreePtr[node*NODE_SIZE + 1], srcTreePtr, srcTreePtr[srcNode * NODE_SIZE + 1]);
		addSubTree(stateNumber, dstTreePtr, dstTreePtr[node*NODE_SIZE + 2], srcTreePtr, srcTreePtr[srcNode * NODE_SIZE + 2]);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand)
{
	const CAutomatDecisionTreeStatic* motherPtr = static_cast<const CAutomatDecisionTreeStatic*>(mother);
	const CAutomatDecisionTreeStatic* fatherPtr = static_cast<const CAutomatDecisionTreeStatic*>(father);

	for (size_t stateNumber = 0; stateNumber < STATES_COUNT; ++stateNumber)
	{
		COUNTERS_TYPE* childTreePtr = buffer + commonDataSize + stateNumber*TREE_SIZE;
		const COUNTERS_TYPE* motherTreePtr = motherPtr->buffer + commonDataSize + stateNumber*TREE_SIZE;
		const COUNTERS_TYPE* fatherTreePtr = fatherPtr->buffer + commonDataSize + stateNumber*TREE_SIZE;

		size_t motherSize = getRealTreeSize(motherTreePtr), fatherSize = getRealTreeSize(fatherTreePtr);
		size_t motherNode = rand->nextUINT() % motherSize, fatherNode = rand->nextUINT() % fatherSize;
		
		std::set<size_t> was;
		size_t motherDepth, fatherHeight;
		getDepthAndWas(motherTreePtr, motherNode, motherDepth, was); 
		fatherHeight = getHeight(fatherTreePtr, fatherNode);
		
		size_t counter = 0;
		while (motherDepth + fatherHeight > MAX_DEPTH)
		{
			
			fatherNode = rand->nextUINT() % fatherSize;
			fatherHeight = getHeight(fatherTreePtr, fatherNode);
			++counter;
			if (counter % 100 == 0)
				BOOST_ASSERT(false);
		}
		m_freePositions[stateNumber] = 0;
		size_t childNode = recreateTree(stateNumber, motherTreePtr, motherNode, childTreePtr);
		addSubTree(stateNumber, childTreePtr, childNode, fatherTreePtr, fatherNode);
		//checkTree(childTreePtr, 0);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>::getBufferOffset() const
{
	int* a = buffer;
	int* b = (int*)this;
	int c = a - b;
	return c;
}