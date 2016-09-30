package com.company.smartplugapp;


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

    private static final String ARG_ID = "id";


    public static HistorySelectionFragment getInstace (String id) {
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
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_history_selection, container, false);

        mPowerDateSpinner = (Spinner)v.findViewById(R.id.frag_history_selection_spin_power);
        mEnergyDateSpinner = (Spinner)v.findViewById(R.id.frag_history_selection_spin_energy);

        /**
         * Los spinner se llenan a partir de las fechas de las entradas en la tabla Measurement para
         * el ID mId.
         */
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
                    Toast.makeText(getActivity(), mPowerSpinnerElements.get(position), Toast.LENGTH_SHORT).show();
                    /** TODO Remover el Toast y llamar al Fragment que muestre el plot */
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
                    Toast.makeText(getActivity(), mEnergySpinnerElements.get(position), Toast.LENGTH_SHORT).show();
                    /** TODO Remover el Toast y llamar al Fragment que muestre el plot */
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        //getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        return v;
    }


}
