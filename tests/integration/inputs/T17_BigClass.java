package com.example.integration;

import java.io.Serializable;
import java.util.List;

/**
 * Большой интеграционный класс.
 */
public abstract class T17_BigClass extends Object implements Serializable {

    public static final int VERSION = 2;
    private String name;
    protected boolean active;
    private int[] scores;

    public T17_BigClass(String name) {
        this.name = name;
    }

    public T17_BigClass(String name, boolean active) {
        this.name = name;
        this.active = active;
    }

    public abstract void process(int id, String data);

    protected static String format(String s, int width) { return s; }

    private boolean validate(String s, int min, int max) { return true; }

    public int[] getScores() { return scores; }

    public void setScores(int[] scores) { this.scores = scores; }

    public void log(String msg) {}

    public void log(String msg, int level) {}

    // вложенный static класс
    public static class Config {
        private String host;
        private int port;

        public Config(String host, int port) {
            this.host = host;
            this.port = port;
        }

        public String getHost() { return host; }

        public int getPort() { return port; }
    }

    // вложенный класс Builder
    public static class Builder {
        private String bname;
        private boolean bactive;

        public Builder setName(String n) { bname = n; return this; }

        public Builder setActive(boolean a) { bactive = a; return this; }
    }

    // вложенный интерфейс
    interface Handler {
        void handle(String event);
        boolean canHandle(String event);
    }

    // вложенный enum
    enum State {
        INIT, RUNNING, PAUSED, DONE
    }
}
