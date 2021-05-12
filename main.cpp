#include "dockspace.h"
#include "editor.h"
#include "imgui-boilerplate/window.h"
#include "imgui.h"
#include "state.h"
#include <dlfcn.h>
#include <iostream>
#include <thread>

static const std::string file = R"(import java.util.ArrayList;
public class Test {
    public void test() {
        String x = "hello", y = "world";
        if (x == "") {
            System.out.println(y);
        }
    }
}
)";

static const std::string file2 = R"(import java.util.ArrayList;
import java.util.NoSuchElementException;
import java.util.List;
import java.math.BigDecimal;
import java.math.MathContext;

public class Test {
    public void test() {
        String x = "hello", y = "world";
        System.out.println(y);
    }
}

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
    MathContext mc;
    double d = 1.1;
    float f = 1.1f;
    BigDecimal bd1 = new BigDecimal(d);
    BigDecimal bd2 = new BigDecimal(f);
    BigDecimal bd3 = new BigDecimal(1.1);
    BigDecimal bd4 = new BigDecimal(1.1f);
    BigDecimal bd5 = new BigDecimal(d, mc);
    BigDecimal bd6 = new BigDecimal(f, mc);
    BigDecimal bd7 = new BigDecimal(1.1, mc);
    BigDecimal bd8 = new BigDecimal(1.1f, mc);
  }
  public static void s2204() {
    /**
     * .equals() should not be used to test the values of "Atomic" classes
     * https://rules.sonarsource.com/java/RSPEC-2204
     */
    AtomicInteger aInt1 = new AtomicInteger(0);
    AtomicInteger aInt2 = new AtomicInteger(0);
    if (aInt1.equals(aInt2)) {
        System.out.println("equals");
    }
    AtomicLong aLong1 = new AtomicLong(0);
    AtomicLong aLong2 = new AtomicLong(0);
    if (aLong1.equals(aLong2)) {
        System.out.println("equals");
    }
    AtomicBoolean aBoolean1 = new AtomicBoolean(0);
    AtomicBoolean aBoolean2 = new AtomicBoolean(0);
    if (aBoolean1.equals(aBoolean2)) {
        System.out.println("equals");
    }
  }
  public static void s2293() {
    /**
     * The diamond operator ("<>") should be used
     * https://rules.sonarsource.com/java/RSPEC-2293
     */
    ArrayList<Integer> x = new ArrayList<Integer>();
    List<String> y = new ArrayList<String>();
  }
  public static void s3984() {
    /**
     * Exceptions should not be created without being thrown
     * https://rules.sonarsource.com/java/RSPEC-3984
     */
    new IllegalArgumentException("x must be nonnegative");
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
}

// Test for rule s4973 (from Sorald)
public class CompareStringsBoxedTypesWithEquals {

    // Test from https://rules.sonarsource.com/java/type/Bug/RSPEC-4973
    public void main(String[] args) {
        String firstName = getFirstName();
        String lastName = getLastName();

        // Noncompliant; false even if the strings have the same value
        if (firstName == lastName) {};
    }

    // Aditional tests
    boolean eq = true;

    // Java implicitly converts one variable to primitive if something boxed and primitive is compared.
    private void mixedCompare() {
        int e = 4;
        Integer f = 4;
        eq = (e != f); // Compliant;
        eq = (f == e); // Compliant;
    }

    // Integer is not primitive and should use .equals()
    private boolean IntegerCompare() {
        Integer a = 5;
        Integer b = 5;
        return b != a; // Noncompliant
    }

    // Int is primitive and can use ==
    private void intCompare() {
        int x = 5;
        int y = 5;
        eq = (x == y); // Compliant;
        eq = (y == x); // Compliant;
    }

    // Null comparisons are excluded from transformation
    private void nullCompare() {
        String x = null;
        eq = (x == null); // Compliant
        eq = (null == x); // Compliant
    }

    enum foo {
        BAR,
        XOR
    }

    // ENUM comparisons are excluded from transformation
    private void nullCompare2() {
        foo x = foo.BAR;
        eq = (x == foo.BAR); // Compliant
        eq = (foo.XOR == x); // Compliant
    }

    // String is not primitive and should use .equals()
    private boolean stringCompare() {
        String firstName = getFirstName();
        String lastName = getLastName();
        // Noncompliant
        if (firstName == lastName) {
            return true;
        }
        return false;
    }

