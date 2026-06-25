package com.example.enumnested;

// Покрывает: parseEnumMembers -> isClassKeyword ветвь (вложенный класс в enum)
public enum T20_EnumWithNestedClass {
    ALPHA,
    BETA,
    GAMMA;

    private int value;

    public int getValue() {
        return value;
    }

    public static T20_EnumWithNestedClass fromInt(int v) {
        return ALPHA;
    }

    // вложенный класс внутри enum после ';'
    public static class Meta {

        private String label;
        private int code;

        public Meta(String label, int code) {
            this.label = label;
            this.code = code;
        }

        public String getLabel() {
            return label;
        }

        public int getCode() {
            return code;
        }
    }

    // вложенный интерфейс внутри enum
    interface Printable {
        void print();
    }
}
