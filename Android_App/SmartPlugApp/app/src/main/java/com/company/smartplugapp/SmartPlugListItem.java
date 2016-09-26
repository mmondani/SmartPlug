package com.company.smartplugapp;


import java.util.Date;

public class SmartPlugListItem {
    private int mIconId;
    private String mId;
    private String mName;
    private Date mLastUpdate;
    private int mConnectionState;
    private boolean mOn;

    public SmartPlugListItem (int iconId, String id, String name, Date lastUpdate, int connectionState, int on) {
        mIconId = iconId;
        mId = id;
        mName = name;
        mLastUpdate = lastUpdate;
        mConnectionState = connectionState;
        mOn = (on == 0)? false : true;
    }

    public int getIconId() {
        return mIconId;
    }

    public void setIconId(int iconId) {
        mIconId = iconId;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public String getName() {
        return mName;
    }

    public void setName(String name) {
        mName = name;
    }

    public Date getLastUpdate() {
        return mLastUpdate;
    }

    public void setLastUpdate(Date lastUpdate) {
        mLastUpdate = lastUpdate;
    }

    public int getConnectionState() {
        return mConnectionState;
    }

    public void setConnectionState(int connectionState) {
        mConnectionState = connectionState;
    }

    public boolean isOn() {
        return mOn;
    }

    public void setOn(boolean on) {
        mOn = on;
    }
}
