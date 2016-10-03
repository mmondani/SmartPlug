package com.company.smartplugapp;

import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import org.greenrobot.eventbus.EventBus;

import java.util.Timer;
import java.util.TimerTask;

import events.WiFiStateEvent;
import service.SmartPlugService;

/**
 * MainActivity se encargará de mostrar, primero, el Splash Screen durante 2 segundos.
 * Luego va a mostrar el Fragment con la lista de Smart Plugs encontrados y si se presiona
 * alguno de estos se va a mostrar el Fragment de detalles.
 * Además, MainActivity es la encargada de iniciar el Service que se encarga de comunicarse con
 * los Smart Plugs.
 */
public class MainActivity extends AppCompatActivity implements SmartPlugListFragment.OnItemClickedInterface,
                                                                SmartPlugDetailsFragment.OnFloatingMenuItemClicked {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /**
         * Si se crea por primera vez la aplicación, se muestra la splash screen.
         */
        if (savedInstanceState == null) {
            FragmentManager fm = getSupportFragmentManager();
            FragmentTransaction ft = fm.beginTransaction();

            ft.add(R.id.main_fragment_container, SplashScreenFragment.getInstance());
            ft.commit();


            /**
             * Se programa que, dentro de 2 segundos, se muestre el SmartPlugListFragment.
             */
            Timer timer = new Timer();
            timer.schedule(new TimerTask() {
                @Override
                public void run() {
                    FragmentManager fm = getSupportFragmentManager();
                    FragmentTransaction ft = fm.beginTransaction();

                    ft.setCustomAnimations(android.R.anim.fade_in,
                            android.R.anim.fade_out, android.R.anim.fade_in,
                            android.R.anim.fade_out);
                    ft.replace(R.id.main_fragment_container, SmartPlugListFragment.getInstance());
                    ft.commit();
                }
            }, 2000);

            /**
             * Se inicia el servicio que se encargará de comunicarse con los Smart Plug.
             */
            if (!SmartPlugService.isRunning())
                startService(new Intent(this, SmartPlugService.class));
        }
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
    public void onItemClicked(SmartPlugListItem smartPlugItem) {
        /**
         * Se presionó uno de los Smart Plug de la lista. Se pasa a la vista de detalle de este
         * Smart Plug
         */
        FragmentManager fm = getSupportFragmentManager();
        FragmentTransaction ft = fm.beginTransaction();

        ft.setCustomAnimations(android.R.anim.slide_in_left,
                android.R.anim.slide_out_right, android.R.anim.slide_in_left,
                android.R.anim.slide_out_right);
        ft.replace(R.id.main_fragment_container, SmartPlugDetailsFragment.getInstance(smartPlugItem.getId()));
        ft.addToBackStack("ToDetailView");
        ft.commit();
    }


    @Override
    public void onFloatingMenuConfigClicked(String id) {
        Intent i = ConfigurationActivity.newIntent(this, id);

        startActivity(i);
    }

    @Override
    public void onFloatingMenuHistoryClicked(String id) {
        Intent i = HistoryActivity.newIntent(this, id);

        startActivity(i);
    }
}
