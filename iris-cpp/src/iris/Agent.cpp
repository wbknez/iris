#include "iris/Agent.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "iris/Model.hpp"
#include "iris/Utils.hpp"

namespace iris
{
    State& State::operator = (const State& state)
    {
        m_behavior = state.m_behavior;
        m_time     = state.m_time;

        return *this;
    }
    
    Agent::Agent()
        : m_familySize(0), m_powerful(false), m_privilege(0), m_uid(0),
          m_bMutex(), m_iMutex()
    {}

    Agent::~Agent()
    {}

    void Agent::addConnection(AgentID to)
    {
        util::sortedInsert(m_network, to);
    }

    BehaviorList Agent::cacheBehaviorsAsSet(const Agent::Network& powerGroup,
                                            Agent* const agents,
                                            types::uint32 index,
                                            types::uint64 time)
    {
        BehaviorList cached;

        for(auto& pg : powerGroup)
        {
            const auto behav = agents[pg].getBehaviorAt(index, time);
            
            // See if it can be found in cached.
            if(std::find(cached.begin(), cached.end(), behav) == cached.end())
            {
                cached.push_back(behav);
            }
        }

        return cached;
    }

    Agent::Outcome Agent::computeOutcomeDirectly(
                                             const Agent::Sides& sides) const
    {
        return sides.first > sides.second ? Outcome::Change : Outcome::Keep;
    }

    Agent::Outcome Agent::computeOutcomeSociodynamically(const Sides &sides,
                                                 const iris::Parameters &params,
                                                 types::mersenne_twister &random)
        const
    {
        typedef std::uniform_real_distribution<types::fnumeric> FDist;
        FDist chooser(0, 1);
        
        const auto against =
            this->computeUtility(sides.first, params.m_lambda);
        const auto infavor =
            this->computeUtility(sides.second, params.m_lambda);

        const auto changeProb = params.m_resist + infavor - against;
        const auto clampedProb =
            std::min(params.m_resistMax,
                     std::max(changeProb, params.m_resistMin));
        const auto outcomeProb = chooser(random);

        return outcomeProb > clampedProb ?
            Outcome::Change : Outcome::Keep;
    }

    Agent::Sides Agent::computeSides(types::uint32 index,
                                     types::uint32 behavior,
                                     Network socialGroup,
                                     iris::Agent *const agents,
                                     AgentID totalAgents,
                                     types::uint64 time) const
    {
        auto against  =
            std::count_if(socialGroup.begin(), socialGroup.end(),
                          [index, behavior, agents, time](const AgentID& id) {
                              const auto val =
                                  agents[id].getBehaviorAt(index, time);
                              return val != behavior;
                          });
        auto infavor = socialGroup.size() - against;
        return Sides(against, infavor);
    }

    types::fnumeric Agent::computeUtility(types::fnumeric lambda,
                                          types::uint32 x) const
    {
        const auto fx = static_cast<types::fnumeric>(x);
        return 1.0 - std::exp(-1.0 * lambda * fx);
    }

    Agent::CommType Agent::determineCommType(const types::uint32 &me,
                                             const types::uint32 &you,
                                             const iris::Agent::Outcome &outcome)
    {
        if(me == you)
        {
            return outcome == Outcome::Keep ?
                CommType::Reinforced : CommType::Neither;
        }

        return outcome == Outcome::Change ?
            CommType::Censored : CommType::Neither;
    }

    void Agent::distributePrivilege(types::uint32 currentIndex,
                                    types::uint32 currentBehavior,
                                    const Network& socialGroup,
                                    const Agent::Outcome& outcome,
                                    Agent* const agents,
                                    types::uint64 time)
    {
        for(auto& soc : socialGroup)
        {
            const auto socBehavior =
                agents[soc].getBehaviorAt(currentIndex, time);
            const auto commType    =
                this->determineCommType(currentBehavior,
                                        socBehavior,
                                        outcome);

            agents[soc].updateInfluenceOn(m_uid, commType);

            if(m_powerful && (commType != CommType::Neither))
            {
                agents[soc].increasePrivilege();
            }
        }
    }

