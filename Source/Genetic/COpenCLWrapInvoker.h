#pragma once
#include "GeneticCommon/RandomImpl.h"
#include "CGeneticStrategyCLWrap.h"
#include "GenCLWrap.h"

class CCLWrapInvoker
{
protected:
    CGeneticStrategyCLWrap* strategy;
    const CLWrapSettings& settings;
public:
    CCLWrapInvoker( CGeneticStrategyCLWrap* strg, const CLWrapSettings& settings )
        :strategy( strg ), settings(settings) {}

    virtual void operator ()();
    virtual threadPtr getThread();
private:
	CCLWrapInvoker();
};