package com.company.smartplugapp;


import android.app.Activity;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import database.InstantaneousInfoEntry;
import database.OnOffTimesEntry;
import database.StaticInfoEntry;
import events.UpdateSmartPlugEvent;


public class ConfigurationFragment extends Fragment {

    private EditText mNameEdit;
    private ImageView mSendNameButton;
    private ImageView mIconView;
    private TextView mScheduleMondayText;
    private TextView mScheduleTuesdayText;
    private TextView mScheduleWednesdayText;
    private TextView mScheduleThursdayText;
    private TextView mScheduleFridayText;
    private TextView mScheduleSaturdayText;
    private TextView mScheduleSundayText;
    private LinearLayout mScheduleMondayBox;
    private LinearLayout mScheduleTuesdayBox;
    private LinearLayout mScheduleWednesdayBox;
    private LinearLayout mScheduleThursdayBox;
    private LinearLayout mScheduleFridayBox;
    private LinearLayout mScheduleSaturdayBox;
    private LinearLayout mScheduleSundayBox;

    private String mId;
    private OnIconClickedInterface mClickListener = null;

    private static final String ARG_ID = "id";


    public static ConfigurationFragment getInstance (String id) {
        ConfigurationFragment fragment = new ConfigurationFragment();

        Bundle args = new Bundle();
        args.putString(ARG_ID, id);

        fragment.setArguments(args);

        return fragment;
    }

    public ConfigurationFragment() {
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
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        try {
            mClickListener = (OnIconClickedInterface)activity;
        }
        catch (ClassCastException cce) {
            throw new ClassCastException(activity.toString() + " debe implementar OnIconClickedInterface");
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
    public void onResume() {
        super.onResume();

        updateUI();
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
        View v = inflater.inflate(R.layout.fragment_configuration, container, false);

        mNameEdit = (EditText)v.findViewById(R.id.frag_configuration_name);
        mSendNameButton = (ImageView)v.findViewById(R.id.frag_configuration_send_name);
        mIconView = (ImageView)v.findViewById(R.id.frag_configuration_icon);
        mScheduleMondayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_monday);
        mScheduleTuesdayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_tuesday);
        mScheduleWednesdayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_wednesday);
        mScheduleThursdayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_thursday);
        mScheduleFridayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_friday);
        mScheduleSaturdayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_saturday);
        mScheduleSundayText = (TextView)v.findViewById(R.id.frag_configuration_schedule_sunday);
        mScheduleMondayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_monday_box);
        mScheduleTuesdayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_tuesday_box);
        mScheduleWednesdayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_wednesday_box);
        mScheduleThursdayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_thursday_box);
        mScheduleFridayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_friday_box);
        mScheduleSaturdayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_saturday_box);
        mScheduleSundayBox = (LinearLayout)v.findViewById(R.id.frag_configuration_sunday_box);


        mNameEdit.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {

            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {

            }

            @Override
            public void afterTextChanged(Editable s) {

            }
        });


        /**
         * Cuando se presiona la flecha al lado del EditText, se debe enviar el nuevo nombre al Smart Plug.
         */
        mSendNameButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /** TODO Enviar el mensaje con SET(DEVICE_ID) */
            }
        });


        /**
         * Cuando se presiona el ícono se le informa a la actividad que tiene el Fragment
         * para que cambiar el Fragment al de la lista de íconos.
         */
        mIconView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mClickListener != null)
                    mClickListener.onIconClicked();
            }
        });


        /**
         * Cuando se presiona alguno de los días de la semana, se debe llamar al DialogFragment
         * que muestra un time picker.
         */
        mScheduleMondayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                /** TODO Llamar al DialogFragment para configurar las horas */
            }
        });

        mScheduleTuesdayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        mScheduleWednesdayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        mScheduleThursdayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        mScheduleFridayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        mScheduleSaturdayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        mScheduleSundayBox.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        return v;
    }


    public interface OnIconClickedInterface {
        void onIconClicked ();
    }


    /**
     * Se suscribe al evento UpdateSmartPlugEvent. Si el ID del Smart Plug que se actualizó
     * coincide con el que está mostrando la vista de detalle, se actualiza la UI.
     * @param ev Instancia del evento UpdateSmartPlugEvent.
     */
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onUpdateSmartPlugEvent (UpdateSmartPlugEvent ev) {
        if(ev.getId().compareTo(mId) == 0) {
            updateUI();
        }
    }


    private void updateUI () {
        /**
         * A partir del ID pasado como parámetro al Fragment se lee la base de datos para obtener
         * los datos del SmartPlug
         */
        SmartPlugProvider provider = SmartPlugProvider.getInstance(getActivity());

        StaticInfoEntry staticInfoEntry = provider.getStaticInfoEntry(mId);
        OnOffTimesEntry onOffTimesEntry = provider.getOnOffTimesEntry(mId);

        /**
         * Se actualiza la UI a partir de la información en la base de datos.
         */
        mNameEdit.setText(staticInfoEntry.getName());
        mIconView.setImageResource(staticInfoEntry.getIconId());

        /**
         * Si la programación horaria de un día está deshabilitada, se muestra el día con un menor
         * valor de alpha.
         */
        if( (onOffTimesEntry.getEnabledTimes() & 0x01) == 0 )
            mScheduleSundayBox.setAlpha(0.3f);
        if( (onOffTimesEntry.getEnabledTimes() & 0x02) == 0 )
            mScheduleMondayBox.setAlpha(0.3f);
        if( (onOffTimesEntry.getEnabledTimes() & 0x04) == 0 )
            mScheduleTuesdayBox.setAlpha(0.3f);
        if( (onOffTimesEntry.getEnabledTimes() & 0x08) == 0 )
            mScheduleWednesdayBox.setAlpha(0.3f);
        if( (onOffTimesEntry.getEnabledTimes() & 0x10) == 0 )
            mScheduleThursdayBox.setAlpha(0.3f);
        if( (onOffTimesEntry.getEnabledTimes() & 0x20) == 0 )
            mScheduleFridayBox.setAlpha(0.3f);
        if( (onOffTimesEntry.getEnabledTimes() & 0x40) == 0 )
            mScheduleSaturdayBox.setAlpha(0.3f);

        /**
         * Se cargan las horas de encendido y apagado esté o no habilitado el día.
         */
        mScheduleMondayText.setText(onOffTimesEntry.getMondayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getMondayLoadOffTime().toHourMinutes());
        mScheduleTuesdayText.setText(onOffTimesEntry.getTuesdayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getTuesdayLoadOffTime().toHourMinutes());
        mScheduleWednesdayText.setText(onOffTimesEntry.getWednesdayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getWednesdayLoadOffTime().toHourMinutes());
        mScheduleThursdayText.setText(onOffTimesEntry.getThursdayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getThursdayLoadOffTime().toHourMinutes());
        mScheduleFridayText.setText(onOffTimesEntry.getFridayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getFridayLoadOffTime().toHourMinutes());
        mScheduleSaturdayText.setText(onOffTimesEntry.getSaturdayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getSaturdayLoadOffTime().toHourMinutes());
        mScheduleSundayText.setText(onOffTimesEntry.getSundayLoadOnTime().toHourMinutes() + " - " +
                onOffTimesEntry.getSundayLoadOffTime().toHourMinutes());
    }
}
