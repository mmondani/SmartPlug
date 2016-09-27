package events;


import java.util.Date;

public class UpdateSmartPlugEvent {
    private String mId;

    public UpdateSmartPlugEvent (String id) {
        mId = id;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }
}
