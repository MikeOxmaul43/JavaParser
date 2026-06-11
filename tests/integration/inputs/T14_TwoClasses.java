package com.example.multi;

public class T14_ClassA {

    private String name;

    public T14_ClassA(String name) {
        this.name = name;
    }

    public String getName() { return name; }
}

class T14_ClassB {

    private int value;

    public T14_ClassB(int value) {
        this.value = value;
    }

    public int getValue() { return value; }

    public void setValue(int v) { this.value = v; }
}
