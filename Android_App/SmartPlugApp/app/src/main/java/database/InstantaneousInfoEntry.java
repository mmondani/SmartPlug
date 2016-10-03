package database;


import java.util.Date;


public class InstantaneousInfoEntry {
    private String mId;
    private String mIp;
    private Date mLastUpdate;
    private int mConnectionState;
    private int mLoadState;
    private float mVoltage;
    private float mCurrent;
    private float mPower;
    private float mTotalEnergy;
    private int mTimeouts;

    public InstantaneousInfoEntry (String id, String ip, long lastUpdate, int connectionState, int loadState,
                                   float voltage, float current, float power, float totalEnergy, int timeouts) {
        mId = id;
        mIp = ip;
        mLastUpdate = new Date(lastUpdate);
        mConnectionState = connectionState;
        mLoadState = loadState;
        mVoltage = voltage;
        mCurrent = current;
        mPower = power;
        mTotalEnergy = totalEnergy;
        mTimeouts = timeouts;
    }

    public InstantaneousInfoEntry () {
        mId = null;
        mIp = null;
        mLastUpdate = null;
        mConnectionState = 0;
        mLoadState = 0;
        mVoltage = 0.0f;
        mCurrent = 0.0f;
        mPower = 0.0f;
        mTotalEnergy = 0.0f;
        mTimeouts = 0;
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

    public int getLoadState() {
        return mLoadState;
    }

    public void setLoadState(int loadState) {
        mLoadState = loadState;
    }

    public float getVoltage() {
        return mVoltage;
    }

    public void setVoltage(float voltage) {
        mVoltage = voltage;
    }

    public float getCurrent() {
        return mCurrent;
    }

    public void setCurrent(float current) {
        mCurrent = current;
    }

    public float getPower() {
        return mPower;
    }

    public void setPower(float power) {
        mPower = power;
    }

    public float getTotalEnergy() {
        return mTotalEnergy;
    }

    public void setTotalEnergy(float totalEnergy) {
        mTotalEnergy = totalEnergy;
    }

    public int getTimeouts() {
        return mTimeouts;
    }

    public void setTimeouts(int timeouts) {
        mTimeouts = timeouts;
    }
}
