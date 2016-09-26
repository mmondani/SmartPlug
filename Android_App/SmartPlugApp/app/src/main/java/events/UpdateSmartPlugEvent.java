package events;


import java.util.Date;

public class UpdateSmartPlugEvent {
    private String mId;
    private Date mDate;

    public UpdateSmartPlugEvent (String id, Date date) {
        mId = id;
        mDate = date;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public Date getDate() {
        return mDate;
    }

    public void setDate(Date date) {
        mDate = date;
    }
}
