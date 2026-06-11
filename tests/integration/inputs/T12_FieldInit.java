package com.example.init;

public class T12_FieldInit {

    public static final String OPEN = "{";
    public static final String CLOSE = "}";
    public static final char BRACE = '{';
    public static final char QUOTE = '\'';
    public static final String PATH = "C:\\tmp\\file.txt";
    public static final int MASK = 0xFF;
    private int x = 10;
    private double ratio = 3.14;

    public T12_FieldInit() {}

    public void test() {
        String s = "{ not a block }";
        char c = '}';
    }
}
