package database;


import android.database.Cursor;
import android.database.CursorWrapper;

import java.util.Date;

public class InstantaneousInfoCursoWrapper extends CursorWrapper{

    public InstantaneousInfoCursoWrapper (Cursor cursor) {
        super (cursor);
    }

    public InstantaneousInfoEntry getInstantaneousInfoEntry () {
        String id = getString(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.ID));
        String ip = getString(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.IP));
        long lastUpdate = getLong(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.LAST_UPDATE));
        int connectionState = getInt(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.CONNECTION_STATE));
        int loadState = getInt(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.LOAD_STATE));
        float voltage = getFloat(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.VOLTAGE));
        float current = getFloat(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.CURRENT));
        float power = getFloat(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.POWER));
        float totalEnergy = getFloat(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.TOTAL_ENERGY));
        int timeouts = getInt(getColumnIndex(SmartPlugDb.InstantaneousInfoTable.Cols.TIMEOUTS));

        InstantaneousInfoEntry entry = new InstantaneousInfoEntry(id, ip, lastUpdate, connectionState, loadState,
                voltage, current, power, totalEnergy, timeouts);

        return entry;
    }
}
