package com.company.smartplugapp;

/**
 * Created by mariano on 27/09/16.
 */
public class BasicFrame {
    private int mFrameType;
    private byte mLength;
    private byte mCommand;

    public static final class Types {
        public static final int NO_PARAM = 1;
        public static final int FLOAT_PARAM = 2;
        public static final int STRING_PARAM = 3;
        public static final int BYTE_PARAM = 4;
        public static final int BYTE_ARRAY_PARAM = 5;
        public static final int FLOAT_ARRAY_PARAM = 6;
    }

    public BasicFrame () {

    }

    public BasicFrame (int frameType, byte length, byte command) {
        mFrameType = frameType;
        mLength = length;
        mCommand = command;
    }

    public int getFrameType() {
        return mFrameType;
    }

    public void setFrameType(int frameType) {
        mFrameType = frameType;
    }

    public byte getLength() {
        return mLength;
    }

    public void setLength(byte length) {
        mLength = length;
    }

    public byte getCommand() {
        return mCommand;
    }

    public void setCommand(byte command) {
        mCommand = command;
    }
}
