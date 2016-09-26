package com.company.smartplugapp;


import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.View;
import android.view.ViewGroup;


/**
 * Se encarga únicamente de mostrar el logo y el nombre de la aplicación.
 */
public class SplashScreenFragment extends Fragment {


    /**
     * No recibe parámetros
     * @return instancia de SplashScreenFragment.
     */
    public static SplashScreenFragment getInstance () {
        SplashScreenFragment fragment = new SplashScreenFragment();

        return fragment;
    }

    public SplashScreenFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_splash_screen, container, false);

        return v;
    }

}
