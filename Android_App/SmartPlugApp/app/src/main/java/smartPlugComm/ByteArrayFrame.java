package smartPlugComm;

/**
 * Created by mariano on 28/09/16.
 */
public class ByteArrayFrame extends BasicFrame {
    private byte mRegister;
    private byte[] mData;

    public ByteArrayFrame (byte length, byte command, byte register, byte[] payload) {
        super(Types.BYTE_ARRAY_PARAM, length, command);

        mRegister = register;

        /**
         * El payload se compone de N bytes
         */
        mData = payload;
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }

    public byte[] getData() {
        return mData;
    }

    public void setData(byte[] data) {
        mData = data;
    }
}
