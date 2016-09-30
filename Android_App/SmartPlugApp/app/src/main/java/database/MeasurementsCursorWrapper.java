package database;

import android.database.Cursor;
import android.database.CursorWrapper;

import database.SmartPlugDb.MeasurementsTable;

/**
 * Created by mariano on 30/09/16.
 */
public class MeasurementsCursorWrapper extends CursorWrapper{

    public MeasurementsCursorWrapper (Cursor cursor) {
        super (cursor);
    }

    public MeasurementsEntry getMeasurementsEntry () {
        String id = getString(getColumnIndex(MeasurementsTable.Cols.ID));
        String date = getString(getColumnIndex(MeasurementsTable.Cols.DATE));
        int measurementType = getInt(getColumnIndex(MeasurementsTable.Cols.MEASUREMENT_TYPE));
        String measurements = getString(getColumnIndex(MeasurementsTable.Cols.MEASUREMENTS));

        MeasurementsEntry entry = new MeasurementsEntry(id, date, measurementType, measurements);

        return entry;
    }
}
