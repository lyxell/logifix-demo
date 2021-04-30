#include "state.h"
#include <iostream>
#include "datalog-repair/repair.h"

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
        auto p = rep.get_ast(filename);
        // terrible, terrible hack
        auto* x = new std::vector<std::shared_ptr<sjp::tree_node>>();
        x->push_back(p);
        s->ast = p;
        //s->show_demo_window = !s->show_demo_window;
    }
}
