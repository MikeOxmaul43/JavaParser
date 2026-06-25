package com.example.annmods;

// Покрывает: parseClassBody -> второй while(peekChar() == '@') — аннотация ПОСЛЕ модификаторов
// Также: skipAnnotation с параметрами-строками, числами, множественными параметрами
public class T22_AnnotationAfterMods {

    private int x;

    public static @interface MyAnnotation {}

    public @Deprecated int legacyField = 0;

    public @SuppressWarnings("unused") void doNothing() {}

    public static @Deprecated void oldStatic() {}

    protected final @SuppressWarnings(value = "all") String compute(
        int a,
        int b
    ) {
        return "";
    }
}
