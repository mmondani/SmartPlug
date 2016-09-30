package database;

/**
 * Created by mariano on 30/09/16.
 */
public class MeasurementsEntry {
    private String mId;
    private String mDate;
    private int mMeasurementType;
    private String mMeasurements;


    public static final class MeasurementType {
        public static final int ACTIVE_POWER = 1;
        public static final int ENERGY = 2;
    }

    public MeasurementsEntry (String id, String date, int measurementType, String measurements) {
        mId = id;
        mDate = date;
        mMeasurementType = measurementType;
        mMeasurements = measurements;
    }

    public MeasurementsEntry () {
        mId = null;
        mDate = null;
        mMeasurementType = 0;
        mMeasurements = null;
    }

    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public String getDate() {
        return mDate;
    }

    public void setDate(String date) {
        mDate = date;
    }

    public int getMeasurementType() {
        return mMeasurementType;
    }

    public void setMeasurementType(int measurementType) {
        mMeasurementType = measurementType;
    }

    public String getMeasurements() {
        return mMeasurements;
    }

    public void setMeasurements(String measurements) {
        mMeasurements = measurements;
    }
}
