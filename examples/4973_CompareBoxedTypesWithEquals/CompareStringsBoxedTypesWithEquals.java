// https://github.com/SpoonLabs/sorald/blob/master/src/test/resources/processor_test_files/4973_CompareStringsBoxedTypesWithEquals/CompareStringsBoxedTypesWithEquals.java

public class CompareStringsBoxedTypesWithEquals {

    // Test from https://rules.sonarsource.com/java/type/Bug/RSPEC-4973
    public void main(String[] args) {
        // String overrides equals
        String firstName = getFirstName();
        String lastName = getLastName();
        // Noncompliant, false even if the strings have the same value
        if (firstName == lastName) {
            System.out.println("equal");
        }; 
    }

    // Additional tests
    boolean eq = true;

    // Java implicitly converts one variable to primitive if something boxed
    // and primitive is compared.
    private void mixedCompare() {
        int e = 4;
        Integer f = 4;
        eq = (e != f); // Compliant
        eq = (f == e); // Compliant
    }

    // Integer is not primitive and should use .equals()
    private boolean IntegerCompare() {
        Integer a = 5;
        Integer b = 5;
        // Noncompliant
        return b != a;
    }

    // Int is primitive and can use ==
    private void intCompare() {
        int x = 5;
        int y = 5;
        eq = (x == y); // Compliant
        eq = (y == x); // Compliant
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
        // String overrides equals
        String firstName = getFirstName(); 
        String lastName = getLastName();
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
