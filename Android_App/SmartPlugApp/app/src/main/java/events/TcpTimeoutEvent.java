package events;

/**
 * Created by mariano on 03/10/16.
 */
public class TcpTimeoutEvent {
    private String mId;

    public TcpTimeoutEvent (String id) {
        mId = id;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }
}
