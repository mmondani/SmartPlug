package com.company.smartplugapp;


import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import database.InstantaneousInfoCursoWrapper;
import database.InstantaneousInfoEntry;
import database.SmartPlugDb;
import database.SmartPlugDb.InstantaneousInfoTable;
import database.SmartPlugDbHelper;


public class SmartPlugProvider {
    private static SmartPlugProvider ourInstance = null;
    private SQLiteDatabase mDatabase;


    public static SmartPlugProvider getInstance(Context context) {
        if (ourInstance == null)
            ourInstance = new SmartPlugProvider(context);

        return ourInstance;
    }

    private SmartPlugProvider(Context context) {
        mDatabase = new SmartPlugDbHelper(context).getWritableDatabase();
    }


    /**
     * Retorna si el Smart Plug con ID id existe en la base de datos no.
     * @param id ID del Smart Plug.
     * @return false si no existe.
     *          true si existe.
     */
    public boolean isSmartPlugInDb (String id) {
        InstantaneousInfoCursoWrapper cursor = queryInstantaneousInfo(
                InstantaneousInfoTable.Cols.ID + " = ?",
                new String[]{id}
        );

        try {
            if (cursor.getCount() == 0)
                return false;
            else
                return true;
        }finally {
            cursor.close();
        }
    }

    /**
     * Agrega un Smart Plug a la base de datos. Se lo agrega con los datos mínimos
     * @param id ID del Smart Plug a agregar.
     * @param ip IP actual del Smart Plug.
     */
    public void addSmartPlug (String id, String ip) {
        /**
         * Se agrega una entrada en las tablas: InstantaneousInfo, StaticInfo y OnOffTimes.
         * TODO agrega la entrada en StaticInfo y en OnOffTimes.
         */
        InstantaneousInfoEntry instantaneousInfoEntry = new InstantaneousInfoEntry();
        instantaneousInfoEntry.setId(id);
        instantaneousInfoEntry.setIp(ip);
        ContentValues values = getInstantaneousInfoContentValues(instantaneousInfoEntry);

        mDatabase.insert(InstantaneousInfoTable.NAME, null, values);
    }


    public InstantaneousInfoEntry getInstantaneousInfoEntry (String id) {
        InstantaneousInfoCursoWrapper cursor = queryInstantaneousInfo(
                InstantaneousInfoTable.Cols.ID + " = ?",
                new String[]{id}
        );

        try {
            if (cursor.getCount() == 0)
                return null;

            cursor.moveToFirst();
            return cursor.getInstantaneousInfoEntry();
        }finally {
            cursor.close();
        }
    }


    public void updateInstantaneousInfoEntry (InstantaneousInfoEntry entry) {
        ContentValues values = getInstantaneousInfoContentValues(entry);

        mDatabase.update(InstantaneousInfoTable.NAME, values, InstantaneousInfoTable.Cols.ID + " = ?",
                new String[]{entry.getId()});
    }

    private InstantaneousInfoCursoWrapper queryInstantaneousInfo (String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                InstantaneousInfoTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );

        return new InstantaneousInfoCursoWrapper(cursor);
    }

    private static ContentValues getInstantaneousInfoContentValues (InstantaneousInfoEntry entry) {
        ContentValues values = new ContentValues();

        values.put(InstantaneousInfoTable.Cols.ID, entry.getId());
        values.put(InstantaneousInfoTable.Cols.IP, entry.getIp());
        if (entry.getLastUpdate() != null)
            values.put(InstantaneousInfoTable.Cols.LAST_UPDATE, entry.getLastUpdate().getTime());
        else
            values.put(InstantaneousInfoTable.Cols.LAST_UPDATE, 0);
        values.put(InstantaneousInfoTable.Cols.CONNECTION_STATE, entry.getConnectionState());
        values.put(InstantaneousInfoTable.Cols.LOAD_STATE, entry.getLoadState());
        values.put(InstantaneousInfoTable.Cols.VOLTAGE, entry.getVoltage());
        values.put(InstantaneousInfoTable.Cols.CURRENT, entry.getCurrent());
        values.put(InstantaneousInfoTable.Cols.POWER, entry.getPower());
        values.put(InstantaneousInfoTable.Cols.TOTAL_ENERGY, entry.getTotalEnergy());

        return values;
    }
}
