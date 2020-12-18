/*
 *  Copyright 2020 juteman
 *
 *  This file is a part of Hawl
 *  see(https://github.com/juteman/Hawl)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#include <chrono>
#include "tbb/tbb.h"
#include "BaseType.h"
#include "Logger.h"

using namespace tbb;

namespace Hawl {
    class ParallelLogger {
    public:
        ParallelLogger() = default;

        void operator()(const blocked_range<uint32> &r) const {
            for (uint32 i = r.begin(); i != r.end(); ++i) {
                Logger::info("current range is {}", i);
            }
        }
    };

}

int main() {
    using namespace Hawl;
    parallel_for(blocked_range<uint32>(0, 16), Hawl::ParallelLogger());
    uint64 count = 0;
    auto start = std::chrono::high_resolution_clock::now();
    parallel_for(blocked_range<uint32>(0, 100), [&](const blocked_range<uint32> &r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            //if (i % 7 == 0) {
                count += 1;
           // }
        }
    });
    auto end = std::chrono::high_resolution_clock::now();
    Logger::info("int 32 bit length contain {} can divide 7 , time cost {} -- parallel auto", count,
                 std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

    count = 0;
    start = std::chrono::high_resolution_clock::now();
    parallel_for(blocked_range<uint32>(0, 100, 1), [&](const blocked_range<uint32> &r) {
        for (int i = r.begin(); i < r.end(); ++i) {
       //     if (i % 7 == 0) {
                count += 1;
        //    }
        }
    }, simple_partitioner());

    end = std::chrono::high_resolution_clock::now();

    Logger::info("int 32 bit length contain {} can divide 7 , time cost {} -- parallel with grain size 1", count,
                 std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());


    count = 0;
    start = std::chrono::high_resolution_clock::now();
    parallel_for(blocked_range<uint32>(0, 100, 16), [&](const blocked_range<uint32> &r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            //if (i % 7 == 0) {
                count += 1;
            //}
        }
    }, simple_partitioner());
    end = std::chrono::high_resolution_clock::now();

    Logger::info(
            "int 32 bit length contain {} can divide 7 , time cost {} -- parallel with grain size 16 same as cpu core",
            count,
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());


    count = 0;
    start = std::chrono::high_resolution_clock::now();
    parallel_for(blocked_range<uint32>(0, 100, 32), [&](const blocked_range<uint32> &r) {
        for (int i = r.begin(); i < r.end(); ++i) {
           // if (i % 7 == 0) {
                count += 1;
           // }
        }
    }, simple_partitioner());
    end = std::chrono::high_resolution_clock::now();

    Logger::info(
            "int 32 bit length contain {} can divide 7 , time cost {} -- parallel with grain size 32 twice as cpu core",
            count,
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());


    count = 0;
    start = std::chrono::high_resolution_clock::now();
    parallel_for(blocked_range<uint32>(0, 100, 20000), [&](const blocked_range<uint32> &r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            // if (i % 7 == 0) {
            count += 1;
            // }
        }
    }, simple_partitioner());
    end = std::chrono::high_resolution_clock::now();

    Logger::info(
            "int 32 bit length contain {} can divide 7 , time cost {} -- parallel with grain size 20000 twice as cpu core",
            count,
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());

    count = 0;
    affinity_partitioner affinityPartitioner;
    start = std::chrono::high_resolution_clock::now();
    parallel_for(blocked_range<uint32>(0, 100), [&](const blocked_range<uint32> &r) {
        for (int i = r.begin(); i < r.end(); ++i) {
            //if (i % 7 == 0) {
            count += 1;
            // }
        }
    }, affinityPartitioner);
    end = std::chrono::high_resolution_clock::now();
    Logger::info("int 32 bit length contain {} can divide 7 , time cost {} -- parallel auto", count,
                 std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());


    return 0;
}
