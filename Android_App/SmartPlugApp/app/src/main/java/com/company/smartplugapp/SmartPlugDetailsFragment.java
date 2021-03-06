package com.company.smartplugapp;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
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
import database.OnOffTimesEntry;
import database.SmartPlugDb;
import database.StaticInfoEntry;
import events.CommandEvent;
import events.UpdateSmartPlugEvent;
import service.SmartPlugService;
import smartPlugComm.SmartPlugCommHelper;


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
    private FloatingActionButton mFloatingItemRefresh;
    private FloatingActionButton mFloatingItemMeasurementsReset;
    private FloatingActionButton mFloatingItemFactoryReset;

    private OnFloatingMenuItemClicked mClickListener = null;

    private String mId;


    private static final String ARG_ID = "id";
    private static final int REQ_REFRESH = 1;
    private static final int REQ_RESET_MEASUREMENTS = 2;
    private static final int REQ_FACTORY_RESET = 3;



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
         * Cada vez que se muestra el Fragment se actualiza la información del Smart Plug.
         */
        updateUI();

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
        mFloatingItemRefresh = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_refresh);
        mFloatingItemMeasurementsReset = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_reset_measurements);
        mFloatingItemFactoryReset = (FloatingActionButton)v.findViewById(R.id.frag_details_float_menu_factory_reset);


        Toolbar toolbar = (Toolbar)v.findViewById(R.id.frag_details_toolbar);
        ((AppCompatActivity)getActivity()).setSupportActionBar(toolbar);
        ((AppCompatActivity)getActivity()).getSupportActionBar().setTitle("");


        mFloatingItemConfig.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onFloatingMenuConfigClicked(mId);
            }
        });

        mFloatingItemHistory.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onFloatingMenuHistoryClicked(mId);
            }
        });

        mFloatingItemRefresh.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FragmentManager fm = getActivity().getSupportFragmentManager();

                AskingDialog ad = AskingDialog.getInstance("Actualizar parámetros",
                        "Está seguro que desea actualizar la información del Smart Plug en la aplicación.");

                ad.setTargetFragment(SmartPlugDetailsFragment.this, REQ_REFRESH);
                ad.show(fm, "RefreshDialog");
            }
        });

        mFloatingItemMeasurementsReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FragmentManager fm = getActivity().getSupportFragmentManager();

                AskingDialog ad = AskingDialog.getInstance("Reiniciar mediciones",
                        "Está seguro que desea reiniciar el registro de mediciones de energía y potencia " +
                                "en el Smart Plug.");

                ad.setTargetFragment(SmartPlugDetailsFragment.this, REQ_RESET_MEASUREMENTS);
                ad.show(fm, "MeasurementResethDialog");
            }
        });

        mFloatingItemFactoryReset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                FragmentManager fm = getActivity().getSupportFragmentManager();

                AskingDialog ad = AskingDialog.getInstance("Volver a valores de fábrica",
                        "Está seguro que desea volver a los valores de fábrica. Perderá " +
                                "toda la información histórica.");

                ad.setTargetFragment(SmartPlugDetailsFragment.this, REQ_FACTORY_RESET);
                ad.show(fm, "FactoryResetDialog");
            }
        });


        return v;
    }


    /**
     * Va a ser llamada cuando los AskingDialogs retornen si se presionó SI o NO.
     */
    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode != Activity.RESULT_OK)
            return;

        if (requestCode == REQ_REFRESH) {
            /**
             * Se presionó que SI se quiere hacer un refresh de las mediciones del Smart Plug.
             * Se va a solicitar: nombre del dispositivo, estado de la carga, mediciones instantaneas,
             * horarios de encendido y apagado, mediciones de los últimos 7 días.
             */
            SmartPlugService.queryInitialValues(mId);

            SmartPlugService.queryWeekMeasurements(mId);
        }
        else if (requestCode == REQ_RESET_MEASUREMENTS) {
            /**
             * Se presionó que SI se quieren reiniciar las mediciones. Se van a reiniciar: la energía
             * total acumulada, las mediciones históricas de potencia, las mediciones históricas
             * de energía.
             */
            EventBus.getDefault().post(new CommandEvent(mId,
                    SmartPlugCommHelper.getInstance().getRawData(
                            SmartPlugCommHelper.Commands.RESET,
                            SmartPlugCommHelper.Registers.TOTAL_ENERGY
                    )));

            EventBus.getDefault().post(new CommandEvent(mId,
                    SmartPlugCommHelper.getInstance().getRawData(
                            SmartPlugCommHelper.Commands.RESET,
                            SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER
                    )));

            EventBus.getDefault().post(new CommandEvent(mId,
                    SmartPlugCommHelper.getInstance().getRawData(
                            SmartPlugCommHelper.Commands.RESET,
                            SmartPlugCommHelper.Registers.PER_HOUR_ENERGY
                    )));
        }
        else if (requestCode == REQ_FACTORY_RESET) {
            /**
             * Se presionó que SI se quiere volver a valores de fábrica. Se va a mandar un comando
             * RESET (ALL_REGISTERS) y se van a volver a pedir: nombre del dispositivo, estado de la carga,
             * mediciones instantaneas, horarios de encendido y apagado
             */
            EventBus.getDefault().post(new CommandEvent(mId,
                    SmartPlugCommHelper.getInstance().getRawData(
                            SmartPlugCommHelper.Commands.RESET,
                            SmartPlugCommHelper.Registers.ALL_REGISTERS
                    )));
        }
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
        void onFloatingMenuConfigClicked (String id);
        void onFloatingMenuHistoryClicked (String id);
    }

    private void updateUI () {
        /**
         * A partir del ID pasado como parámetro al Fragment se lee la base de datos para obtener
         * los datos del SmartPlug
         */
        SmartPlugProvider provider = SmartPlugProvider.getInstance(getActivity());

        InstantaneousInfoEntry instantaneousInfoEntry = provider.getInstantaneousInfoEntry(mId);
        StaticInfoEntry staticInfoEntry = provider.getStaticInfoEntry(mId);
        OnOffTimesEntry onOffTimesEntry = provider.getOnOffTimesEntry(mId);

        /**
         * Se actualiza la UI a partir de la información en la base de datos.
         */
        mIconView.setImageResource(staticInfoEntry.getIconId());
        mNameText.setText(staticInfoEntry.getName());
        mVoltageText.setText(String.format("%.1f", instantaneousInfoEntry.getVoltage()) + " V");
        mCurrentText.setText(String.format("%.1f", instantaneousInfoEntry.getCurrent()) + " A");
        mPowerText.setText(String.format("%.1f", instantaneousInfoEntry.getPower()) + " W");
        mTotalEnergyText.setText(String.format("%.1f", instantaneousInfoEntry.getTotalEnergy()) + " kWh");

        if (instantaneousInfoEntry.getConnectionState() == 0) {
            mCommStateIconView.setImageResource(R.drawable.ok);
            mCommStateText.setText("Funciona correctamente");
        }
        else {
            mCommStateIconView.setImageResource(R.drawable.error);
            mCommStateText.setText("Error de comunicación");
        }

        /**
         * Si todos los días están deshabilitados, no hay programción horaria
         */
        if (onOffTimesEntry.getEnabledTimes() == 0) {
            mScheduleIconView.setImageResource(R.drawable.no_clock);
            mScheduleText.setText("Programación horaria desactivada");
        }
        else {
            mScheduleIconView.setImageResource(R.drawable.clock);
            mScheduleText.setText("Programación horaria activada");
        }


    }
}
