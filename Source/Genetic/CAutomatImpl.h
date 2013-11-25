#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CAutomat.h"

class CAutomatImpl;
typedef boost::shared_ptr<CAutomatImpl> CAutomatImplPtr;

class CAutomatImpl : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatImpl(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions);
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

	static void fillRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, char* buff,
        size_t stSize, CRandom* rand );
    static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatImpl createFromBuffer(CStateContainer<COUNTERS_TYPE>* states,
		CActionContainer<COUNTERS_TYPE>* actions, char* buf, size_t stateSize = 16);

    virtual ~CAutomatImpl();
private:
    CAutomatImpl() {};
	size_t stateSize, startState;
	COUNTERS_TYPE * buffer;
};