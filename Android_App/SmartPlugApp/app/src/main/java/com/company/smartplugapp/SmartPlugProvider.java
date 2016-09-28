package com.company.smartplugapp;


import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.util.ArrayList;
import java.util.List;

import database.InstantaneousInfoCursoWrapper;
import database.InstantaneousInfoEntry;
import database.OnOffTimesCursorWraper;
import database.OnOffTimesEntry;
import database.SmartPlugDb;
import database.SmartPlugDb.InstantaneousInfoTable;
import database.SmartPlugDb.OnOffTimesTable;
import database.SmartPlugDb.StaticInfoTable;
import database.SmartPlugDbHelper;
import database.StaticInfoCursorWraper;
import database.StaticInfoEntry;


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
         */
        ContentValues values;

        /**
         * Nueva entrada en la tabla InstantaneousInfoTable
         */
        InstantaneousInfoEntry instantaneousInfoEntry = new InstantaneousInfoEntry();
        instantaneousInfoEntry.setId(id);
        instantaneousInfoEntry.setIp(ip);
        values = getInstantaneousInfoContentValues(instantaneousInfoEntry);
        mDatabase.insert(InstantaneousInfoTable.NAME, null, values);


        /**
         * Nueva entrada en la tabla StaticInfoTable.
         */
        StaticInfoEntry staticInfoEntry = new StaticInfoEntry();
        staticInfoEntry.setId(id);
        values = getStaticInfoContentValues(staticInfoEntry);
        mDatabase.insert(StaticInfoTable.NAME, null, values);


        /**
         * Nueva entrada en la tabla OnOffTimesTable.
         */
        OnOffTimesEntry onOffTimesEntry = new OnOffTimesEntry();
        onOffTimesEntry.setId(id);
        values = getOnOffTimesContentValues(onOffTimesEntry);
        mDatabase.insert(OnOffTimesTable.NAME, null, values);
    }


    public List<SmartPlugListItem> getSmartPlugs () {
        List<SmartPlugListItem> contacts = new ArrayList<>();

        InstantaneousInfoCursoWrapper cursorInstantaneous = queryInstantaneousInfo(null, null);
        StaticInfoCursorWraper cursorStatic = queryStaticInfo(null, null);

        /**
         * Se leen en paralelo ambos cursores ya que tienen que tener la misma cantidad de filas
         * cada tabla. Y en el mismo orden.
         */
        try {
            cursorInstantaneous.moveToFirst();
            cursorStatic.moveToFirst();
            while (!cursorInstantaneous.isAfterLast() && !cursorStatic.isAfterLast()) {
                InstantaneousInfoEntry instInfoEntry = cursorInstantaneous.getInstantaneousInfoEntry();
                StaticInfoEntry staInfoEntry = cursorStatic.getStaticInfoEntry();

                contacts.add(new SmartPlugListItem(staInfoEntry.getIconId(),
                        instInfoEntry.getId(),
                        staInfoEntry.getName(),
                        instInfoEntry.getLastUpdate(),
                        instInfoEntry.getConnectionState(),
                        instInfoEntry.getLoadState()));

                cursorInstantaneous.moveToNext();
                cursorStatic.moveToNext();
            }
        } finally {
            cursorInstantaneous.close();
        }


        return contacts;
    }


    /**
     * *********************************************************************************************
     * Métodos para acceder a la tabla InstantaneousInfoTable
     * *********************************************************************************************
     */


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


    /**
     * *********************************************************************************************
     * Métodos para acceder a la tabla StaticInfoTable
     * *********************************************************************************************
     */

    public StaticInfoEntry getStaticInfoEntry (String id) {
        StaticInfoCursorWraper cursor = queryStaticInfo(
                StaticInfoTable.Cols.ID + " = ?",
                new String[]{id}
        );

        try {
            if (cursor.getCount() == 0)
                return null;

            cursor.moveToFirst();
            return cursor.getStaticInfoEntry();
        }finally {
            cursor.close();
        }
    }


    public void updateStaticInfoEntry (StaticInfoEntry entry) {
        ContentValues values = getStaticInfoContentValues(entry);

        mDatabase.update(StaticInfoTable.NAME, values, StaticInfoTable.Cols.ID + " = ?",
                new String[]{entry.getId()});
    }

    private StaticInfoCursorWraper queryStaticInfo (String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                StaticInfoTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );

        return new StaticInfoCursorWraper(cursor);
    }

    private static ContentValues getStaticInfoContentValues (StaticInfoEntry entry) {
        ContentValues values = new ContentValues();

        values.put(StaticInfoTable.Cols.ID, entry.getId());
        values.put(StaticInfoTable.Cols.NAME, entry.getName());
        values.put(StaticInfoTable.Cols.ICON_ID, entry.getIconId());

        return values;
    }


    /**
     * *********************************************************************************************
     * Métodos para acceder a la tabla OnOffTimesTable
     * *********************************************************************************************
     */

    public OnOffTimesEntry getOnOffTimesEntry (String id) {
        OnOffTimesCursorWraper cursor = queryOnOffTimes(
                OnOffTimesTable.Cols.ID + " = ?",
                new String[]{id}
        );

        try {
            if (cursor.getCount() == 0)
                return null;

            cursor.moveToFirst();
            return cursor.getOnOffTimesEntry();
        }finally {
            cursor.close();
        }
    }


    public void updateOnOffTimesEntry (OnOffTimesEntry entry) {
        ContentValues values = getOnOffTimesContentValues(entry);

        mDatabase.update(OnOffTimesTable.NAME, values, OnOffTimesTable.Cols.ID + " = ?",
                new String[]{entry.getId()});
    }

    private OnOffTimesCursorWraper queryOnOffTimes (String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                OnOffTimesTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );

        return new OnOffTimesCursorWraper(cursor);
    }

    private static ContentValues getOnOffTimesContentValues (OnOffTimesEntry entry) {
        ContentValues values = new ContentValues();

        values.put(OnOffTimesTable.Cols.ID, entry.getId());
        values.put(OnOffTimesTable.Cols.ENABLED_TIMES, entry.getEnabledTimes());
        values.put(OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.MONDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.TUESDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.TUESDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.WEDNESDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.WEDNESDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.THURSDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.THURSDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.FRIDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.FRIDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.SATURDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.SATURDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.SUNDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.SUNDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());

        return values;
    }
}
