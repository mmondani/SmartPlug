package com.company.smartplugapp;

import android.content.Context;
import android.content.Intent;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;


public class ConfigurationActivity extends AppCompatActivity implements ConfigurationFragment.OnIconClickedInterface {

    private String mId;

    private static final String EXTRA_ID = "id";


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configuration);

        mId = getIntent().getStringExtra(EXTRA_ID);

        if (savedInstanceState == null) {
            FragmentManager fm = getSupportFragmentManager();
            FragmentTransaction ft = fm.beginTransaction();

            ft.add(R.id.act_configuration_fragment_container, ConfigurationFragment.getInstance(mId));
            ft.commit();
        }

        Toolbar toolbar = (Toolbar)findViewById(R.id.act_configuration_toolbar);
        setSupportActionBar(toolbar);
        getSupportActionBar().setTitle("Configuración");
    }


    @Override
    public void onIconClicked() {
        /** TODO Al fragment encargado de mostrar los íconos disponibles */
    }

    public static Intent newIntent (Context context, String id) {
        Intent i = new Intent (context, ConfigurationActivity.class);

        i.putExtra(EXTRA_ID, id);

        return i;
    }
}
