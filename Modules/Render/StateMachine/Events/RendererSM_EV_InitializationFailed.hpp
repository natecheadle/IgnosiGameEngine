#pragma once

#include <boost/statechart/event.hpp>

namespace sc = boost::statechart;

namespace nate::Modules::Render
{
    class RendererSM_EV_InitializationFailed : public sc::event<RendererSM_EV_InitializationFailed>
    {
      public:
        RendererSM_EV_InitializationFailed()  = default;
        ~RendererSM_EV_InitializationFailed() = default;
    };
} // namespace nate::Modules::Render