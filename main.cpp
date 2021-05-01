#include "state.h"
#include "dockspace.h"
#include "editor.h"
#include "imgui-boilerplate/window.h"
#include "imgui.h"
#include <dlfcn.h>
#include <iostream>

const char* libpath = "./libprogram.so";

int main() {
    void* handle;
    void* render_function;
    state s = {
        .lines = {
            "import java.util.ArrayList;",
            "",
            "public class Main {",
            "  public static void main(String[] args) {",
            "    // S1155",
            "    ArrayList<Integer> x = new ArrayList<>();",
            "    if (x.size() == 0) {",
            "      System.out.println(\"empty\");",
            "    }",
            "  }",
            "  public static void test1() {",
            "    // S1132",
            "    String myString = null;",
            "    // Non-Compliant - will raise a NPE",
            "    System.out.println(\"Equal? \" + myString.equals(\"foo\"));",
            "    // Non-Compliant - null check could be removed",
            "    System.out.println(\"Equal? \" + (myString != null && myString.equals(\"foo\")));",
            "    // Compliant - properly deals with the null case",
            "    System.out.println(\"Equal?\" + \"foo\".equals(myString));",
            "  }",
            "  public static void test2() {",
            "    // S1596",
            "    // Noncompliant",
            "    List<String> collection1 = Collections.EMPTY_LIST;",
            "    // Noncompliant",
            "    Map<String, String> collection2 = Collections.EMPTY_MAP;",
            "    // Noncompliant",
            "    Set<String> collection3 = Collections.EMPTY_SET;",
            "  }",
            "  public static void test3() {",
            "    // S1125",
            "    if (booleanMethod() == true) {}",
            "    if (booleanMethod() == false) {}",
            "    doSomething(booleanMethod() == true);",
            "  }",
            "  public static void test4() {",
            "    // S2111",
            "    double d = 1.1;",
            "    // Noncompliant",
            "    BigDecimal bd1 = new BigDecimal(d);",
            "    // Noncompliant",
            "    BigDecimal bd2 = new BigDecimal(1.1);",
            "  }",
            "  public static void test5() {",
            "    // think about how to merge these into one rewrite",
            "    ArrayList<Integer> x = new ArrayList<>();",
            "    if ((x.size() == 20 - 20) == true) {",
            "      System.out.println(\"empty\");",
            "    }",
            "  }",
            "}",
        },
        .cursor = {0, 0},
        .show_demo_window = false
    };
    std::string filename = "Example.java";
    window::init();
    bool show_demo_window = true;
    while (!window::is_exiting()) {
        struct stat attr;
        if ((stat(libpath, &attr) == 0) && (s.inode != attr.st_ino)) {
            s.inode = attr.st_ino;
            std::cout << "changed" << std::endl;
            if (handle) {
                dlclose(handle);
                handle = nullptr;
                render_function = nullptr;
            }
            handle = dlopen(libpath, RTLD_LAZY);
            if (handle == nullptr) {
                std::cerr << "could not connect to libprogram: " << dlerror() << std::endl;
                s.inode = {};
            } else {
                std::cout << "new handle: " << handle << std::endl;
            }
            if (handle) {
                render_function = dlsym(handle, "run");
                if (!render_function) {
                    std::cerr << "could not load render function: " << dlerror() << std::endl;
                    s.inode = {};
                } else {
                    s.dirty = true;
                }
            }
        }
        window::start_frame();
        s.keyboard_input = window::keyboard_input;
        s.text_input = window::text_input;
        ui::dockspace::render();
        ui::editor::render(&s);
        if (!s.ast.expired()) {
            ui::ast::render("Test", s.ast.lock(), 0);
        } else {
            ui::ast::render("Test", nullptr, 0);
        }
        if (s.show_demo_window) {
            ImGui::ShowDemoWindow();
        }
        if (render_function != nullptr && s.dirty) {
            std::cout << "calling function" << std::endl;
            reinterpret_cast<void(*)(state*)>(render_function)(&s);
            s.dirty = false;
        }
        window::text_input = {};
        window::end_frame();
    }
    window::destroy();
    return 0;
}

