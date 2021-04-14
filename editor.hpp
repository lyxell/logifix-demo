#pragma once

#include <cstring>

class editor {
public:
    char text[1024 * 16];
    void render();
    editor() {
        std::strcpy(text,
            "public class Main {\n"
            "   public static void main(String[] args) {\n"
            "       System.out.println();\n"
            "   }\n"
            "}");
    }
};
