#include "cvision/trigger.hpp"
#include "cvision/element.hpp"

namespace cvis
{

CVTriggerTarget::CVTriggerTarget(CVElement* element,
                                 const std::string& newSignal,
                                 const unsigned int& newState):
                                     target(element),
                                     signal(newSignal),
                                     state(newState)
{
    if(target)
    {
        tag = target->tag();
    }
}

}
