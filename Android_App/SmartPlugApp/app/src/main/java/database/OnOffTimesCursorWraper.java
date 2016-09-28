package database;

import android.database.Cursor;
import android.database.CursorWrapper;


public class OnOffTimesCursorWraper extends CursorWrapper {

    public OnOffTimesCursorWraper (Cursor cursor) {
        super (cursor);
    }

    public OnOffTimesEntry getOnOffTimesEntry () {
        String id = getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.ID));
        int enabledTimes = getInt(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.ENABLED_TIMES));
        SimpleTime mondayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime mondayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime tuesdayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime tuesdayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime wednesdayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime wedenesdayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime thursdayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime thursdayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime fridayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime fridayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime saturdayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime saturdayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime sundayLoadOnTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));
        SimpleTime sundayLoadOffTime = new SimpleTime(getString(getColumnIndex(SmartPlugDb.OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME)));

        OnOffTimesEntry entry = new OnOffTimesEntry(id, (byte)enabledTimes, mondayLoadOnTime,
                                                    mondayLoadOffTime, tuesdayLoadOnTime, tuesdayLoadOffTime,
                                                    wednesdayLoadOnTime, wedenesdayLoadOffTime,
                                                    thursdayLoadOnTime, thursdayLoadOffTime,
                                                    fridayLoadOnTime, fridayLoadOffTime,
                                                    saturdayLoadOnTime, saturdayLoadOffTime,
                                                    sundayLoadOnTime, sundayLoadOffTime);

        return entry;
    }

}
