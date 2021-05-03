#include "datalog-repair/repair.h"
#include "state.h"
#include <iostream>

extern "C" {
void run(state* s) {
    const char* filename = "Example.java";
    repair rep;
    std::string data;
    for (const auto& line : s->lines) {
        data += line;
        data += "\n";
    }
    rep.add_string(filename, data.c_str());
    rep.run();
    s->repairs = rep.get_possible_repairs(filename);
    // temporary way to not include overlapping repairs
    decltype(s->repairs) result;
    for (size_t i = 0; i < s->repairs.size(); i++) {
        auto [ai, bi, ci] = s->repairs[i];
        bool contained = false;
        for (size_t j = 0; j < s->repairs.size(); j++) {
            if (i == j) continue;
            auto [aj, bj, cj] = s->repairs[j];
            if ((aj < ai && bj >= bi) || (aj <= ai && bj > bi)) {
                contained = true;
            }
        }
        if (!contained) {
            result.emplace_back(ai, bi, ci);
        }
    }
    s->repairs = result;
    auto p = rep.get_ast(filename);
    // terrible, terrible hack
    auto* x = new std::vector<std::shared_ptr<sjp::tree_node>>();
    x->push_back(p);
    s->ast = p;
}
}
