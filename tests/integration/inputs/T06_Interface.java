package com.example.iface;

public interface T06_Interface {

    int DEFAULT_SIZE = 10;

    void open();

    void close();

    int read(byte[] buf);

    boolean isOpen();

    String getName();
}