    // Object comparison should not be converted
    private void objectCompare() {
        Object a = 1;
        Object b = 1;
        eq = a == b; // Compliant
        Integer x = 2;
        eq = a == x; // Compliant
    }

    private String getFirstName() {
        return new String("John");
    }

    private String getLastName() {
        return new String("John");
    }

}

// Test for rule s2111 (from Sorald)
public class BigDecimalDoubleConstructor {

    // Tests from https://rules.sonarsource.com/java/type/Bug/RSPEC-2111
    public void main(String[] args) {
        double d = 1.1;
        BigDecimal bd1 = new BigDecimal(d); // Noncompliant; see comment above
        BigDecimal bd2 = new BigDecimal(1.1); // Noncompliant; same result
    }

    // Tests from https://github.com/SonarSource/sonar-java/blob/master/java-checks-test-sources/src/main/java/checks/BigDecimalDoubleConstructorCheck.java
    public void main2(String[] args) {
        MathContext mc = null;
        BigDecimal bd1 = new BigDecimal("1");
        // Noncompliant {{Use "BigDecimal.valueOf" instead.}}
        BigDecimal bd2 = new BigDecimal(2.0);
        // Noncompliant {{Use "BigDecimal.valueOf" instead.}}
        BigDecimal bd4 = new BigDecimal(2.0, mc); 
        // Noncompliant {{Use "BigDecimal.valueOf" instead.}}
        BigDecimal bd5 = new BigDecimal(2.0f);
        // Noncompliant {{Use "BigDecimal.valueOf" instead.}}
        BigDecimal bd6 = new BigDecimal(2.0f, mc);
        BigDecimal bd3 = BigDecimal.valueOf(2.0);
    }

    // Additional tests
    public void foo(String[] args) {
        double d = 1.1;
        float f = 2.2f;
        float f1 = 2f;
        // Noncompliant
        BigDecimal bd3 = new BigDecimal(f);
        // Noncompliant
        BigDecimal bd4 = new BigDecimal(f1);
        // Compliant
        BigDecimal bd5 = BigDecimal.valueOf(d);
        // Compliant; using String constructor will result in precise value
        BigDecimal bd6 = new BigDecimal("1.1");
        // Compliant
        BigDecimal bd7 = BigDecimal.valueOf(f);
        // Compliant
        BigDecimal bd8 = BigDecimal.valueOf(f1);
    }

}

