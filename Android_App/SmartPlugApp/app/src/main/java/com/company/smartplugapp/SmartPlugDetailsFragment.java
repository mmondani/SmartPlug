package com.company.smartplugapp;


import android.app.Activity;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.github.clans.fab.FloatingActionButton;
import com.github.clans.fab.FloatingActionMenu;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import database.InstantaneousInfoEntry;
import database.StaticInfoEntry;
import events.UpdateSmartPlugEvent;


public class SmartPlugDetailsFragment extends Fragment {

    private ImageView mIconView;
    private TextView mNameText;
    private TextView mVoltageText;
    private TextView mCurrentText;
    private TextView mPowerText;
    private TextView mTotalEnergyText;
    private ImageView mCommStateIconView;
    private ImageView mScheduleIconView;
    private TextView mCommStateText;
    private TextView mScheduleText;
    private FloatingActionMenu mFloatingMenu;
    private FloatingActionButton mFloatingItemConfig;
    private FloatingActionButton mFloatingItemHistory;
    private FloatingActionButton mFloatingItemEnergyReset;
    private FloatingActionButton mFloatingItemFactoryReset;

    private OnFloatingMenuItemClicked mClickListener = null;

    private String mId;


    private static final String ARG_ID = "id";


    public static SmartPlugDetailsFragment getInstance (String id) {
        SmartPlugDetailsFragment fragment = new SmartPlugDetailsFragment();

        Bundle args = new Bundle();
        args.putString(ARG_ID, id);

        fragment.setArguments(args);

        return fragment;
    }

    public SmartPlugDetailsFragment() {
        // Required empty public constructor
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if(getArguments() != null) {
            mId = getArguments().getString(ARG_ID);
        }
    }

    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        try {
            mClickListener = (OnFloatingMenuItemClicked)activity;
        }
        catch (ClassCastException cce) {
            throw new ClassCastException(activity.toString() + " debe implementar OnFloatingMenuItemClicked");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mClickListener = null;
    }

    @Override
    public void onStart() {
        super.onStart();
        /**
         * Se registra este framento en el EventBus para empezar a recibir los eventos
         * a los que esté suscrito.
         */
        EventBus.getDefault().register(this);
    }

    @Override
    public void onStop() {
        /**
         * Se des-registra del EventBus para dejar de recibir los eventos.
         */
        EventBus.getDefault().unregister(this);
        super.onStop();
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_smart_plug_details, container, false);

        mIconView = (ImageView)v.findViewById(R.id.frag_details_icon);
        mNameText = (TextView)v.findViewById(R.id.frag_details_icon_text);
        mVoltageText = (TextView)v.findViewById(R.id.frag_details_voltage);
        mCurrentText = (TextView)v.findViewById(R.id.frag_details_current);
        mPowerText = (TextView)v.findViewById(R.id.frag_details_power);
        mTotalEnergyText = (TextView)v.findViewById(R.id.frag_details_total_energy);
        mCommStateIconView = (ImageView)v.findViewById(R.id.frag_details_comm_state_icon);
        mScheduleIconView = (ImageView)v.findViewById(R.id.frag_details_schedule_icon);
        mCommStateText = (TextView)v.findViewById(R.id.frag_details_comm_state_text);
        mScheduleText = (TextView)v.findViewById(R.id.frag_details_schedule_text);
        mFloatingMenu = (FloatingActionMenu)v.findViewById(R.id.frag_details_float_menu);
        mFloatingItemConfig = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_configuration);
        mFloatingItemHistory = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_history);
        mFloatingItemEnergyReset = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_reset_energy);
        mFloatingItemFactoryReset = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_factory_reset);


        Toolbar toolbar = (Toolbar)v.findViewById(R.id.frag_details_toolbar);
        toolbar.setTitle("Smart Plugs");
        ((AppCompatActivity)getActivity()).setSupportActionBar(toolbar);
        ((AppCompatActivity)getActivity()).getSupportActionBar().setTitle("");

        /**
         * Se actualiza la UI
         */
        updateUI();


        mFloatingItemConfig.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onFlatingMenuConfigClicked();
            }
        });

        mFloatingItemHistory.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onFlatingMenuHistoryClicked();
            }
        });

        mFloatingItemEnergyReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onFlatingMenuEnergyResetClicked();
            }
        });

        mFloatingItemFactoryReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onFlatingMenuFactoryResetClicked();
            }
        });


        return v;
    }


    /**
     * Se suscribe al evento UpdateSmartPlugEvent. Si el ID del Smart Plug que se actualizó
     * coincide con el que está mostrando la vista de detalle, se actualiza la UI.
     * @param ev Instancia del evento UpdateSmartPlugEvent.
     */
    @Subscribe (threadMode = ThreadMode.MAIN)
    public void onUpdateSmartPlugEvent (UpdateSmartPlugEvent ev) {
        if(ev.getId().compareTo(mId) == 0) {
            updateUI();
        }
    }


    public interface OnFloatingMenuItemClicked {
        void onFlatingMenuConfigClicked ();
        void onFlatingMenuHistoryClicked ();
        void onFlatingMenuEnergyResetClicked ();
        void onFlatingMenuFactoryResetClicked ();
    }

    private void updateUI () {
        /**
         * A partir del ID pasado como parámetro al Fragment se lee la base de datos para obtener
         * los datos del SmartPlug
         */
        SmartPlugProvider provider = SmartPlugProvider.getInstance(getActivity());

        InstantaneousInfoEntry instantaneousInfoEntry = provider.getInstantaneousInfoEntry(mId);
        StaticInfoEntry staticInfoEntry = provider.getStaticInfoEntry(mId);
        /** TODO Falta obtener la entrada de la tabla OnOffTimes*/

        /**
         * Se actualiza la UI a partir de la información en la base de datos.
         */
        mIconView.setImageResource(staticInfoEntry.getIconId());
        mNameText.setText(staticInfoEntry.getName());
        mVoltageText.setText(String.format("%.1f", instantaneousInfoEntry.getVoltage()) + " V");
        mCurrentText.setText(String.format("%.1f", instantaneousInfoEntry.getCurrent()) + " A");
        mPowerText.setText(String.format("%.1f", instantaneousInfoEntry.getPower()) + " W");
        mTotalEnergyText.setText(String.format("%.2f", instantaneousInfoEntry.getTotalEnergy()) + "KWh");

        if (instantaneousInfoEntry.getConnectionState() == 0) {
            mCommStateIconView.setImageResource(R.drawable.ok);
            mCommStateText.setText("Funciona correctamente");
        }
        else {
            mCommStateIconView.setImageResource(R.drawable.error);
            mCommStateText.setText("Error de comunicación");
        }

        /** TODO Falta determinar si está habilitada la programación horaria o no */

    }
}
