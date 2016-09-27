package events;

/**
 * Created by mariano on 27/09/16.
 */
public class CommandEvent {
    private String mId;
    private byte[] mData;

    public CommandEvent (String id, byte[] data) {
        mId = id;
        mData = data;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public byte[] getData() {
        return mData;
    }

    public void setData(byte[] data) {
        mData = data;
    }
}
