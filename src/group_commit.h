// C++'s version of https://github.com/tikv/raft-engine

#include <memory>
#include <optional>
#include <mutex>
#include <condition_variable>
#include <string>

template <typename P, typename O>
struct Writer {
    using Ptr = std::shared_ptr<Writer<P, O>>;
    using MaybePtr = std::optional<Ptr>;

    Writer(P* payload_) : payload(payload_), nxt(std::nullopt) {

    }

    P& mut_payload() {
        return *payload;
    }

    void set_output(O && output_) {
        this->out = std::move(output_);
    }

    const O & output() {
        return out;
    }

    MaybePtr next() {
        return this->nxt;
    }

    void set_next(MaybePtr next_) {
        this->nxt = next_;
    }

protected:
    MaybePtr nxt;
    P* payload;
    O out;
};

template <typename P, typename O>
struct WriterGroup;

template <typename T>
std::optional<T> Some(T inner) {
    return std::optional<T>(std::move(inner));
}

template <typename P, typename O>
struct WriteBarrier {
    // Returns none if we are not the leader, and the group is committed.
    // Returns some(g) if we are the leader, and we need to commit this group.
    std::optional<WriterGroup<P, O>> enter(typename Writer<P, O>::Ptr writer, uint64_t tag) {
        auto node = Some(writer);
        LOG("[thread %lld] Enter with %llu\n", tag, writer->mut_payload().data);
        std::unique_lock<std::mutex> guard{ this->mut };
        if (this->tail.has_value()) {
            this->tail.value()->set_next(node);
            this->tail = node;

            if (this->pending_leader.has_value()) {
                LOG("[thread %lld] Follower of next\n", tag);
                // Follower of next write group.
                this->follower_cvs[this->pending_index % 2].wait(guard);
                return std::nullopt;
            } else {
                // Leader of next write group.
                LOG("[thread %lld] Leader of next\n", tag);
                this->pending_leader = node;
                this->pending_index ++;
                this->pending_index %= 2;
                this->leader_cv.wait(guard);
                // Now it is our turn.
                LOG("[thread %lld] Now my turn\n", tag);
                this->pending_leader = std::nullopt;
            }
        } else {
            // Just form a new group and be current leader.
            assert(!this->pending_leader.has_value());
            LOG("[thread %lld] Empty\n", tag);
            this->head = node;
            this->tail = node;
        }

        LOG("[thread %lld] Form group with start %llu back %llu\n", tag, node.value()->mut_payload().data, this->tail.value()->mut_payload().data);
        // Now I am the current group leader.
        return Some(WriterGroup<P, O>{
            node,
            this->tail,
            this,
            tag
        });
    }

    void after_commit(uint64_t tag) {
        std::unique_lock<std::mutex> guard{ this->mut };
        if (this->pending_leader.has_value()) {
            LOG("[thread %lld] Exit with pending leader\n", tag);
            // Notify the next group.
            this->leader_cv.notify_one();
            // Wait up our followers. 
            // Notice pending_index is now at next write group.
            this->follower_cvs[(this->pending_index-1) % 2].notify_all();
            this->head = this->pending_leader.value();
        } else {
            LOG("[thread %lld] Exit with no pending leader\n", tag);
            this->follower_cvs[this->pending_index % 2].notify_all();
            this->head = std::nullopt;
            this->tail = std::nullopt;
        }
    }

    std::mutex mut;
    typename Writer<P, O>::MaybePtr head;
    typename Writer<P, O>::MaybePtr tail;
    typename Writer<P, O>::MaybePtr pending_leader;
    size_t pending_index;

    std::condition_variable leader_cv;
    std::condition_variable follower_cvs[2];
};

template <typename P, typename O>
struct WriterGroup {
    typename Writer<P, O>::MaybePtr start;
    typename Writer<P, O>::MaybePtr back;
    WriteBarrier<P, O> * barrier;
    uint64_t tag;

    void finish() {
        barrier->after_commit(tag);
    }
};