    void Agent::distributePrivilegeWithPower(types::uint32 currentIndex,
                                             types::uint32 currentBehavior,
                                             const Network& socialGroup,
                                             const Network& powerGroup,
                                             const Agent::Outcome& outcome,
                                             Agent* const agents,
                                             types::uint64 time)
    {
        // First, cache the powerful agents' behaviors.
        const auto powerCache = this->cacheBehaviorsAsSet(powerGroup,
                                                          agents,
                                                          currentIndex,
                                                          time);

        for(auto& soc : socialGroup)
        {
            const auto socBehavior =
                agents[soc].getBehaviorAt(currentIndex, time);
            const auto commType    =
                this->determineCommType(currentBehavior, socBehavior, outcome);

            agents[soc].updateInfluenceOn(m_uid, commType);

            if((std::find(powerCache.begin(), powerCache.end(), socBehavior)
                != powerCache.end()) && (commType != CommType::Neither))
            {
                agents[soc].increasePrivilege();
            }
        }
    }

    Agent::Network Agent::extractPowerful(const Agent::Network& network,
                                          iris::Agent *const agents,
                                          AgentID totalAgents)
    {
        Network powerful;

        for(Agent::Network::size_type i = 0; i < network.size(); i++)
        {
            if(agents[i].isPowerful())
            {
                powerful.push_back(i);
            }
        }

        return powerful;
    }

    BehaviorList Agent::getBehavior() const
    {
        return m_state[0].m_time > m_state[1].m_time ?
            m_state[0].m_behavior : m_state[1].m_behavior;
    }

    types::uint32 Agent::getBehaviorCount() const
    {
        return m_state[0].m_time > m_state[1].m_time ?
            m_state[0].m_behavior.size() : m_state[1].m_behavior.size();
    }

    types::uint32 Agent::getBehaviorAt(types::uint32 index,
                                       types::uint64 time)
    {
        //        mutex_guard lock(m_bMutex);

        if(m_state[0].m_time == time)
        {
            return m_state[0].m_behavior[index];
        }
        else if(m_state[1].m_time == time)
        {
            //            m_bMutex.unlock();
            return m_state[1].m_behavior[index];
        }

        throw std::runtime_error("No behavior at time: " +
                                 util::toString(time));
    }

    Agent::InteractionMap::iterator Agent::getInteractionsWith(
                                                             const AgentID& id)
    {
        auto comm = m_interactions.find(id);
        if(comm == m_interactions.end())
        {
            const auto interact = std::pair<AgentID, Interaction>(id, {});
            auto handle =  m_interactions.insert(interact);
            comm = handle.first;
        }
        return comm;
    }

    types::uint32 Agent::getFamilyConnections() const
    {
        return m_familySize - 1;
    }
    
    types::uint32 Agent::getFamilySize() const
    {
        return m_familySize;
    }

    Agent::InteractionMap Agent::getInteractions() const
    {
        return m_interactions;
    }

    Agent::Network Agent::getNetwork() const
    {
        return m_network;
    }
    
    types::unumeric Agent::getPrivilege() const
    {
        return m_privilege;
    }

    AgentID Agent::getUId() const
    {
        return m_uid;
    }

    ValueList Agent::getValues() const
    {
        return m_values;
    }

    void Agent::increasePrivilege()
    {
        m_privilege++;
    }
    
    bool Agent::isConnectedTo(AgentID to)
    {
        return std::find(m_network.begin(), m_network.end(), to) !=
            m_network.end();
    }

    bool Agent::isNetworkFull(types::uint32 outConnections,
                              AgentID totalAgents) const
    {
        const auto familySize = this->getFamilyConnections();
        const auto upperBound =
            ((outConnections + familySize) > (totalAgents - 1)) ?
            (totalAgents - 1) : (outConnections + familySize);
        const auto remainder = (upperBound - m_network.size());

        return (remainder == 0 || remainder >= upperBound);
    }

    bool Agent::isPowerful() const
    {
        return m_powerful;
    }

    Agent::Network Agent::obtainRandomInfluentialGroup(types::uint32 qIn,
                                                       types::uint32 qOut,
                                                       Agent* const agents,
                                                       AgentID totalAgents,
                                                types::mersenne_twister& random)
  {
      const auto inGroup  = this->obtainRandomInGroup(qIn, random);
      auto       outGroup = this->obtainRandomOutGroup(qOut, totalAgents,
                                                       random);

      if(m_powerful)
      {
          this->removeNonPowerful(outGroup, agents, totalAgents);          
      }
      
      Network influential;

      influential.reserve(inGroup.size() + outGroup.size());
      influential.insert(influential.end(), inGroup.begin(), inGroup.end());
      influential.insert(influential.end(), outGroup.begin(), outGroup.end());

      return influential;
  }

