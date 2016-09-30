package smartPlugComm;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class FloatFrame extends BasicFrame {

    private byte mRegister;
    private float mData;

    public FloatFrame (byte length, byte command, byte register, byte[] payload) {
        super(Types.FLOAT_PARAM, length, command);

        mRegister = register;

        /**
         * El payload se compone de 1 float (4 bytes)
         */
        if (payload.length == 4)
            mData = ByteBuffer.wrap(payload).order(ByteOrder.LITTLE_ENDIAN).getFloat();
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }

    public float getData() {
        return mData;
    }

    public void setData(float data) {
        mData = data;
    }
}
