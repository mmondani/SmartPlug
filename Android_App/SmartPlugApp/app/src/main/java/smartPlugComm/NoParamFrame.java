package smartPlugComm;

/**
 * Created by mariano on 27/09/16.
 */
public class NoParamFrame extends BasicFrame {

    private byte mRegister;

    public NoParamFrame (byte length, byte command, byte register) {
        super(Types.NO_PARAM, length, command);

        mRegister = register;
    }

    public byte getRegister() {
        return mRegister;
    }

    public void setRegister(byte register) {
        mRegister = register;
    }
}