public class Test {
    // default accessor to enable unit tests without requiring reflection
    static Map<String, String> convertCommandLineArgs(
            Map<String, String> rawArgs) {
        final HashMap<String, String> props = new HashMap<String, String>();
        boolean errorArg = false;
        for (Entry<String, String> arg : rawArgs.entrySet()) {
            if (arg.getKey().length() == 1 || arg.getKey().startsWith("D")) {
                String value = arg.getValue();
                switch (arg.getKey().charAt(0)) {
                    case 'j':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-j", "Missing host:port value");
                            errorArg = true;
                            continue;
                        }
                        props.put(PROP_CONTROL_SOCKET, value);
                        break;
                    case 'l':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-l", "Missing log level value");
                            errorArg = true;
                            continue;
                        }
                        props.put(PROP_LOG_LEVEL, value);
                        break;
                    case 'f':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-f", "Missing log file value");
                            errorArg = true;
                            continue;
                        } else if ("-".equals(value)) {
                            value = "";
                        }
                        props.put(PROP_LOG_FILE, value);
                        break;
                    case 'c':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-c", "Missing directory value");
                            errorArg = true;
                            continue;
                        }
                        props.put(SharedConstants.SLING_HOME, value);
                        break;
                    case 'i':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-i", "Missing launchpad directory value");
                            errorArg = true;
                            continue;
                        }
                        props.put(SharedConstants.SLING_LAUNCHPAD, value);
                        break;
                    case 'a':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-a", "Missing address value");
                            errorArg = true;
                            continue;
                        }
                        props.put(PROP_HOST, value);
                        break;
                    case 'p':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-p", "Missing port value");
                            errorArg = true;
                            continue;
                        }
                        try {
                            // just to verify it is a number
                            Integer.parseInt(value);
                            props.put(PROP_PORT, value);
                        } catch (RuntimeException e) {
                            errorArg("-p", "Bad port: " + value);
                            errorArg = true;
                        }
                        break;
                    case 'r':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-r", "Missing root path value");
                            errorArg = true;
                            continue;
                        }
                        props.put(PROP_CONTEXT_PATH, value);
                        break;
                    case 'n':
                        props.put(PROP_SHUTDOWN_HOOK, Boolean.FALSE.toString());
                        break;
                    case 'D':
                        // Noncompliant S4973
                        if (value == arg.getKey()) {
                            errorArg("-D", "Missing property assignment");
                            errorArg = true;
                            continue;
                        }
                        if (arg.getKey().length() > 1) {
                            //Dfoo=bar arg.key=Dfoo and arg.value=bar
                            props.put(arg.getKey().substring(1), arg.getValue());
                        } else {
                            //D foo=bar arg.key=D and arg.value=foo=bar
                            String[] parts = value.split("=");
                            int valueIdx = (parts.length > 1) ? 1 : 0;
                            props.put(parts[0], parts[valueIdx]);
                        }
                        break;
                    default:
                        errorArg("-" + arg.getKey(), "Unrecognized option");
                        errorArg = true;
                        break;
                }
            } else if ("start".equals(arg.getKey())
                    || "stop".equals(arg.getKey())
                    || "status".equals(arg.getKey())
                    || "threads".equals(arg.getKey())) {
                props.put(PROP_CONTROL_ACTION, arg.getValue());
            } else {
                errorArg(arg.getKey(), "Unrecognized option");
                errorArg = true;
            }
        }
        return errorArg ? null : props;
    }
    @Override
    protected ElementFrame<Node, Node> getChildFrame() {
        // Noncompliant 2293
        return new ElementFrame<Node, Node>(getCurrentFrame().currentChild,
                                getCurrentFrame());
    }
}

public class MyIterator implements Iterator<String> {
  public String next() {
    if (!hasNext()) {
      return null;
    }
  }
}

public class MyIterator implements Iterator<String> {
  public String next() {
    if (!hasNext()) {
      throw new NoSuchElementException();
    }
  }
}

)";

const char* libpath = "./libprogram.so";

int main() {
    void* handle = nullptr;
    void* render_function = nullptr;
    state s = {.cursor = {0, 0}, .show_demo_window = true};
    std::istringstream f(file);
    std::string line;
    while (std::getline(f, line)) {
        s.lines.emplace_back(line);
    }
    std::string filename = "Example.java";
    window::init();
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

        {
            const std::lock_guard<std::mutex> lock(s.mutex);
            ui::editor::render(&s);
        }

        size_t buffer_position = 0;
        for (int i = 0; i < s.cursor.second; i++) {
            buffer_position += s.lines[i].size() + 1; // 1 extra for newline
        }
        buffer_position +=
            std::min(s.cursor.first, int(s.lines[s.cursor.second].size()));

        if (!s.ast.expired()) {
            const std::lock_guard<std::mutex> lock(s.mutex);
            ui::ast::render("Test", s.ast.lock(), buffer_position);
        } else {
            ui::ast::render("Test", nullptr, buffer_position);
        }

        {
            const std::lock_guard<std::mutex> lock(s.mutex);
            ImGui::Begin("Variables in scope");
            std::set<std::pair<std::string,std::string>> vars;
            for (auto [v, type, a, b] : s.variables_in_scope) {
                if (a <= buffer_position && b >= buffer_position) {
                    vars.emplace(v, type);
                }
            }
            if (ImGui::BeginTable("Declared variables", 2)) {
                ImGui::TableSetupColumn("Variable name");
                ImGui::TableSetupColumn("Type");
                ImGui::TableHeadersRow();
                for (auto [v, type] : vars) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", v.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", type.c_str());
                }
                ImGui::EndTable();
            }
            ImGui::End();
        }

        if (s.show_demo_window) {
            ImGui::ShowDemoWindow();
        }
        if (render_function != nullptr && s.dirty) {
            s.dirty = false;
            auto fn = reinterpret_cast<void (*)(state*)>(render_function);
            std::thread t(fn, &s);
            t.detach();
        }
        window::text_input = {};
        window::end_frame();
    }
    window::destroy();
    return 0;
}
