package com.company.smartplugapp;

import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Esta clase provee métodos que permiten parsear la data que
 * llega o se debe enviar a los Smart Plugs de acuerdo al formato
 * de trama establecido.
 */
public class SmartPlugCommHelper {

    public static final class Commands {
        public static final byte GET = 0x01;
        public static final byte SET = 0x02;
        public static final byte NODE_ON = 0x10;
        public static final byte NODE_OFF = 0x11;
        public static final byte RESET = 0x20;
        public static final byte RESP_GET = 0x30;
        public static final byte RESP_SET = 0x31;
        public static final byte RESP_RESET = 0x32;
        public static final byte RESP_NODE_ON = 0x33;
        public static final byte RESP_NODE_OFF = 0x34;
    }

    /**
     * Retorna una instancia de SmartPlugCommHelper. Se debe llamar a este método
     * para utilizar las funciones de esta clase.
     * @return instancia de SmartPlugCommHelper.
     */
    public static SmartPlugCommHelper getInstance () {
        SmartPlugCommHelper helper = new SmartPlugCommHelper();

        return helper;
    }

    /**
     * Constructor privado de la calse SmartPlugCommHelper.
     */
    private SmartPlugCommHelper () {

    }

    /**
     * Obtiene los campos de una trama de heartbeat a partir del array de bytes.
     * @param data Array de bytes de la trama de heartbeat
     * @return instancia de HeartbeatFrame con los campos extraídos de la trama. Retorna
     *          null si la cantidad de bytes de data es menor a 100 (la trama de heartbeat
     *          siempre tiene 110 bytes).
     */
    public HeartbeatFrame parseHeartBeat (byte[] data) {

        if (data.length > 100) {
            HeartbeatFrame frame = new HeartbeatFrame();

            frame.setMacAddress(Arrays.copyOfRange(data, 0, 6));
            frame.setRssi(data[7]);
            frame.setPort(ByteBuffer.wrap(Arrays.copyOfRange(data, 8, 8 + 2)).getShort());
            frame.setId(new String(Arrays.copyOfRange(data, 60, 60 + 32)).substring(0, 6));

            return frame;
        }
        else
            return null;
    }

    public byte[] getRawData (byte command) {
        byte[] data = new byte[6];

        data[0] = '#';
        data[1] = '!';
        data[2] = 3;        // Longitud del paquete sin contar data[0] y data[1]
        data[3] = command;
        data[4] = '#';
        data[5] = '!';

        return data;
    }


    public BasicFrame parseFrame (byte[] data) {
        BasicFrame frame = null;
        int length;
        int command;
        int register;

        if (data.length > 3) {
            if (data[0] == '#' && data[1] == '!') {
                length = data[2];

                /**
                 * A la longitud indicada en la trama se le agregan 3 bytes iniciales: #, ! y el byte LEN
                 */
                if (data.length >= (length+3)) {
                    command = data[3];

                    if ( (command != Commands.RESP_NODE_ON) && (command != Commands.RESP_NODE_OFF) ) {
                        /**
                         * Si no es un comando RESP_NODE_ON o RESP_NODE_OFF, hay un registro que acompaña al comando
                         */
                        register = data[4];

                        /** TODO Parsear el resto de las tramas a partir del comando y el registro */
                    }
                    else {
                        /**
                         * Si es un comando RESP_NODE_ON o RESP_NODE_OFF ya se puede devolver una instancia de NoParamFame
                         */
                        frame = new NoParamFrame((byte)length, (byte)command);
                    }
                }
            }
        }

        return frame;
    }


    /**
     * Representa una trama de heartbeat. Contiene los elementos de interés que vienen
     * incluidos en la trama.
     */
    public class HeartbeatFrame {
        private byte[] mMacAddress;
        private short mRssi;
        private int mPort;
        private String mId;

        public HeartbeatFrame () {

        }

        public HeartbeatFrame (byte[] macAddress, short rssi, int port, String id) {
            mMacAddress = macAddress;
            mRssi = rssi;
            mPort = port;
            mId = id;
        }

        public byte[] getMacAddress() {
            return mMacAddress;
        }

        public void setMacAddress(byte[] macAddress) {
            mMacAddress = macAddress;
        }

        public short getRssi() {
            return mRssi;
        }

        public void setRssi(short rssi) {
            mRssi = rssi;
        }

        public int getPort() {
            return mPort;
        }

        public void setPort(int port) {
            mPort = port;
        }

        public String getId() {
            return mId;
        }

        public void setId(String id) {
            mId = id;
        }
    }
}
