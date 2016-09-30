package smartPlugComm;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

/**
 * Created by mariano on 28/09/16.
 */
public class FloatArrayFrame extends BasicFrame {
    private byte mRegister;
    private float[] mData;

    public FloatArrayFrame (byte length, byte command, byte register, byte[] payload) {
        super(Types.FLOAT_ARRAY_PARAM, length, command);

        mRegister = register;

        /**
         * El payload se compone de N floats (N * 4 bytes)
         */
        if ( (payload.length % 4) == 0 ) {
            mData = new float[payload.length / 4];

            for (int i = 0; i < mData.length; i++) {
                byte[] data = Arrays.copyOfRange(payload, i*4, i*4 + 4);
                ByteBuffer buff = ByteBuffer.wrap(data);
                buff.order(ByteOrder.BIG_ENDIAN);
                mData[i] = buff.getFloat();
            }
        }
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }

    public float[] getData() {
        return mData;
    }

    public void setData(float[] data) {
        mData = data;
    }
}
