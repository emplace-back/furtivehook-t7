#pragma once
#include "dependencies/std_include.hpp"

namespace session
{
    namespace simulate_join
    {
        extern bool enabled;
        extern bool ready;
    }
    
    game::MatchMakingInfo* get(const uint64_t session_xuid);
    void register_session(const game::MatchMakingInfo& session);
    void draw_session_list(const float width, const float spacing);
    void initialize();
}