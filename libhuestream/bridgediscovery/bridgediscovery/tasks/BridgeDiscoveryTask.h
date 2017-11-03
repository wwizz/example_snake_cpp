/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_TASKS_BRIDGEDISCOVERYTASK_H_
#define BRIDGEDISCOVERY_TASKS_BRIDGEDISCOVERYTASK_H_

#include <vector>
#include <queue>
#include <memory>
#include <unordered_map>
#include <string>

#include "threading/Job.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"

using Task = huesdk_lib::JobTask;

namespace huesdk {

    class BridgeDiscoveryTask : public Task {
    public:
        /**
         Constructor
         @param discovery_methods discovery methods to use for bridge searching
         */
        explicit BridgeDiscoveryTask(std::vector<std::unique_ptr<IBridgeDiscoveryMethod>> &&discovery_methods);

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         @see Job.h
         */
        void stop() override;

        /**
         Gets task execution result
         @return const reference to result array
         */
        std::vector<BridgeDiscoveryResult> get_results() const;

    private:
        /**
         Starts executing next search method in the queue. If the queue is empty, task finishes.
         */
        void start_next_search_method();

        /**
         Filters out already existing results from the input and runs BridgeDiscoveryCheckIpArrayTask with new results
         @param discovery_results discovery method results to be checked
         */
        void perform_ip_check_and_continue(const std::vector<BridgeDiscoveryResult> &discovery_results);

        /**
         Saves discovery results to result map. Pops current discovery result from the queue.
         @param discovery_results discovery method results to be processed
         */
        void process_results_and_continue(const std::vector<BridgeDiscoveryResult> &discovery_results);

        huesdk_lib::QueueExecutor _executor;
        std::queue<std::unique_ptr<IBridgeDiscoveryMethod>> _current_discovery_methods;

        std::unordered_map<std::string, BridgeDiscoveryResult> _results;

        CompletionHandler _done;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_TASKS_BRIDGEDISCOVERYTASK_H_
