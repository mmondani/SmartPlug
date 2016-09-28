package database;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import database.SmartPlugDb.InstantaneousInfoTable;
import database.SmartPlugDb.OnOffTimesTable;
import database.SmartPlugDb.StaticInfoTable;


public class SmartPlugDbHelper extends SQLiteOpenHelper {
    private static final int VERSION = 1;
    private static final String DATABASE_NAME = "smartPlug.db";

    public SmartPlugDbHelper (Context context) {
        super(context, DATABASE_NAME, null, VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        db.execSQL("create table " + InstantaneousInfoTable.NAME + " (" +
                "_id integer primary key autoincrement, " +
                InstantaneousInfoTable.Cols.ID + ", " +
                InstantaneousInfoTable.Cols.IP + ", " +
                InstantaneousInfoTable.Cols.LAST_UPDATE + ", " +
                InstantaneousInfoTable.Cols.CONNECTION_STATE + ", " +
                InstantaneousInfoTable.Cols.LOAD_STATE + ", " +
                InstantaneousInfoTable.Cols.VOLTAGE + ", " +
                InstantaneousInfoTable.Cols.CURRENT + ", " +
                InstantaneousInfoTable.Cols.POWER + ", " +
                InstantaneousInfoTable.Cols.TOTAL_ENERGY + ")");

        db.execSQL("create table " + StaticInfoTable.NAME + " (" +
                "_id integer primary key autoincrement, " +
                StaticInfoTable.Cols.ID + ", " +
                StaticInfoTable.Cols.NAME + ", " +
                StaticInfoTable.Cols.ICON_ID + ")");

        db.execSQL("create table " + OnOffTimesTable.NAME + " (" +
                "_id integer primary key autoincrement, " +
                OnOffTimesTable.Cols.ID + ", " +
                OnOffTimesTable.Cols.ENABLED_TIMES + ", " +
                OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.MONDAY_LOAD_OFF_TIME + ", " +
                OnOffTimesTable.Cols.TUESDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.TUESDAY_LOAD_OFF_TIME + ", " +
                OnOffTimesTable.Cols.WEDNESDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.WEDNESDAY_LOAD_OFF_TIME + ", " +
                OnOffTimesTable.Cols.THURSDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.THURSDAY_LOAD_OFF_TIME + ", " +
                OnOffTimesTable.Cols.FRIDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.FRIDAY_LOAD_OFF_TIME + ", " +
                OnOffTimesTable.Cols.SATURDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.SATURDAY_LOAD_OFF_TIME + ", " +
                OnOffTimesTable.Cols.SUNDAY_LOAD_ON_TIME + ", " +
                OnOffTimesTable.Cols.SUNDAY_LOAD_OFF_TIME + ")");

        /** TODO crear la otra tabla en la base de datos */
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
}
