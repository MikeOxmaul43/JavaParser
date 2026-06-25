package com.example.abstract2;

// Покрывает: parseClass -> implements с несколькими интерфейсами (уже есть в T05)
// + abstract класс с implements (ci.modifiers.contains("abstract") в buildNode)
// + interface с default-методом (тело есть -> skipBody, а не readWord(";"))
// + несколько уровней вложенности nested классов
public abstract class T25_AbstractInterface implements Runnable, Cloneable {

    protected abstract int compute(int x);

    public abstract void process(String data, int flags);

    public abstract boolean validate();

    // вложенный abstract класс
    public abstract static class Base {
        protected abstract void init();
        public abstract String name();
    }

    // вложенный конкретный класс наследующий вложенный abstract
    public static class Impl extends Base {
        private int id;

        public Impl(int id) { this.id = id; }

        protected void init() {}

        public String name() { return "Impl"; }

        public int getId() { return id; }
    }

    // вложенный интерфейс с несколькими методами
    interface Processor {
        void start();
        void stop();
        boolean isRunning();
    }
}