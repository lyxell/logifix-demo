#include "squarelog/repair.h"
#include "state.h"
#include <iostream>

extern "C" {
void run(state* s) {
    const char* filename = "Example.java";
    std::string data;

    {
        const std::lock_guard<std::mutex> lock(s->mutex);
        for (const auto& line : s->lines) {
            data += line;
            data += "\n";
        }
    }

    repair rep;
    rep.add_string(filename, data.c_str());
    rep.run();
    auto repairs = rep.get_possible_repairs(filename);
    // temporary way to not include overlapping repairs
    decltype(repairs) filtered_repairs;
    for (size_t i = 0; i < repairs.size(); i++) {
        auto [ai, bi, ci, di] = repairs[i];
        bool contained = false;
        for (size_t j = 0; j < repairs.size(); j++) {
            if (i == j)
                continue;
            auto [aj, bj, cj, dj] = repairs[j];
            if ((aj < ai && bj >= bi) || (aj <= ai && bj > bi)) {
                contained = true;
            }
        }
        if (!contained) {
            filtered_repairs.emplace_back(ai, bi, ci, di);
        }
    }
    auto p = rep.get_ast(filename);
    // terrible, terrible hack
    auto* x = new std::vector<std::shared_ptr<sjp::tree_node>>();
    x->push_back(p);

    {
        const std::lock_guard<std::mutex> lock(s->mutex);
        s->ast = p;
        s->repairs = {};
        for (auto& [a, b, c, d] : filtered_repairs)
            s->repairs.emplace_back(a,b,c,d,false);
        s->variables_in_scope = rep.get_variables_in_scope(filename);
    }
}
}
