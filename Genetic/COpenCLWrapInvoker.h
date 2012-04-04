#pragma once
#include "CRandomImpl.h"
#include "CGeneticStrategyCLWrap.h"

struct CLWrapSettings
{
	size_t N, M, step;
	size_t flowsCnt;
	CRandomPtr rand;
};

class CCLWrapInvoker
{
protected:
    CGeneticStrategyCLWrap* strategy;
    const CLWrapSettings& settings;
public:
    CCLWrapInvoker( CGeneticStrategyCLWrap* strg, const CLWrapSettings& settings )
        :strategy( strg ), settings(settings) {}

    virtual void operator ()();
    virtual threadPtr getThread() const;
private:
	CCLWrapInvoker();
	CCLWrapInvoker( const CCLWrapInvoker& );
};