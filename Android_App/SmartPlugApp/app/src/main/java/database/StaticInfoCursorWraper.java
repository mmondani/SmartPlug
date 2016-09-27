package database;

import android.database.Cursor;
import android.database.CursorWrapper;


public class StaticInfoCursorWraper extends CursorWrapper {

    public StaticInfoCursorWraper (Cursor cursor) {
        super (cursor);
    }

    public StaticInfoEntry getStaticInfoEntry () {
        String id = getString(getColumnIndex(SmartPlugDb.StaticInfoTable.Cols.ID));
        String name = getString(getColumnIndex(SmartPlugDb.StaticInfoTable.Cols.NAME));
        int iconId = getInt(getColumnIndex(SmartPlugDb.StaticInfoTable.Cols.ICON_ID));

        StaticInfoEntry entry = new StaticInfoEntry(id, name, iconId);

        return entry;
    }
}
