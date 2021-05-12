class RedundantBooleanInEqualsExpression {
    boolean z = true;
    boolean first(String x) {
        // Noncompliant
        return x.isEmpty() == true;
    }
    boolean second(String x) {
        // Noncompliant
        return x.isEmpty() == false;
    }
    void withIf() {
        // Noncompliant
        if (z == false) {
            System.out.println("print");
        }
        // Noncompliant
        if (z == true) {
            System.out.println("print");
        }
    }
}
