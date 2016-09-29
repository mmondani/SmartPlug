package com.company.smartplugapp;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by mariano on 28/09/16.
 */
public class StringFrame extends BasicFrame {
    private byte mRegister;
    private String mData;

    public StringFrame (byte length, byte command, byte register, byte[] payload) {
        super(Types.STRING_PARAM, length, command);

        mRegister = register;

        /**
         * El payload se compone de un String de 33 caracteres.
         */
        if (payload.length == 33)
            mData = new String(payload).trim();
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }

    public String getData() {
        return mData;
    }

    public void setData(String data) {
        mData = data;
    }
}
