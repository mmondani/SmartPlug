package com.company.smartplugapp;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.text.DateFormat;
import java.text.SimpleDateFormat;

import events.HeartbeatEvent;


/**
 * Se encarga de mostrar la lista de Smart Plugs encontrados. Los muestra a través
 * de un RecyclerView.
 */
public class SmartPlugListFragment extends Fragment {


    /**
     * No recibe parámetros.
     * @return instancia de SmartPlugListFragment.
     */
    public static SmartPlugListFragment getInstance () {
        SmartPlugListFragment fragment = new SmartPlugListFragment();

        return fragment;
    }

    public SmartPlugListFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_smart_plug_list, container, false);

        Toolbar toolbar = (Toolbar)v.findViewById(R.id.list_fragment_toolbar);
        toolbar.setTitle("Smart Plugs");
        ((AppCompatActivity)getActivity()).setSupportActionBar(toolbar);

        return v;
    }

    @Override
    public void onStart() {
        super.onStart();
        EventBus.getDefault().register(this);
    }

    @Override
    public void onStop() {
        EventBus.getDefault().unregister(this);
        super.onStop();
    }

    @Subscribe (threadMode = ThreadMode.MAIN)
    public void onHeartbeatEvent (HeartbeatEvent ev) {
        Toast.makeText(getActivity(), "Heartbeat" + new SimpleDateFormat("HH:mm:ss").format(ev.getDate()), Toast.LENGTH_SHORT).show();
    }
}
