package smartPlugComm;

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

    public static final class Registers {
        public static final byte V_RMS = 0x01;
        public static final byte I_RMS = 0x02;
        public static final byte POWER_FACTOR = 0x03;
        public static final byte FREQUENCY = 0x04;
        public static final byte ACTIVE_POWER = 0x05;
        public static final byte TOTAL_ENERGY = 0x06;
        public static final byte CURRENT_HOUR_ENERGY = 0x07;
        public static final byte CURRENT_MEASUREMENTS = 0x08;
        public static final byte DEVICE_ID = 0x10;
        public static final byte LOAD_STATE = 0x15;
        public static final byte SYNCH_TIME = 0x1A;
        public static final byte DATE_TIME = 0x1B;
        public static final byte MONDAY_LOAD_ON_TIME = 0x20;
        public static final byte MONDAY_LOAD_OFF_TIME = 0x21;
        public static final byte TUESDAY_LOAD_ON_TIME = 0x22;
        public static final byte TUESDAY_LOAD_OFF_TIME = 0x23;
        public static final byte WEDNESDAY_LOAD_ON_TIME = 0x24;
        public static final byte WEDNESDAY_LOAD_OFF_TIME = 0x25;
        public static final byte THURSDAY_LOAD_ON_TIME = 0x26;
        public static final byte THURSDAY_LOAD_OFF_TIME = 0x27;
        public static final byte FRIDAY_LOAD_ON_TIME = 0x28;
        public static final byte FRIDAY_LOAD_OFF_TIME = 0x29;
        public static final byte SATURDAY_LOAD_ON_TIME = 0x2A;
        public static final byte SATURDAY_LOAD_OFF_TIME = 0x2B;
        public static final byte SUNDAY_LOAD_ON_TIME = 0x2C;
        public static final byte SUNDAY_LOAD_OFF_TIME = 0x2D;
        public static final byte ENABLE_ONOFF_TIME = 0x2E;
        public static final byte ONOFF_TIMES = 0x2F;
        public static final byte PER_HOUR_ENERGY = 0x30;
        public static final byte PER_HOUR_ACTIVE_POWER = 0x31;
        public static final byte ALL_REGISTERS = 0x70;
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

    public byte[] getRawData (byte command, byte register) {
        byte[] data = new byte[7];

        data[0] = '#';
        data[1] = '!';
        data[2] = 4;        // Longitud del paquete sin contar data[0] y data[1]
        data[3] = command;
        data[4] = register;
        data[5] = '#';
        data[6] = '!';

        return data;
    }

    public byte[] getRawData (byte command, byte register, byte[] payload) {
        byte[] data = new byte[payload.length + 7];

        data[0] = '#';
        data[1] = '!';
        data[2] = (byte) (payload.length + 4);          /** Longitud del paquete sin contar data[0] y data[1] */
        data[3] = command;
        data[4] = register;

        int  i;
        for (i = 0; i < payload.length; i++)            /** Se carga el payload. */
            data[i+5] = payload[i];

        data[i+5] = '#';
        data[i+6] = '!';

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

                    if ( (command != Commands.RESP_NODE_ON) && (command != Commands.RESP_NODE_OFF)  && (command != Commands.RESP_RESET)) {
                        /**
                         * Si no es un comando RESP_NODE_ON, RESP_NODE_OFF o RESP_RESET, hay un registro que acompaña al comando
                         */
                        register = data[4];

                        if (command == Commands.RESP_GET) {
                            if ((register == Registers.V_RMS) || (register == Registers.I_RMS) || (register == Registers.POWER_FACTOR) ||
                                    (register == Registers.FREQUENCY) || (register == Registers.ACTIVE_POWER) || (register == Registers.TOTAL_ENERGY) ||
                                    (register == Registers.CURRENT_HOUR_ENERGY)) {
                                /**
                                 * El payload de estos registros es un solo float.
                                 */
                                if ( (data.length - 7) == 4 )
                                    frame = new FloatFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if (register == Registers.CURRENT_MEASUREMENTS) {
                                /**
                                 * El payload son 4 floats: tensión, corriente, potencia, energía total acumulada
                                 */
                                if ( (data.length - 7) == 16 )
                                    frame = new FloatArrayFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if (register == Registers.DEVICE_ID) {
                                /**
                                 * El payload es un string de 33 caracteres.
                                 */
                                if ( (data.length - 7) == 33 )
                                    frame = new StringFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if ( (register == Registers.MONDAY_LOAD_ON_TIME) || (register == Registers.MONDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.TUESDAY_LOAD_ON_TIME) || (register == Registers.TUESDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.WEDNESDAY_LOAD_ON_TIME) || (register == Registers.WEDNESDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.THURSDAY_LOAD_ON_TIME) || (register == Registers.THURSDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.FRIDAY_LOAD_ON_TIME) || (register == Registers.FRIDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.SATURDAY_LOAD_ON_TIME) || (register == Registers.SATURDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.SUNDAY_LOAD_ON_TIME) || (register == Registers.SUNDAY_LOAD_OFF_TIME)) {
                                /**
                                 * El payload son 2 bytes que indican la hora y los minutos.
                                 */
                                if ( (data.length - 7) == 2 )
                                    frame = new ByteArrayFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if (register == Registers.ENABLE_ONOFF_TIME) {
                                /**
                                 * El payload es 1 byte que en que cada bit indica si la programación
                                 * horaria del día está habilitada o no. El bit 0 es el domingo y
                                 * el 6 es el sábado.
                                 */
                                if ( (data.length - 7) == 1 )
                                    frame = new ByteFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if (register == Registers.ONOFF_TIMES) {
                                /**
                                 * EL payload se compone de 7 horas de encendido, 7 horas de apagado
                                 * y 1 byte que indica que días están habilitados para la programación
                                 * horaria. El orden de las horas es: lunes encendido, martes encendido, ...
                                 * domingo encendido, lunes apagado, ...., domingo apagado.
                                 * Totalizan 29 bytes.
                                 */
                                if ( (data.length - 7) == 29 )
                                    frame = new ByteArrayFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if (register == Registers.LOAD_STATE) {
                                /**
                                 * El payload es 1 byte que indica si la carga está encendida (1) o
                                 * apagada (0).
                                 */
                                if ( (data.length - 7) == 1 )
                                    frame = new ByteFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if (register == Registers.SYNCH_TIME) {
                                /**
                                 * El payload es 1 byte que indica si el RTC está sincronizado (1) o
                                 * no (0).
                                 */
                                if ( (data.length - 7) == 1 )
                                    frame = new ByteFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if ( (register == Registers.PER_HOUR_ACTIVE_POWER) || (register == Registers.PER_HOUR_ENERGY) ) {
                                /**
                                 * El payload son 3 bytes y 24 mediciones float de energía o potencia activa.
                                 * Totalizan 99 bytes.
                                 */
                                if ( (data.length - 7) == 99 )
                                    frame = new ByteFloatArrayFrame ((byte)length, (byte)command, (byte)register, 3, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                        }
                        else if (command == Commands.RESP_SET) {
                            if (register == Registers.DEVICE_ID) {
                                /**
                                 * El payload es un string de 33 caracteres.
                                 */
                                if ( (data.length - 7) == 33 )
                                    frame = new StringFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                            else if ( (register == Registers.MONDAY_LOAD_ON_TIME) || (register == Registers.MONDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.TUESDAY_LOAD_ON_TIME) || (register == Registers.TUESDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.WEDNESDAY_LOAD_ON_TIME) || (register == Registers.WEDNESDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.THURSDAY_LOAD_ON_TIME) || (register == Registers.THURSDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.FRIDAY_LOAD_ON_TIME) || (register == Registers.FRIDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.SATURDAY_LOAD_ON_TIME) || (register == Registers.SATURDAY_LOAD_OFF_TIME) ||
                                    (register == Registers.SUNDAY_LOAD_ON_TIME) || (register == Registers.SUNDAY_LOAD_OFF_TIME)) {
                                /**
                                 * El payload son 2 bytes que indican la hora y los minutos.
                                 */
                                if ( (data.length - 7) == 2 )
                                    frame = new ByteArrayFrame ((byte)length, (byte)command, (byte)register, Arrays.copyOfRange(data, 5, data.length - 2));
                            }
                        }
                    }
                    else {
                        /**
                         * Si es un comando RESP_NODE_ON o RESP_NODE_OFF ya se puede devolver una instancia de NoParamFame
                         * Si es un comando RESP_RESET, se tiene que extraer el registro de la trama.
                         */
                        if (command == Commands.RESP_RESET)
                            frame = new NoParamFrame((byte)length, (byte)command, data[4]);
                        else
                            frame = new NoParamFrame((byte)length, (byte)command, (byte)0);
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
