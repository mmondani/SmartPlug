package com.company.smartplugapp;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by mariano on 28/09/16.
 */
public class ByteFrame extends BasicFrame {
    private byte mRegister;
    private byte mData;

    public ByteFrame (byte length, byte command, byte register, byte[] payload) {
        super(Types.BYTE_PARAM, length, command);

        mRegister = register;

        /**
         * El payload se compone de 1 byte.
         */
        if (payload.length == 1)
            mData = payload[0];
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }

    public byte getData() {
        return mData;
    }

    public void setData(byte data) {
        mData = data;
    }
}