    Agent::Network Agent::obtainRandomInGroup(types::uint32 qIn,
                                              types::mersenne_twister& random)
    {
        // This is one of those excellent cases where we abuse the stack.
        Network network(m_network.begin(), m_network.end());
        std::shuffle(network.begin(), network.end(), random);

        // Trim to however many are necessary.
        if(qIn < network.size())
        {
            network.resize(qIn);
        }

        return network;
    }

    Agent::Network Agent::obtainRandomOutGroup(types::uint32 qOut,
                                               AgentID totalAgents,
                                               types::mersenne_twister& random)
    {
        typedef std::uniform_int_distribution<AgentID> UintDist;
        
        const auto networkBound = totalAgents - m_network.size() - 1;
        const auto upperBound   = (qOut > networkBound) ? networkBound : qOut;

        Network  network(m_network.begin(), m_network.end());
        Network  outGroup;
        UintDist chooser(0, totalAgents - 1);

        // Add current id.
        util::sortedInsert(network, m_uid);
        
        for(AgentID i = 0; i < upperBound; i++)
        {
            try
            {
                const auto nextAgent =
                    util::ensureRandom(chooser(random), network,
                                       (AgentID)0, totalAgents);
                util::sortedInsert(network, nextAgent);
                outGroup.push_back(nextAgent);
            }
            catch(std::runtime_error& re)
            {
                // Nothing to do here, just continue.
            }
        }

        return outGroup;
    }

    void Agent::removeNonPowerful(Agent::Network &network,
                                  iris::Agent *const agents,
                                  AgentID totalAgents)
    {
        auto iter = std::remove_if(network.begin(), network.end(),
            [agents](const AgentID& id){ return !agents[id].isPowerful(); });
        network.erase(iter, network.end());
    }

    types::uint32 Agent::selectNewBehavior(types::uint32 currentBehavior,
                                           types::uint32 behaviorRange,
                                           types::mersenne_twister& random)
    {
        using namespace iris::types;
        using namespace iris::util;
        
        if(behaviorRange == 0 || behaviorRange == 1)
        {
            return currentBehavior;
        }
        
        typedef std::uniform_int_distribution<types::uint32> UintDist;        
        auto       chooser      = UintDist{0, behaviorRange - 1};
        const auto chosen       = chooser(random);
        const auto behaviorList = Uint32List{currentBehavior};

        return ensureRandom<uint32>(chosen, behaviorList,
                                    0, behaviorRange);
    }

    void Agent::setFamilySize(types::uint32 familySize)
    {
        m_familySize = familySize;
    }
    
    void Agent::setInitialBehavior(BehaviorList behavior)
    {
        // For this, set *both* to t = 0 to avoid potential problems.
        m_state[0].m_behavior = behavior;
        m_state[0].m_time = 0;
        m_state[1].m_behavior = behavior;
        m_state[1].m_time = 0;
    }

    void Agent::setInitialValues(ValueList values)
    {
        m_values = values;
    }

    void Agent::setPowerful(bool isPowerful)
    {
        m_powerful = isPowerful;
    }

    void Agent::setUId(AgentID uid)
    {
        m_uid = uid;
    }

