package com.company.smartplugapp;


import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

import java.util.ArrayList;
import java.util.List;

import database.InstantaneousInfoCursoWrapper;
import database.InstantaneousInfoEntry;
import database.MeasurementsCursorWrapper;
import database.MeasurementsEntry;
import database.OnOffTimesCursorWrapper;
import database.OnOffTimesEntry;
import database.SmartPlugDb;
import database.SmartPlugDb.InstantaneousInfoTable;
import database.SmartPlugDb.MeasurementsTable;
import database.SmartPlugDb.OnOffTimesTable;
import database.SmartPlugDb.StaticInfoTable;
import database.SmartPlugDbHelper;
import database.StaticInfoCursorWrapper;
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
        StaticInfoCursorWrapper cursorStatic = queryStaticInfo(null, null);

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
            cursorStatic.close();
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
        StaticInfoCursorWrapper cursor = queryStaticInfo(
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

    private StaticInfoCursorWrapper queryStaticInfo (String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                StaticInfoTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );

        return new StaticInfoCursorWrapper(cursor);
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
        OnOffTimesCursorWrapper cursor = queryOnOffTimes(
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

    private OnOffTimesCursorWrapper queryOnOffTimes (String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                OnOffTimesTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );

        return new OnOffTimesCursorWrapper(cursor);
    }

    private static ContentValues getOnOffTimesContentValues (OnOffTimesEntry entry) {
        ContentValues values = new ContentValues();

        values.put(OnOffTimesTable.Cols.ID, entry.getId());
        values.put(OnOffTimesTable.Cols.ENABLED_TIMES, entry.getEnabledTimes());
        values.put(OnOffTimesTable.Cols.MONDAY_LOAD_ON_TIME, entry.getMondayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.MONDAY_LOAD_OFF_TIME, entry.getMondayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.TUESDAY_LOAD_ON_TIME, entry.getTuesdayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.TUESDAY_LOAD_OFF_TIME, entry.getTuesdayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.WEDNESDAY_LOAD_ON_TIME, entry.getWednesdayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.WEDNESDAY_LOAD_OFF_TIME, entry.getWednesdayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.THURSDAY_LOAD_ON_TIME, entry.getThursdayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.THURSDAY_LOAD_OFF_TIME, entry.getThursdayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.FRIDAY_LOAD_ON_TIME, entry.getFridayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.FRIDAY_LOAD_OFF_TIME, entry.getFridayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.SATURDAY_LOAD_ON_TIME, entry.getSaturdayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.SATURDAY_LOAD_OFF_TIME, entry.getSaturdayLoadOffTime().toString());
        values.put(OnOffTimesTable.Cols.SUNDAY_LOAD_ON_TIME, entry.getSundayLoadOnTime().toString());
        values.put(OnOffTimesTable.Cols.SUNDAY_LOAD_OFF_TIME, entry.getSundayLoadOffTime().toString());

        return values;
    }



    /**
     * *********************************************************************************************
     * Métodos para acceder a la tabla MeasurementsTable
     * *********************************************************************************************
     */

    public void addMeasurementsEntry (MeasurementsEntry entry) {
        /**
         * Se agrega una entrada en las tabla MeasurementsTable.
         */
        ContentValues values = getMeasurementsContentValues(entry);

        mDatabase.insert(MeasurementsTable.NAME, null, values);
    }

    /**
     * Devuelve la entrada de la tabla MeasurementsTable que tenga ID = id, DATE = date y
     * MEASUREMENT_TYPE = measurementType
     * @param id ID del Smart Plug que se quiere consultar.
     * @param date Fecha de las mediciones que se quieren consultar. Formato: DD/MM/AA
     * @param measurementType Tipo de medición que se está consultando: MeasurementsEntry.MeasurementType.ACTIVE_POWER
     *                        o MeasurementsEntry.MeasurementType.ENERGY.
     * @return instancia de MeasurementsEntry si se la encontró o null si no se la encontró
     */
    public MeasurementsEntry getMeasurementsEntry (String id, String date, int measurementType) {
        /**
         * Se castea la columna MEASUREMENT_TYPE a TEXT para poder compararla con un string.
         */
        MeasurementsCursorWrapper cursor = queryMeasurements(
                MeasurementsTable.Cols.ID + " = ? AND " + MeasurementsTable.Cols.DATE + " = ? AND " +
                        "CAST(" + MeasurementsTable.Cols.MEASUREMENT_TYPE + " as TEXT) = ?",
                new String[]{id, date, Integer.toString(measurementType)}
        );

        try {
            if (cursor.getCount() == 0)
                return null;

            cursor.moveToFirst();
            return cursor.getMeasurementsEntry();
        }finally {
            cursor.close();
        }
    }

    /**
     * Devuelve una lista de MeasurementsEntry con todas las entradas de la tabla MeasurementsTable
     * que tengan ID = id y MEASUREMENT_TYPE = measurementType.
     * @param id ID del Smart Plug que se quiere consultar.
     * @param measurementType Tipo de medición que se está consultando: MeasurementsEntry.MeasurementType.ACTIVE_POWER
     *                        o MeasurementsEntry.MeasurementType.ENERGY.
     * @return instancia de List<MeasurementsEntry> o null si no encontró ninguna entrada.
     */
    public List<MeasurementsEntry> getMeasurementsEntries (String id, int measurementType) {
        List<MeasurementsEntry> measurementsEntries = new ArrayList<>();

        /**
         * Se castea la columna MEASUREMENT_TYPE a TEXT para poder compararla con un string.
         */
        MeasurementsCursorWrapper cursor = queryMeasurements(
                MeasurementsTable.Cols.ID + " = ? AND " +
                        "CAST(" + MeasurementsTable.Cols.MEASUREMENT_TYPE  + " as TEXT) = ?",
                new String[]{id, Integer.toString(measurementType)}
        );

        try {
            if (cursor.getCount() == 0)
                return null;

            cursor.moveToFirst();
            while (!cursor.isAfterLast()) {
                MeasurementsEntry entry = cursor.getMeasurementsEntry();
                measurementsEntries.add(entry);

                cursor.moveToNext();
            }
        }finally {
            cursor.close();
        }

        return measurementsEntries;
    }


    /**
     * Actualiza la entrada correspondiente en la tabla MeasurementsTable.
     * @param entry Entrada que se quiere actualizar.
     */
    public void updateMeasurementsEntry (MeasurementsEntry entry) {
        ContentValues values = getMeasurementsContentValues(entry);

        /**
         * Se castea la columna MEASUREMENT_TYPE a TEXT para poder compararla con un string.
         */
        mDatabase.update(MeasurementsTable.NAME, values,
                MeasurementsTable.Cols.ID + " = ? AND " +
                        MeasurementsTable.Cols.DATE + " = ? AND " +
                        "CAST(" + MeasurementsTable.Cols.MEASUREMENT_TYPE  + " as TEXT) = ?",
                new String[]{entry.getId(), entry.getDate(), Integer.toString(entry.getMeasurementType())});
    }


    /**
     * Devuelve si existe una entrada en la tabla MeasurementsTable o no.
     * @param id ID del dispositivo que se quiere consultar.
     * @param date Fecha de la medición que se quiere consultar.
     * @param measurementType Tipo de medición que se quiere consultar.
     * @return true si la entrada existe, false en caso contrario.
     */
    public boolean existMeasurementsEntry (String id, String date, int measurementType) {
        /**
         * Se castea la columna MEASUREMENT_TYPE a TEXT para poder compararla con un string.
         */
        MeasurementsCursorWrapper cursor = queryMeasurements(
                MeasurementsTable.Cols.ID + " = ? AND " + MeasurementsTable.Cols.DATE + " = ? AND " +
                        "CAST(" + MeasurementsTable.Cols.MEASUREMENT_TYPE  + " as TEXT) = ?",
                new String[]{id, date, Integer.toString(measurementType)}
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
     * Realiza una consulta (SELECT) sobre la tabla MeasurementsTable devolviendo las entradas que cumplan
     * con whereClause.
     * @param whereClause Where clause de la consulta SELECT usada para filtrar el resultado.
     * @param whereArgs Parámetros que completan la where clause.
     * @return instancia de MeasurementsCursorWrapper apuntando al principio del resultado del SELECT.
     */
    private MeasurementsCursorWrapper queryMeasurements (String whereClause, String[] whereArgs) {
        Cursor cursor = mDatabase.query(
                MeasurementsTable.NAME,
                null,
                whereClause,
                whereArgs,
                null,
                null,
                null
        );

        return new MeasurementsCursorWrapper(cursor);
    }

    /**
     * Devuelve los ContentValues de la entrda entry.
     * @param entry Entrada con la que cargar la instancia de ContentValues.
     * @return instancia de ContentValues con los valores de entry cargados.
     */
    private static ContentValues getMeasurementsContentValues (MeasurementsEntry entry) {
        ContentValues values = new ContentValues();

        values.put(MeasurementsTable.Cols.ID, entry.getId());
        values.put(MeasurementsTable.Cols.DATE, entry.getDate());
        values.put(MeasurementsTable.Cols.MEASUREMENT_TYPE, entry.getMeasurementType());
        values.put(MeasurementsTable.Cols.MEASUREMENTS, entry.getMeasurements());


        return values;
    }
}
