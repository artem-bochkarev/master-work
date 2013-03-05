#include "CLaboratory.h"

void CLaboratory::writeInfo( std::ostream& ost ) const
{
    ost << getStrategy()->getDeviceType() << 
        ", states count: " << getStates()->size() << ",  size: " << 
        getStrategy()->getN() << "x" << getStrategy()->getM() << std::endl;
}

