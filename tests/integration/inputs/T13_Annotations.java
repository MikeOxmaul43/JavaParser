package com.example.annotations;

import java.lang.Override;

@SuppressWarnings("all")
public class T13_Annotations {

    @Deprecated
    private int oldField;

    @SuppressWarnings("unchecked")
    public T13_Annotations() {}

    @Override
    public String toString() { return ""; }

    public void save(@NotNull String name, @Min(0) int count) {}

    @Deprecated
    public static void legacyMethod() {}
}
