package com.example.nested;

public class T08_NestedClasses {

    private int value;

    public T08_NestedClasses(int value) {
        this.value = value;
    }

    public static class Builder {
        private int val;

        public Builder setValue(int v) { return this; }

        public T08_NestedClasses build() { return new T08_NestedClasses(val); }
    }

    class Inner {
        private String label;

        public String getLabel() { return label; }
    }

    interface Validator {
        boolean validate(int v);
    }

    enum Status {
        OK, FAIL
    }
}
