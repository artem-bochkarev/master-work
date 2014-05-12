#pragma once
#include "AntCommon.h"
#include <set>
#include <map>


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
class CAutomatDecisionTreeStatic;


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT,
	typename C = CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT> >
	using CAutomatDecisionTreeStaticPtr = boost::shared_ptr< C >;

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t MAX_DEPTH, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
class CAutomatDecisionTreeStatic final : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatDecisionTreeStatic();
	CAutomatDecisionTreeStatic(CAntCommon<COUNTERS_TYPE>* pAntCommon);// size_t stateSize);
	CAutomatDecisionTreeStatic(const CAutomatDecisionTreeStatic& automat);
	virtual ~CAutomatDecisionTreeStatic();
	CAutomatDecisionTreeStatic& operator = (const CAutomatDecisionTreeStatic& automat);
	virtual void generateRandom(CRandom* rand) override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getStartState() const override;

	virtual void mutate(CRandom* rand) override;
	virtual void crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand) override;

	static void fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, char* buff, CRandom* rand);
	static char generateRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, CRandom* rand);
	//static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatDecisionTreeStaticPtr<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT>
		createFromBuffer(CAntCommon<COUNTERS_TYPE>* pAntCommon, COUNTERS_TYPE* buf);

	size_t getBufferOffset() const;
	const CAntCommon<COUNTERS_TYPE>* getAntCommon() const
	{
		return pAntCommon;
	}
protected:
	static const size_t NODE_SIZE = 3;
	static const size_t N = (1 << MAX_DEPTH);
	static const size_t TREE_SIZE = NODE_SIZE * (N * (N - 1) / 2);

	static const size_t commonDataSize = 4;
	static const size_t stateSize = TREE_SIZE;
	static const size_t bufferSize = STATES_COUNT * stateSize + commonDataSize;

private:
	void addRandomNode(CRandom* rand, size_t stateNumber, size_t position, std::set<size_t>& was, size_t curDepth);
	void getDepthAndWas(const COUNTERS_TYPE* treePtr, size_t newPosition, size_t& depth, std::set<size_t>& was) const;
	void getDepthAndWas(size_t curPos, size_t curDepth, std::set<size_t>& curWas, const COUNTERS_TYPE* treePtr, size_t newPosition, size_t& resultDepth, std::set<size_t>& resultWas) const;
	size_t getHeight(const COUNTERS_TYPE* fatherTreePtr, size_t fatherNode) const;
	size_t getRealTreeSize(size_t curPos, const COUNTERS_TYPE* treePtr) const;
	size_t getRealTreeSize(const COUNTERS_TYPE* treePtr) const;
	size_t recreateTree(size_t curState, size_t stateNumber, const COUNTERS_TYPE* treePtr, size_t nodePosition, COUNTERS_TYPE* tempTree, size_t& newPosition);
	size_t recreateTree(size_t stateNumber, const COUNTERS_TYPE* treePtr, size_t nodePosition, COUNTERS_TYPE* tempTree);
	void addSubTree(size_t stateNumber, COUNTERS_TYPE* dstTreePtr, size_t node, const COUNTERS_TYPE* srcTreePtr, size_t srcNode);
	size_t nextFreePosition(size_t stateNumber);
	std::map<size_t, size_t> m_freePositions;

	CAntCommon<COUNTERS_TYPE>* pAntCommon;
	COUNTERS_TYPE startState;
	mutable COUNTERS_TYPE buffer[bufferSize];
};