
void ignore(...) { }
#define LOG ignore
#include "group_commit.h"
#include <vector>
#include <thread>
#include <future>
#include <map>
#include <cstdio>
#include <chrono>

struct WriteBatch {
    uint64_t data;
};

struct Context {
    WriteBarrier<WriteBatch, uint64_t> barrier;
    std::map<uint64_t, uint64_t> sink;
};

int main() {
    Context ctx;
    std::vector<std::future<uint64_t>> futs;

    int total_threads = 10;
    int total_iter = 100;
    int total = std::max(total_threads, total_iter);
    srand(time(0));

    auto f = [&](uint64_t from, uint64_t to, uint64_t tag) {
        uint64_t local_sink;
        for (auto iter = from; iter < to; iter ++) {
            std::this_thread::sleep_for(std::chrono::milliseconds{rand() % 100});
            WriteBatch wb { iter };
            auto writer = std::make_shared<Writer<WriteBatch, uint64_t>>(&wb);
            auto maybe_wg = ctx.barrier.enter(writer, tag);
            if (maybe_wg.has_value()) {
                // Leader is locked to write.
                LOG("[thread %lld] Leader when writing %llu\n", tag, iter);
                decltype(auto) wg = maybe_wg.value();
                for (auto begin = wg.start; ; begin = begin.value()->next()) {
                    auto key = begin.value()->mut_payload().data;
                    if (ctx.sink.count(key)) {
                        LOG("[thread %lld] Duplicate %llu\n", tag, key);
                        exit(1);
                    } else {
                        uint64_t value = uint64_t(rand()) % 100;
                        LOG("[thread %lld] Write For %llu Key %llu Value %llu\n", tag, key / total, key, value);
                        ctx.sink[key] = value;
                        begin.value()->set_output(std::move(value));
                    }
                    if (begin.value()->mut_payload().data == wg.back.value()->mut_payload().data) {
                        break;
                    }
                }
                LOG("[thread %lld] Write Finish\n", tag);
                wg.finish();
            } else {
                // Follower is notified finished write.
                LOG("[thread %lld] Follower at %llu\n", tag, iter);
            }
            LOG("[thread %lld] Receive Write For %llu Key %llu Value %llu\n", tag, iter / total, iter, writer->output());
            local_sink += writer->output();
        }
        LOG("[thread %lld] Sum up to %llu\n", tag, local_sink);
        return local_sink;
    };

    for (uint64_t i = 0; i < total_threads; i++) {
        futs.push_back(std::async(f, i * total, (i + 1) * total, i));
    }

    std::map<int, uint64_t> ans;
    for (uint64_t i = 0; i < total_threads; i++) {
        ans[i] = futs[i].get();
    }

    for (uint64_t i = 0; i < total_threads; i++) {
        uint64_t expected = 0;
        for (uint64_t s = i * total; s < (i + 1) * total; s++) {
            // LOG("[thread %lld] DETAIL!!! ctx.sink[%llu]=%llu\n", i, s, ctx.sink[s]);
            expected += ctx.sink[s];
        }
        // LOG("[thread %lld] RES!!! expected %llu actual %llu\n", i, expected, ans[i]);
        assert(ans[i] == expected);
    }
}