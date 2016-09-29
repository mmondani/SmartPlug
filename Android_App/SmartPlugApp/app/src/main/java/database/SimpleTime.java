package database;

/**
 * Created by mariano on 28/09/16.
 */
public class SimpleTime {
    private byte mHours;
    private byte mMinutes;
    private byte mSeconds;

    public SimpleTime (byte hours, byte minutes, byte seconds) {
        mHours = hours;
        mMinutes = minutes;
        mSeconds = seconds;
    }

    public SimpleTime (byte hours, byte minutes) {
        mHours = hours;
        mMinutes = minutes;
        mSeconds = 0;
    }

    public SimpleTime (byte hours) {
        mHours = hours;
        mMinutes = 0;
        mSeconds = 0;
    }

    public SimpleTime () {
        mHours = 0;
        mMinutes = 0;
        mSeconds = 0;
    }

    public SimpleTime (String timeString) {
        String[] timeElements = timeString.split(":");

        if (timeElements.length == 3) {
            int auxValue;

            auxValue = Integer.parseInt(timeElements[0]);
            if (auxValue >= 0 && auxValue <= 24)
                mHours = (byte)auxValue;
            else
                mHours = 0;

            auxValue = Integer.parseInt(timeElements[1]);
            if (auxValue >= 0 && auxValue <= 60)
                mMinutes = (byte)auxValue;
            else
                mMinutes = 0;

            auxValue = Integer.parseInt(timeElements[2]);
            if (auxValue >= 0 && auxValue <= 60)
                mSeconds = (byte)auxValue;
            else
                mSeconds = 0;
        }
        else {
            mHours = 0;
            mMinutes = 0;
            mSeconds = 0;
        }

    }

    @Override
    public String toString() {
        return (String.format("%02d", mHours) + ":" +
                String.format("%02d", mMinutes) + ":" +
                String.format("%02d", mSeconds));
    }

    public String toHourMinutes() {
        return (String.format("%02d", mHours) + ":" + String.format("%02d", mMinutes));
    }

    public byte getHours() {
        return mHours;
    }

    public void setHours(byte hours) {
        mHours = hours;
    }

    public byte getMinutes() {
        return mMinutes;
    }

    public void setMinutes(byte minutes) {
        mMinutes = minutes;
    }

    public byte getSeconds() {
        return mSeconds;
    }

    public void setSeconds(byte seconds) {
        mSeconds = seconds;
    }
}
