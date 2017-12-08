/* Copyright Fabrice Triboix - Please read the LICENSE file */

#include <skal/skal.hpp>
#include <vector>
#include <sstream>

int main()
{
    constexpr const int nworkers = 100; // Number of workers
    constexpr const int nmsg = 10'000;  // Number of messages
    std::vector<int> counts;
    counts.resize(nworkers, 0);

    struct job_t {
        int n;
        std::string next;
        int& count;

        job_t(int _n, int& _count) : n(_n), count(_count) {
            std::ostringstream oss;
            oss << "worker" << (n+1);
            next = oss.str();
        }

        bool operator()(std::unique_ptr<skal::msg_t> msg) {
            if (msg->action() == "stress") {
                ++count;
                if (msg->has_int("last")) {
                    msg = skal::msg_t::create(next, "stress");
                    msg->add_field("last", 1);
                    skal::send(std::move(msg));
                    return false;
                }
                skal::send(skal::msg_t::create(next, "stress"));
            }
            return true;
        }
    };

    for (int i = 0; i < nworkers; ++i) {
        std::ostringstream oss;
        oss << "worker" << i;
        skal::worker_t::create(oss.str(), job_t(i, counts[i]));
    }

    int source_count = 0;
    skal::worker_t::create("source",
            [nmsg, &source_count] (std::unique_ptr<skal::msg_t> msg)
            {
                if (msg->action() == "kick") {
                    msg = skal::msg_t::create("worker0", "stress");
                    ++source_count;
                    if (source_count >= nmsg) {
                        msg->add_field("last", 1);
                    }
                    skal::send(std::move(msg));
                    if (source_count >= nmsg) {
                        return false;
                    }
                }
                skal::send(skal::msg_t::create("source", "kick"));
                return true;
            });

    skal::wait();
    return 0;
}
