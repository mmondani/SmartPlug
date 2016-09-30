package com.company.smartplugapp;

import android.content.Context;
import android.content.Intent;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;

import database.StaticInfoEntry;

public class HistoryActivity extends AppCompatActivity implements HistorySelectionFragment.OnDateSelectedInterface{

    private String mId;

    private static final String EXTRA_ID = "id";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_history);


        mId = getIntent().getStringExtra(EXTRA_ID);

        if (savedInstanceState == null) {
            FragmentManager fm = getSupportFragmentManager();
            FragmentTransaction ft = fm.beginTransaction();

            ft.add(R.id.act_history_fragment_container, HistorySelectionFragment.getInstance(mId));
            ft.commit();
        }


        /**
         * En la toolbar se muestra el nombre del Smart Plug elegido.
         */
        Toolbar tb = (Toolbar) findViewById(R.id.act_history_toolbar);
        setSupportActionBar(tb);

        StaticInfoEntry entry = SmartPlugProvider.getInstance(this).getStaticInfoEntry(mId);
        getSupportActionBar().setTitle("Historial - " + entry.getName());
    }


    public static Intent newIntent (Context context, String id) {
        Intent i = new Intent(context, HistoryActivity.class);

        i.putExtra(EXTRA_ID, id);

        return i;
    }


    @Override
    public void onDateSelected(String date, int measurementType) {
        FragmentManager fm = getSupportFragmentManager();
        FragmentTransaction ft = fm.beginTransaction();

        ft.setCustomAnimations(android.R.anim.slide_in_left,
                android.R.anim.slide_out_right, android.R.anim.slide_in_left,
                android.R.anim.slide_out_right);
        ft.replace(R.id.act_history_fragment_container, HistoryPlotFragment.getInstance(mId, date, measurementType));
        ft.addToBackStack("ToPlotView");
        ft.commit();
    }
}