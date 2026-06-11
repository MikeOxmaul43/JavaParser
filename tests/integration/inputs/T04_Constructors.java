package com.example.ctors;

public class T04_Constructors {

    private String name;
    private int age;

    public T04_Constructors() {}

    public T04_Constructors(String name) {
        this.name = name;
    }

    public T04_Constructors(String name, int age) {
        this.name = name;
        this.age = age;
    }

    protected T04_Constructors(int age) {
        this.age = age;
    }

    private T04_Constructors(boolean dummy) {}
}
