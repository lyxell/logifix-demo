import java.util.List;
import java.util.Stack;
import java.util.ArrayList;

public class CompareSizeToZero {
    private List<String> privateList;
    public boolean compareList(List<Integer> list) {
        // Noncompliant
        return list.size() == 0;
    }
    public boolean compareArrayList(ArrayList<Integer> list) {
        // Noncompliant
        return list.size() == 0;
    }
    public boolean comparePrivateList() {
        // Noncompliant
        return privateList.size() == 0;
    }
    public boolean compareStack(Stack<Integer> stack) {
        // Noncompliant
        return stack.size() == 0;
    }
}