    void Agent::step(const Parameters &params, iris::Agent *const agents,
                     AgentID totalAgents, const BehaviorList& behaviors,
                     types::uint64 time, types::mersenne_twister& random)
    {
        typedef std::uniform_int_distribution<types::uint32> UintDist;
#ifdef IRIS_DEBUG
        std::cout << "Prevar check: " << behaviors.size() << std::endl;
#endif
        /*
         * The actions each agent takes per simulation step are as follows:
         *   1) Collect randomized social group.
         *     i) Collect random in-group from social network.
         *     ii) Collect random out-group from outside social network.
         *       a) If powerful, then remove all non-powerful agents from
         *       out-group.
         *   2) Compute a random behavior to "inspect".
         *   3) Compute whether or not to change this agent's behavior.
         *     i) If powerful OR not powerful and with no powerful agents
         *     present, use a utility function to perform a sociodynamic
         *     response.
         *     ii) If not powerful AND powerful agents are present, use
         *     those agents to perform a game theory response.
         *   4) Assign privilege to and update communication information of
         *   all agents that participated.
         *     i) If not powerful AND no powerful agents are present, then
         *     no privilege is assigned.
         *     ii) If not powerful AND powerful agents are present, then
         *     privilege is assigned for all agents that match each powerful
         *     one (that has the correct )
         *
         * Each of these steps is broken down into sub-functions to make them
         * easy to unit test.
         */
        const auto socialGroup =
          this->obtainRandomInfluentialGroup(params.m_qIn, params.m_qOut, agents,
                                             totalAgents, random);
        const auto powerGroup  =
          this->extractPowerful(socialGroup, agents, totalAgents);

#ifdef IRIS_DEBUG
        std::cout << "Number of members in social group: "
                  << socialGroup.size() << std::endl;
        std::cout << "Number of members in power group: "
                  << powerGroup.size() << std::endl;

        for(auto& soc : socialGroup)
        {
            std::cout << soc << " ";
        }
        std::cout << std::endl;
#endif
        
        const auto numBehaviors    =
            static_cast<types::uint32>(behaviors.size());
        auto indexChooser          = UintDist(0, numBehaviors - 1);
        const auto inspectIndex    = indexChooser(random);
        const auto inspectBehav    = this->getBehaviorAt(inspectIndex, time - 1);

        // (3) Determine a social outcome.
        Agent::Outcome outcome;
        
        if(m_powerful || (!m_powerful && powerGroup.size() == 0))
        {
            const auto sides = this->computeSides(inspectIndex, inspectBehav,
                                                  socialGroup, agents,
                                                  totalAgents, time - 1);
            outcome          =
                this->computeOutcomeSociodynamically(sides, params, random);

            // Assign privilege and update.
            this->distributePrivilege(inspectIndex, inspectBehav,
                                      socialGroup, outcome,
                                      agents, time - 1);
        }
        else
        {
            const auto sides = this->computeSides(inspectIndex, inspectBehav,
                                                  powerGroup, agents,
                                                  totalAgents, time - 1);
            outcome          = this->computeOutcomeDirectly(sides);
            this->distributePrivilegeWithPower(inspectIndex, inspectBehav,
                                               socialGroup, powerGroup,
                                               outcome, agents, time - 1);
        }

        // Change behaviors if necessary.
        if(outcome == Outcome::Change)
        {
            const auto newBehavior =
                this->selectNewBehavior(inspectBehav, behaviors[inspectIndex],
                                        random);
            this->updateState(inspectIndex, newBehavior, time);
        }
        else
        {
            this->updateState(inspectIndex, inspectBehav, time);
        }

        // Update our own privilege.
        if(outcome == Outcome::Keep && (m_powerful || powerGroup.size() != 0))
        {
            this->increasePrivilege();
        }

        // Finally, update our own map.
        this->updateCommunicationWith(socialGroup);
    }

    void Agent::updateCommunicationWith(const Network& network)
    {
        //        mutex_guard lock(m_iMutex);        
        for(auto& otherId : network)
        {
            auto comm = this->getInteractionsWith(otherId);
            (*comm).second.m_communicated++;
        }
    }

    void Agent::updateInfluenceOn(const AgentID& targetId,
                                  const CommType& commType)
    {
        //        mutex_guard lock(m_iMutex);
        auto comm = this->getInteractionsWith(targetId);
        
        switch(commType)
        {
            case Censored:
                (*comm).second.m_censored++;
                break;
            case Neither:
                break;
            case Reinforced:
                (*comm).second.m_reinforced++;
                break;
        }

        (*comm).second.m_communicated++;
    }

    void Agent::updateState(types::uint32 index, types::uint32 behavior,
                            types::uint64 time)
    {
        //        mutex_guard lock(m_bMutex);

        // Copy "current" state to previous.
        m_state[1] = m_state[0];

        // Update "current".
        m_state[0].m_behavior[index] = behavior;
        m_state[0].m_time = time;
    }
}
