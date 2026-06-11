package com.example.methods;

public abstract class T03_Methods {

    public void reset() {}

    public int getCount() { return 0; }

    protected static String format(String s) { return s; }

    public int[] getValues() { return new int[0]; }

    public abstract void process();

    public abstract int compute(int a, int b);

    // перегрузка
    public void log(String msg) {}

    public void log(String msg, int level) {}

    public void log(String msg, int level, boolean force) {}

    private final void lock() {}
}
