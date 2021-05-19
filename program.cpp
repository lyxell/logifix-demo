#include "logifix/logifix.h"
#include "state.h"
#include <iostream>

extern "C" {
void run(state* s) {
    typedef std::chrono::high_resolution_clock hclock;
    const char* filename = "Example.java";
    std::string data;

    {
        const std::lock_guard<std::mutex> lock(s->mutex);
        for (const auto& line : s->lines) {
            data += line;
            data += "\n";
        }
    }

    auto program = std::make_unique<logifix::program>();
    auto t1 = hclock::now();
    //rep.run();
    program->add_string(filename, data.c_str());
    auto t2 = hclock::now();
    std::cout << "Parsing took:  "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << " milliseconds" << std::endl;
    program->run();
    auto t3 = hclock::now();
    std::cout << "Analysis took:  "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2)
                     .count()
              << " milliseconds" << std::endl;

    auto repairs = program->get_possible_rewrites(filename);
    // temporary way to not include overlapping repairs
    decltype(repairs) filtered_repairs;
    for (size_t i = 0; i < repairs.size(); i++) {
        auto [ri, ai, bi, ci, di] = repairs[i];
        bool contained = false;
        for (size_t j = 0; j < repairs.size(); j++) {
            if (i == j)
                continue;
            auto [rj, aj, bj, cj, dj] = repairs[j];
            if ((aj < ai && bj >= bi) || (aj <= ai && bj > bi)) {
                contained = true;
            }
        }
        if (!contained) {
            filtered_repairs.emplace_back(ri, ai, bi, ci, di);
        }
    }

    {
        const std::lock_guard<std::mutex> lock(s->mutex);
        if (filtered_repairs.size() == s->repairs.size()) {
            for (size_t i = 0; i < filtered_repairs.size(); i++) {
                auto& [rn, start, end, replacement, message] = filtered_repairs[i];
                s->repairs[i].start = start;
                s->repairs[i].end = end;
                s->repairs[i].message = message;
                s->repairs[i].replacement = replacement;
            }
        } else {
            s->repairs = {};
            for (auto& [rn, start, end, replacement, message] : filtered_repairs) {
                repair r;
                r.start = start;
                r.end = end;
                r.message = message;
                r.replacement = replacement;
                r.open = false;
                r.window_height = 160.0f;
                s->repairs.push_back(r);
            }
        }
        s->variables_in_scope = program->get_variables_in_scope(filename);
        s->program = std::move(program);
    }
}
}
