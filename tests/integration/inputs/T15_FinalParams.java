package com.example.finals;

public class T15_FinalParams {

    private final int id;
    private final String name;

    public T15_FinalParams(final int id, final String name) {
        this.id = id;
        this.name = name;
    }

    public int add(final int a, final int b) { return a + b; }

    public boolean check(final String s, final boolean strict) { return false; }

    public static void apply(final int[] data, final int offset) {}
}
