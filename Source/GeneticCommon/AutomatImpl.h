#pragma once
//#include "GeneticParams.h"
#include "AntCommon.h"
#include "GeneticAPI/CAutomat.h"

template <typename COUNTERS_TYPE, typename INPUT_TYPE>
class CAutomatImpl;

template <typename COUNTERS_TYPE, typename INPUT_TYPE, typename C = CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE> >
using CAutomatImplPtr = boost::shared_ptr< C >;

template <typename COUNTERS_TYPE, typename INPUT_TYPE>
class CAutomatImpl : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatImpl(CAntCommon<COUNTERS_TYPE>* pAntCommon);
    CAutomatImpl( const CAutomatImpl& automat );
    CAutomatImpl& operator = ( const CAutomatImpl& automat );
    virtual void generateRandom( CRandom* rand ) override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, INPUT_TYPE* input) const;
	virtual COUNTERS_TYPE getStartState() const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, INPUT_TYPE* input) const;
	size_t countIndex(const std::vector<INPUT_TYPE>& input) const;
	size_t countIndex(INPUT_TYPE* input) const;
    virtual void mutate( CRandom* rand ) override;
    virtual void crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand ) override;

	static void fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, COUNTERS_TYPE* buff, CRandom* rand);
    static std::vector< CAutomatImplPtr<COUNTERS_TYPE, INPUT_TYPE> > cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatImplPtr<COUNTERS_TYPE, INPUT_TYPE> createFromBuffer(CAntCommon<COUNTERS_TYPE>* pAntCommon, COUNTERS_TYPE* buf);

    virtual ~CAutomatImpl();
private:
    CAutomatImpl() {};
	size_t stateSize, startState;
	COUNTERS_TYPE * buffer;
	CAntCommon<COUNTERS_TYPE>* pAntCommon;
};

#include "AutomatImpl.hpp"