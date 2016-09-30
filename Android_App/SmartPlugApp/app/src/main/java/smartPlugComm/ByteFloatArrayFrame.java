package smartPlugComm;

import android.text.TextUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

/**
 * Created by mariano on 30/09/16.
 */
public class ByteFloatArrayFrame extends BasicFrame {
    private byte mRegister;
    private byte[] mDataBytes;
    private float[] mDataFloats;

    public ByteFloatArrayFrame (byte length, byte command, byte register, int numberOfBytes, byte[] payload) {
        super(Types.BYTE_FLOAT_ARRAY_PARAM, length, command);

        mRegister = register;

        /**
         * El payload se compone de numberOfBytes bytes al principio y N floats (N * 4 bytes) después
         */
        if ( ( (payload.length-numberOfBytes) % 4) == 0 ) {
            mDataBytes = new byte[numberOfBytes];
            for (int i = 0; i < numberOfBytes; i++)
                mDataBytes[i] = payload[i];

            mDataFloats = new float[(payload.length-numberOfBytes) / 4];

            for (int i = numberOfBytes; i < mDataFloats.length; i++) {
                byte[] data = Arrays.copyOfRange(payload, i * 4, i * 4 + 4);
                ByteBuffer buff = ByteBuffer.wrap(data);
                buff.order(ByteOrder.BIG_ENDIAN);
                mDataFloats[i] = buff.getFloat();
            }
        }
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }

    public byte[] getDataBytes() {
        return mDataBytes;
    }

    public void setDataBytes(byte[] dataBytes) {
        mDataBytes = dataBytes;
    }

    public float[] getDataFloats() {
        return mDataFloats;
    }

    public void setDataFloats(float[] dataFloats) {
        mDataFloats = dataFloats;
    }


    public String joinFloats (String delimiter) {
        /**
         * Se crea una array de Float para pasárselo a TextUtils.join.
         */
        Float[] arrayToConvert = new Float[mDataFloats.length];
        for (int i = 0; i < mDataFloats.length; i ++) {
            arrayToConvert[i] = mDataFloats[i];
        }

        if (mDataFloats.length > 0)
            return TextUtils.join(delimiter, arrayToConvert);
        else
            return null;
    }
}
