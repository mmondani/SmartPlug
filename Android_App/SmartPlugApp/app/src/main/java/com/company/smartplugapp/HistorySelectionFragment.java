package com.company.smartplugapp;


import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

import database.MeasurementsEntry;


/**
 * A simple {@link Fragment} subclass.
 */
public class HistorySelectionFragment extends Fragment {

    private Spinner mPowerDateSpinner;
    private Spinner mEnergyDateSpinner;
    List<String> mPowerSpinnerElements;
    List<String> mEnergySpinnerElements;

    private String mId;
    private OnDateSelectedInterface mSelectedListener;

    private static final String ARG_ID = "id";


    public static HistorySelectionFragment getInstance (String id) {
        HistorySelectionFragment fragment = new HistorySelectionFragment();

        Bundle args = new Bundle();
        args.putString(ARG_ID, id);
        fragment.setArguments(args);

        return fragment;
    }

    public HistorySelectionFragment() {
        // Required empty public constructor
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (getArguments() != null) {
            mId = getArguments().getString(ARG_ID);
        }
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        try {
            mSelectedListener = (OnDateSelectedInterface)activity;
        } catch (ClassCastException cce) {
            throw new ClassCastException(activity.toString() + " debe implementar OnDateSelectedInterface");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();

        mSelectedListener = null;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_history_selection, container, false);

        /**
         * Este Fragment se muestra siempre en portrait.
         * En el AndroidManifest se agregó la línea:
         * android:configChanges="keyboardHidden|orientation|screenSize"
         * en HistoryActivity para evitar que el usuario pueda cambiar la orientación.
         */
        getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);



        /**
         * Los spinner se llenan a partir de las fechas de las entradas en la tabla Measurement para
         * el ID mId.
         */
        mPowerDateSpinner = (Spinner)v.findViewById(R.id.frag_history_selection_spin_power);
        mEnergyDateSpinner = (Spinner)v.findViewById(R.id.frag_history_selection_spin_energy);



        List<MeasurementsEntry> powerMeasurementsEntries = SmartPlugProvider.getInstance(getActivity()).
                getMeasurementsEntries(mId, MeasurementsEntry.MeasurementType.ACTIVE_POWER);

        List<MeasurementsEntry> energyMeasurementsEntries = SmartPlugProvider.getInstance(getActivity()).
                getMeasurementsEntries(mId, MeasurementsEntry.MeasurementType.ENERGY);

        mPowerSpinnerElements = new ArrayList<>();
        mEnergySpinnerElements = new ArrayList<>();


        if (powerMeasurementsEntries != null && energyMeasurementsEntries != null) {
            mPowerSpinnerElements.add("Elija una fecha");
            for (MeasurementsEntry entry : powerMeasurementsEntries)
                mPowerSpinnerElements.add(entry.getDate());

            mEnergySpinnerElements.add("Elija una fecha");
            for (MeasurementsEntry entry : energyMeasurementsEntries)
                mEnergySpinnerElements.add(entry.getDate());
        }


        ArrayAdapter<String> powerSpinnerAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, mPowerSpinnerElements);
        ArrayAdapter<String> energySpinnerAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, mEnergySpinnerElements);

        powerSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        energySpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);


        mPowerDateSpinner.setAdapter(powerSpinnerAdapter);
        mEnergyDateSpinner.setAdapter(energySpinnerAdapter);

        mPowerDateSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                /**
                 * Este método también se llama cuando el Spinner es creado (position = 0). De esta forma
                 * se evita llamar al Fragment del plot cuando se crea el Spinner.
                 */
                if (position != 0) {
                    if (mSelectedListener != null) {
                        /**
                         * Se vuelve a seleccionar el elemento 0 de la lista ("Elija una fecha"),
                         * para que cuando el usuario presione el botón de Back y se vuelva a crear
                         * la vista de este Fragment, se muestren los Spinner correctamente y no
                         * con la última fecha elegida.
                         */
                        mPowerDateSpinner.setSelection(0);

                        mSelectedListener.onDateSelected(mPowerSpinnerElements.get(position),
                                MeasurementsEntry.MeasurementType.ACTIVE_POWER);
                    }
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        mEnergyDateSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                /**
                 * Este método también se llama cuando el Spinner es creado (position = 0). De esta forma
                 * se evita llamar al Fragment del plot cuando se crea el Spinner.
                 */
                if (position != 0) {
                    if (mSelectedListener != null) {
                        /**
                         * Se vuelve a seleccionar el elemento 0 de la lista ("Elija una fecha"),
                         * para que cuando el usuario presione el botón de Back y se vuelva a crear
                         * la vista de este Fragment, se muestren los Spinner correctamente y no
                         * con la última fecha elegida.
                         */
                        mEnergyDateSpinner.setSelection(0);

                        mSelectedListener.onDateSelected(mEnergySpinnerElements.get(position),
                                MeasurementsEntry.MeasurementType.ENERGY);
                    }
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        return v;
    }


    public interface OnDateSelectedInterface {
        void onDateSelected (String date, int measurementType);
    }
}
