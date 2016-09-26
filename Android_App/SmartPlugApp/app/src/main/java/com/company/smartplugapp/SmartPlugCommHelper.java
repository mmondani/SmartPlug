package com.company.smartplugapp;

import java.nio.ByteBuffer;
import java.util.Arrays;

/**
 * Esta clase provee métodos que permiten parsear la data que
 * llega o se debe enviar a los Smart Plugs de acuerdo al formato
 * de trama establecido.
 */
public class SmartPlugCommHelper {

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
