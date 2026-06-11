package com.example.staticinit;

public class T16_StaticInit {

    public static final int BASE;
    private static String prefix;

    static {
        BASE = 42;
        prefix = "X";
    }

    private int value;

    {
        value = BASE;
    }

    public T16_StaticInit() {}

    public T16_StaticInit(int value) {
        this.value = value;
    }

    public int getValue() { return value; }

    public static int getBase() { return BASE; }
}
