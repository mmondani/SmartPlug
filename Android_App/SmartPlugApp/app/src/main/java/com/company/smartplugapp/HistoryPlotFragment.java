package com.company.smartplugapp;


import android.content.pm.ActivityInfo;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.AxisBase;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.formatter.AxisValueFormatter;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import database.MeasurementsEntry;


/**
 * A simple {@link Fragment} subclass.
 */
public class HistoryPlotFragment extends Fragment {

    private LineChart mPlot;
    private TextView mPlotNameText;

    private String mId;
    private String mDate;
    private int mMeasurementType;

    private static final String ARG_ID = "id";
    private static final String ARG_DATE = "date";
    private static final String ARG_MEASUREMENT_TYPE = "measurement_Type";


    public static HistoryPlotFragment getInstance (String id, String date, int measurementType) {
        HistoryPlotFragment fragment = new HistoryPlotFragment();

        Bundle args = new Bundle();
        args.putString(ARG_ID, id);
        args.putString(ARG_DATE, date);
        args.putInt(ARG_MEASUREMENT_TYPE, measurementType);
        fragment.setArguments(args);

        return fragment;
    }

    public HistoryPlotFragment() {
        // Required empty public constructor
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (getArguments() != null) {
            mId = getArguments().getString(ARG_ID);
            mDate = getArguments().getString(ARG_DATE);
            mMeasurementType = getArguments().getInt(ARG_MEASUREMENT_TYPE);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_history_plot, container, false);

        /**
         * Este Fragment se muestra siempre en landscape para poder ver mejor el plot.
         * En el AndroidManifest se agregó la línea:
         * android:configChanges="keyboardHidden|orientation|screenSize"
         * en HistoryActivity para evitar que el usuario pueda cambiar la orientación.
         */
        getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);



        /**
         * Se configura el plot
         */
        mPlot = (LineChart)v.findViewById(R.id.frag_history_plot_plot);

        /**
         * Si está en false, el eje X y el Y se pueden agrandar y achicar de forma independiente.
         */
        mPlot.setPinchZoom(true);

        /**
         * No se pueden seleccionar los puntos del gráfico
         */
        mPlot.setHighlightPerDragEnabled(false);
        mPlot.setHighlightPerTapEnabled(false);

        /**
         * Se crea una lista con los valores a mostrar en el eje Y.
         * Estos valores se obtienen de la base de datos para el ID, la fecha
         * y el tipo de medición pasados como parámetros al Fragment.
         */
        MeasurementsEntry entry = SmartPlugProvider.getInstance(getActivity()).getMeasurementsEntry(
                mId, mDate, mMeasurementType);

        /**
         * En la columna MEASUREMENTS de entry hay una lista concatenada por ";" de las mediciones
         * Si la fecha que se está mostrando coincide con el día actual, solamente se van a mostrar
         * las mediciones hasta la hora anterior a la actual.
         */
        Calendar currentDate = Calendar.getInstance();
        byte currentDay = (byte) currentDate.get(Calendar.DAY_OF_MONTH);
        byte currentMonth = (byte) (currentDate.get(Calendar.MONTH) + 1);
        byte currentYear = (byte) (currentDate.get(Calendar.YEAR) - 2000);
        String stringCurrentDate = currentDay + "/" + currentMonth + "/" + currentYear;

        String[] measurementsStrings = entry.getMeasurements().split(";");

        int maxEntries;
        if (mDate.compareTo(stringCurrentDate) == 0)
            maxEntries = currentDate.get(Calendar.HOUR_OF_DAY);
        else
            maxEntries = measurementsStrings.length;


        /**
         * De la lista de mediciones se obtiene la suma total (que se va a mostrar en el título si
         * el gráfico es de energía por hora) y el promedio (que se va a mostrar en el título si
         * el gráfico es de potencia activa por hora).
         */
        float sum = 0.0f;

        List<Entry> plotEntries = new ArrayList<>();
        for (int i = 0; i < maxEntries; i++) {
            float measurement = Float.parseFloat(measurementsStrings[i]);

            if (measurement < 0.0f)
                measurement = 0.0f;

            sum += measurement;

            plotEntries.add(new Entry((float)i, measurement));
        }


        LineDataSet dataSet = new LineDataSet(plotEntries, "Mediciones");;
        dataSet.setColor(Color.rgb(0x30, 0x3F, 0x9F));
        dataSet.setCircleColor(Color.rgb(0x30, 0x3F, 0x9F));
        dataSet.setLineWidth(2.0f);
        dataSet.setDrawFilled(true);
        dataSet.setFillColor(Color.rgb(0x02, 0x8b, 0xe7));

        LineData lineData = new LineData(dataSet);

        mPlot.setData(lineData);


        /**
         * Se configuran labels personalizados para el eje X, para poder mostrar la hora.
         */
        final String[] hours = new String[] {"00:00", "01:00", "02:00", "03:00", "04:00", "05:00",
                "06:00", "07:00", "08:00", "09:00", "10:00", "11:00",
                "12:00", "13:00", "14:00", "15:00", "16:00", "17:00",
                "18:00", "19:00", "20:00", "21:00", "22:00", "23:00"};

        mPlot.getXAxis().setValueFormatter(new AxisValueFormatter() {
            @Override
            public String getFormattedValue(float value, AxisBase axis) {
                return hours[(int) value];
            }

            @Override
            public int getDecimalDigits() {
                return 0;
            }
        });


        /**
         * Configuración del eje X.
         */
        mPlot.getXAxis().setGranularity(1.0f);
        mPlot.getXAxis().setPosition(XAxis.XAxisPosition.BOTTOM);
        mPlot.getXAxis().setDrawGridLines(false);


        /**
         * Configuración del eje Y.
         */
        //mPlot.getAxisLeft().setDrawGridLines(false);
        mPlot.getXAxis().enableGridDashedLine(3.0f, 1.0f, 0.0f);
        mPlot.getAxisLeft().enableGridDashedLine(3.0f, 1.0f, 0.0f);
        mPlot.getAxisLeft().setAxisMinValue(0.0f);

        /**
         * Se deshabilita el eje de la derecha.
         */
        mPlot.getAxisRight().setEnabled(false);


        /**
         * Se elimina la descripción del gráfico.
         */
        mPlot.setDescription("");

        /**
         * Se oculta la leyenda del gráfico-
         */
        mPlot.getLegend().setEnabled(false);

        /**
         * Se hace un refresh del gráfico.
         */
        mPlot.invalidate();



        /**
         * Se configura el nombre del gráfico con el tipo de medición y la fecha.
         */
        mPlotNameText = (TextView)v.findViewById(R.id.frag_history_plot_name);

        String title = "";
        if (mMeasurementType == MeasurementsEntry.MeasurementType.ACTIVE_POWER) {
            title = "Potencia (W) vs hora del día - " + mDate + ". Promedio: ";
            if (maxEntries > 0)
                title += String.format("%.1f W", sum/maxEntries);
            else
                title += String.format("%.1f W", 0.0f);
        }
        else if (mMeasurementType == MeasurementsEntry.MeasurementType.ENERGY) {
            title = "Energía (kWh) vs hora del día - " + mDate + ". Total: ";
            title += String.format("%.1f kWh", sum);
        }

        mPlotNameText.setText(title);


        return v;
    }


}
