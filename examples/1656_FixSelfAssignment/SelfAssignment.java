// https://github.com/SpoonLabs/sorald/blob/master/src/test/resources/processor_test_files/1656_SelfAssignment/SelfAssignement.java

class SelfAssignment {

  int a,c = 0;
  int[] b = {0};
  int h = 0;
  int[] g = {0};
  SelfAssignmentCheckB checkB = new SelfAssignmentCheckB();

  void method(int e,int h) {
    a = a; // Noncompliant
    this.a = this.a; // Noncompliant
    b[0] = b[0]; // Noncompliant
    b[fun()] = b[fun()]; // Noncompliant
    int d = 0;
    d = d; // Noncompliant
    e = e; // Noncompliant
    int[] g = {0};
    g[fun()] = g[fun()]; // Noncompliant
    h = h; // Noncompliant
    checkB.b = checkB.b; // Noncompliant
    checkB.getSelf().foo = checkB.getSelf().foo; // Noncompliant
  }


  int fun() {
    return 0;
  }

  class SelfAssignmentCheckB {
    int b;
    int foo;

    void setFoo(int foo){
      this.foo = foo;
    }

    int getFoo() {
      return this.foo;
    }

    SelfAssignmentCheckB getSelf() {
      return this;
    }

    SelfAssignmentCheckB(SelfAssignmentCheckB bInstance) {
      foo = bInstance.foo;
    }

    SelfAssignmentCheckB() {}

  }
}
