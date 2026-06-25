package com.example.escaped;

// Покрывает: processBodyChar -> skipQuotedLiteral -> ветвь c == '\\'
// Экранированные символы В ТЕЛЕ МЕТОДА (не в инициализаторах полей)
public class T21_EscapedLiterals {

    private String path;
    private char sep;

    public T21_EscapedLiterals() {}

    public String buildPath() {
        String a = "C:\\Users\\test\\file.txt";
        String b = "line1\nline2\ttabbed";
        String c = "she said \"hello\"";
        char q = '\'';
        char bs = '\\';
        char nl = '\n';
        return a + b + c + q + bs + nl;
    }

    public void mixedBraces() {
        String open = "{";
        String close = "}";
        char ob = '{';
        char cb = '}';
        String nested = "{ inner { deep } }";
    }
}
