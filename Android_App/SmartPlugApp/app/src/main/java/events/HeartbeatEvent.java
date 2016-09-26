package events;

import java.util.Date;

/**
 * Created by mariano on 26/09/16.
 */
public class HeartbeatEvent {
    private String mId;
    private String mIp;
    private Date mDate;

    public HeartbeatEvent (String id, String ip, Date date) {
        mId = id;
        mIp = ip;
        mDate = date;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public String getIp() {
        return mIp;
    }

    public void setIp(String ip) {
        mIp = ip;
    }

    public Date getDate() {
        return mDate;
    }

    public void setDate(Date date) {
        mDate = date;
    }
}
