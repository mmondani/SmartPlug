package database;


import java.util.Calendar;
import java.util.Date;

public class OnOffTimesEntry {
    private String mId;
    private byte mEnabledTimes;
    private SimpleTime mMondayLoadOnTime;
    private SimpleTime mMondayLoadOffTime;
    private SimpleTime mTuesdayLoadOnTime;
    private SimpleTime mTuesdayLoadOffTime;
    private SimpleTime mWednesdayLoadOnTime;
    private SimpleTime mWednesdayLoadOffTime;
    private SimpleTime mThursdayLoadOnTime;
    private SimpleTime mThursdayLoadOffTime;
    private SimpleTime mFridayLoadOnTime;
    private SimpleTime mFridayLoadOffTime;
    private SimpleTime mSaturdayLoadOnTime;
    private SimpleTime mSaturdayLoadOffTime;
    private SimpleTime mSundayLoadOnTime;
    private SimpleTime mSundayLoadOffTime;


    public OnOffTimesEntry (String id, byte enabledTimes, SimpleTime mondayLoadOnTime,
                            SimpleTime mondayLoadOffTime, SimpleTime tuesdayLoadOnTime,
                            SimpleTime tuesdayLoadOffTime, SimpleTime wednesdayLoadOnTime,
                            SimpleTime wednesdayLoadOffTime, SimpleTime thursdayLoadOnTime,
                            SimpleTime thursdayLoadOffTime, SimpleTime fridayLoadOnTime,
                            SimpleTime fridayLoadOffTime, SimpleTime saturdayLoadOnTime,
                            SimpleTime saturdayLoadOffTime, SimpleTime sundayLoadOnTime,
                            SimpleTime sundayLoadOffTime) {
        mId = id;
        mEnabledTimes = enabledTimes;
        mMondayLoadOnTime = mondayLoadOnTime;
        mMondayLoadOffTime = mondayLoadOffTime;
        mTuesdayLoadOnTime = tuesdayLoadOnTime;
        mTuesdayLoadOffTime = tuesdayLoadOffTime;
        mWednesdayLoadOnTime = wednesdayLoadOnTime;
        mWednesdayLoadOffTime = wednesdayLoadOffTime;
        mThursdayLoadOnTime = thursdayLoadOnTime;
        mThursdayLoadOffTime = thursdayLoadOffTime;
        mFridayLoadOnTime = fridayLoadOnTime;
        mFridayLoadOffTime = fridayLoadOffTime;
        mSaturdayLoadOnTime = saturdayLoadOnTime;
        mSaturdayLoadOffTime = saturdayLoadOffTime;
        mSundayLoadOnTime = sundayLoadOnTime;
        mSundayLoadOffTime = sundayLoadOffTime;
    }

    public OnOffTimesEntry () {
        mId = null;
        mEnabledTimes = 0;
        mMondayLoadOnTime = new SimpleTime();
        mMondayLoadOffTime = new SimpleTime();
        mTuesdayLoadOnTime = new SimpleTime();
        mTuesdayLoadOffTime = new SimpleTime();
        mWednesdayLoadOnTime = new SimpleTime();
        mWednesdayLoadOffTime = new SimpleTime();
        mThursdayLoadOnTime = new SimpleTime();
        mThursdayLoadOffTime = new SimpleTime();
        mFridayLoadOnTime = new SimpleTime();
        mFridayLoadOffTime = new SimpleTime();
        mSaturdayLoadOnTime = new SimpleTime();
        mSaturdayLoadOffTime = new SimpleTime();
        mSundayLoadOnTime = new SimpleTime();
        mSundayLoadOffTime = new SimpleTime();
    }


    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public byte getEnabledTimes() {
        return mEnabledTimes;
    }

    public void setEnabledTimes(byte enabledTimes) {
        mEnabledTimes = enabledTimes;
    }

    public SimpleTime getMondayLoadOnTime() {
        return mMondayLoadOnTime;
    }

    public void setMondayLoadOnTime(SimpleTime mondayLoadOnTime) {
        mMondayLoadOnTime = mondayLoadOnTime;
    }

    public SimpleTime getMondayLoadOffTime() {
        return mMondayLoadOffTime;
    }

    public void setMondayLoadOffTime(SimpleTime mondayLoadOffTime) {
        mMondayLoadOffTime = mondayLoadOffTime;
    }

    public SimpleTime getTuesdayLoadOnTime() {
        return mTuesdayLoadOnTime;
    }

    public void setTuesdayLoadOnTime(SimpleTime tuesdayLoadOnTime) {
        mTuesdayLoadOnTime = tuesdayLoadOnTime;
    }

    public SimpleTime getTuesdayLoadOffTime() {
        return mTuesdayLoadOffTime;
    }

    public void setTuesdayLoadOffTime(SimpleTime tuesdayLoadOffTime) {
        mTuesdayLoadOffTime = tuesdayLoadOffTime;
    }

    public SimpleTime getWednesdayLoadOnTime() {
        return mWednesdayLoadOnTime;
    }

    public void setWednesdayLoadOnTime(SimpleTime wednesdayLoadOnTime) {
        mWednesdayLoadOnTime = wednesdayLoadOnTime;
    }

    public SimpleTime getWednesdayLoadOffTime() {
        return mWednesdayLoadOffTime;
    }

    public void setWednesdayLoadOffTime(SimpleTime wednesdayLoadOffTime) {
        mWednesdayLoadOffTime = wednesdayLoadOffTime;
    }

    public SimpleTime getThursdayLoadOnTime() {
        return mThursdayLoadOnTime;
    }

    public void setThursdayLoadOnTime(SimpleTime thursdayLoadOnTime) {
        mThursdayLoadOnTime = thursdayLoadOnTime;
    }

    public SimpleTime getThursdayLoadOffTime() {
        return mThursdayLoadOffTime;
    }

    public void setThursdayLoadOffTime(SimpleTime thursdayLoadOffTime) {
        mThursdayLoadOffTime = thursdayLoadOffTime;
    }

    public SimpleTime getFridayLoadOnTime() {
        return mFridayLoadOnTime;
    }

    public void setFridayLoadOnTime(SimpleTime fridayLoadOnTime) {
        mFridayLoadOnTime = fridayLoadOnTime;
    }

    public SimpleTime getFridayLoadOffTime() {
        return mFridayLoadOffTime;
    }

    public void setFridayLoadOffTime(SimpleTime fridayLoadOffTime) {
        mFridayLoadOffTime = fridayLoadOffTime;
    }

    public SimpleTime getSaturdayLoadOnTime() {
        return mSaturdayLoadOnTime;
    }

    public void setSaturdayLoadOnTime(SimpleTime saturdayLoadOnTime) {
        mSaturdayLoadOnTime = saturdayLoadOnTime;
    }

    public SimpleTime getSaturdayLoadOffTime() {
        return mSaturdayLoadOffTime;
    }

    public void setSaturdayLoadOffTime(SimpleTime saturdayLoadOffTime) {
        mSaturdayLoadOffTime = saturdayLoadOffTime;
    }

    public SimpleTime getSundayLoadOnTime() {
        return mSundayLoadOnTime;
    }

    public void setSundayLoadOnTime(SimpleTime sundayLoadOnTime) {
        mSundayLoadOnTime = sundayLoadOnTime;
    }

    public SimpleTime getSundayLoadOffTime() {
        return mSundayLoadOffTime;
    }

    public void setSundayLoadOffTime(SimpleTime sundayLoadOffTime) {
        mSundayLoadOffTime = sundayLoadOffTime;
    }
}
