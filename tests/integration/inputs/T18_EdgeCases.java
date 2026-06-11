package com.example.edge;

// Пустой enum
enum T18_EmptyEnum {
}

// Интерфейс без членов
interface T18_EmptyInterface {
}

// Класс только с конструктором, без полей и методов
class T18_CtorOnly {
    public T18_CtorOnly() {}
}

// Объявление класса с переносами строк везде
public
class
T18_Multiline
extends T18_CtorOnly
implements T18_EmptyInterface
{
    private
    int
    value;

    public
    T18_Multiline(
        int value
    ) {
        this.value = value;
    }

    public
    int
    getValue() {
        return value;
    }
}
