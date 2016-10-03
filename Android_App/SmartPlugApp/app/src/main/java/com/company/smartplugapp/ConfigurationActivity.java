package com.company.smartplugapp;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.Toolbar;

import org.greenrobot.eventbus.EventBus;

import database.StaticInfoEntry;
import events.WiFiStateEvent;


public class ConfigurationActivity extends AppCompatActivity implements ConfigurationFragment.OnIconClickedInterface,
                                                                        IconPickerFragment.OnIconItemClickedInterface {

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
    protected void onResume() {
        super.onResume();

        /**
         * Se determina si está conectado a una red WiFi o no.
         * Se deben agregar en el AndroidManifest los permisos: android.permission.ACCESS_WIFI_STATE y
         * android.permission.ACCESS_NETWORK_STATE
         * TODO: implementar la lógica con el SSID de la red para saber si está en la red de los Smart Plugs o en otro.
         */
        ConnectivityManager cm = (ConnectivityManager)getApplicationContext().getSystemService(CONNECTIVITY_SERVICE);
        NetworkInfo ni = cm.getActiveNetworkInfo();

        if (ni != null && ni.getType() == ConnectivityManager.TYPE_WIFI)
            EventBus.getDefault().post(new WiFiStateEvent(true));
        else
            EventBus.getDefault().post(new WiFiStateEvent(false));
    }

    @Override
    public void onIconClicked() {
        FragmentManager fm = getSupportFragmentManager();
        FragmentTransaction ft = fm.beginTransaction();

        ft.setCustomAnimations(android.R.anim.slide_in_left,
                android.R.anim.slide_out_right, android.R.anim.slide_in_left,
                android.R.anim.slide_out_right);
        ft.replace(R.id.act_configuration_fragment_container, new IconPickerFragment());
        ft.addToBackStack("iconGrid");
        ft.commit();
    }

    @Override
    public void onIconItemClicked(int resId) {
        /**
         * Se actualiza el ícono elegido en la entrada correspondiente de StaticInfoTable
         */
        StaticInfoEntry entry = SmartPlugProvider.getInstance(this).getStaticInfoEntry(mId);
        entry.setIconId(resId);
        SmartPlugProvider.getInstance(this).updateStaticInfoEntry(entry);

        /**
         * Se vuelve a mostrar el fragment de configuración
         */
        FragmentManager fm = getSupportFragmentManager();

        fm.popBackStack();
    }

    public static Intent newIntent (Context context, String id) {
        Intent i = new Intent (context, ConfigurationActivity.class);

        i.putExtra(EXTRA_ID, id);

        return i;
    }
}
