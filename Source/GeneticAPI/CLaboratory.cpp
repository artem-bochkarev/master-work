#include "CLaboratory.h"

void CLaboratory::writeInfo( std::ostream& ost ) const
{
    ost << getStrategy()->getDeviceType() << 
        ", states count: " << getStates()->size() << ", " << getStrategy()->getPoolInfo() << std::endl;
}

