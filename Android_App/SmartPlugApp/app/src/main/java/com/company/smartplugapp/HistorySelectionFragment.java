package com.company.smartplugapp;


import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import java.util.ArrayList;
import java.util.List;

import database.MeasurementsEntry;


/**
 * A simple {@link Fragment} subclass.
 */
public class HistorySelectionFragment extends Fragment {

    private Spinner mPowerDateSpinner;
    private Spinner mEnergyDateSpinner;

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

        List<String> powerSpinnerElements = new ArrayList<>();
        List<String> energySpinnerElements = new ArrayList<>();


        if (powerMeasurementsEntries != null && energyMeasurementsEntries != null) {
            for (MeasurementsEntry entry : powerMeasurementsEntries)
                powerSpinnerElements.add(entry.getDate());

            for (MeasurementsEntry entry : energyMeasurementsEntries)
                energySpinnerElements.add(entry.getDate());
        }


        ArrayAdapter<String> powerSpinnerAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, powerSpinnerElements);
        ArrayAdapter<String> energySpinnerAdapter = new ArrayAdapter<String>(getActivity(), android.R.layout.simple_spinner_item, energySpinnerElements);

        powerSpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        energySpinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);


        mPowerDateSpinner.setAdapter(powerSpinnerAdapter);
        mEnergyDateSpinner.setAdapter(energySpinnerAdapter);

        //getActivity().setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        return v;
    }


}
