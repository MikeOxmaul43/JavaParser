package com.example.inherit;

import java.io.Serializable;

public class T05_Inheritance extends T05_Base implements Runnable, Serializable {

    private int id;

    public T05_Inheritance(int id) {
        this.id = id;
    }

    public void run() {}

    public int getId() { return id; }
}

class T05_Base {
    protected String label;
    public T05_Base() {}
}
