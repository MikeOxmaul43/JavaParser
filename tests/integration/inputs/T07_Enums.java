package com.example.enums;

public enum T07_SimpleEnum {
    RED, GREEN, BLUE
}

enum T07_RichEnum {
    PENDING, RUNNING, DONE, FAILED;

    private int code;
    public static int total;

    public int getCode() { return code; }

    public static boolean isTerminal(T07_RichEnum s) { return false; }
}
