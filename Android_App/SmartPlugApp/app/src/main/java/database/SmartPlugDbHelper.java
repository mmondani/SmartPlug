package database;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import database.SmartPlugDb.InstantaneousInfoTable;


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

        /** TODO crear las otras 3 tablas en la base de datos */
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {

    }
}