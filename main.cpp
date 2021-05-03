#include "dockspace.h"
#include "editor.h"
#include "imgui-boilerplate/window.h"
#include "imgui.h"
#include "state.h"
#include <dlfcn.h>
#include <iostream>

static const std::string file = R"(import java.util.ArrayList;
import java.util.List;

public class Main {
  public static void s1125() {
    /**
     * Boolean literals should not be redundant
     * https://rules.sonarsource.com/java/RSPEC-1125
     */
    if (booleanMethod() == true) {}
    if (booleanMethod() == false) {}
    doSomething(booleanMethod() == true);
    doSomething(booleanMethod() == (1 == 1 && true));
  }
  public static void s1132() {
    /**
     * Strings literals should be placed on the left side when checking
     * for equality
     * https://rules.sonarsource.com/java/RSPEC-1132
     */
    String myStr = null;
    // Non-Compliant - will raise a NPE
    System.out.println("Equal? " + myStr.equals("foo"));
    // Non-Compliant - null check could be removed
    System.out.println("Equal? " + (myStr != null && myStr.equals("foo")));
    // Compliant - properly deals with the null case
    System.out.println("Equal?" + "foo".equals(myStr));
  }
  public static void s1155() {
    /**
     * Collection.isEmpty() should be used to test for emptiness
     * https://rules.sonarsource.com/java/RSPEC-1155
     */
    ArrayList<Integer> x = new ArrayList<>();
    if (x.size() == 0) {
      System.out.println("empty");
    }
    if (30 - 5 * 5 - 5 == x.size()) {
      System.out.println("empty");
    }
  }
  public static void s1596() {
    /**
     * "Collections.EMPTY_LIST", "EMPTY_MAP", and "EMPTY_SET" should not be used
     * https://rules.sonarsource.com/java/RSPEC-1596
     */
    List<String> collection1 = Collections.EMPTY_LIST;
    Map<String, String> collection2 = Collections.EMPTY_MAP;
    Set<String> collection3 = Collections.EMPTY_SET;
  }
  public static void s2111() {
    /**
     * BigDecimal(double) should not be used
     * https://rules.sonarsource.com/java/RSPEC-2111
     */
    double d = 1.1;
    float f = 1.1f;
    BigDecimal bd1 = new BigDecimal(d);
    BigDecimal bd2 = new BigDecimal(f);
    BigDecimal bd3 = new BigDecimal(1.1);
  }
  public static void s2293() {
    /**
     * The diamond operator ("<>") should be used
     * https://rules.sonarsource.com/java/RSPEC-2293
     */
    ArrayList<Integer> x = new ArrayList<Integer>();
    List<String> x = new ArrayList<String>();
  }
  public static void s4973() {
    /**
     * Strings and Boxed types should be compared using equals()
     * https://rules.sonarsource.com/java/RSPEC-4973
     */
    String firstName = getFirstName();
    String lastName = getLastName();
    if (firstName == lastName) {
        System.out.println("equal");
    }
    if (firstName == "test") {
        System.out.println("equal");
    }
    if ("test" == firstName) {
        System.out.println("equal");
    }
    if ("test" == "test") {
        System.out.println("equal");
    }
  }
  public static void other() {
    // TODO think about how to merge these into one rewrite
    ArrayList<Integer> x = new ArrayList<>();
    if ((x.size() == 20 - 20) == true) {
      System.out.println("empty");
    }
  }
}
)";

const char* libpath = "./libprogram.so";

int main() {
    void* handle = nullptr;
    void* render_function = nullptr;
    state s = {.cursor = {0, 0}, .show_demo_window = false};
    std::istringstream f(file);
    std::string line;
    while (std::getline(f, line)) {
        s.lines.emplace_back(line);
    }
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
                std::cerr << "could not connect to libprogram: " << dlerror()
                          << std::endl;
                s.inode = {};
            } else {
                std::cout << "new handle: " << handle << std::endl;
            }
            if (handle) {
                render_function = dlsym(handle, "run");
                if (!render_function) {
                    std::cerr << "could not load render function: " << dlerror()
                              << std::endl;
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

        size_t buffer_position = 0;
        for (int i = 0; i < s.cursor.second; i++) {
            buffer_position += s.lines[i].size() + 1; // 1 extra for newline
        }
        buffer_position +=
            std::min(s.cursor.first, int(s.lines[s.cursor.second].size()));

        if (!s.ast.expired()) {
            ui::ast::render("Test", s.ast.lock(), buffer_position);
        } else {
            ui::ast::render("Test", nullptr, buffer_position);
        }
        if (s.show_demo_window) {
            ImGui::ShowDemoWindow();
        }
        if (render_function != nullptr && s.dirty) {
            std::cout << "calling function" << std::endl;
            reinterpret_cast<void (*)(state*)>(render_function)(&s);
            s.dirty = false;
        }
        window::text_input = {};
        window::end_frame();
    }
    window::destroy();
    return 0;
}
