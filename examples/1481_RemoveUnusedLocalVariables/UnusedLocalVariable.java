class UnusedLocalVariable {
    public int numberOfMinutes(int hours) {
        // Noncompliant
        int seconds;
        return hours * 60;
    }
}
