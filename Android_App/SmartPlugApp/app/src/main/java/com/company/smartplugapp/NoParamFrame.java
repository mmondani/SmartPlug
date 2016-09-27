package com.company.smartplugapp;

/**
 * Created by mariano on 27/09/16.
 */
public class NoParamFrame extends BasicFrame {

    public NoParamFrame (byte length, byte command) {
        super(Types.NO_PARAM, length, command);
    }
}
