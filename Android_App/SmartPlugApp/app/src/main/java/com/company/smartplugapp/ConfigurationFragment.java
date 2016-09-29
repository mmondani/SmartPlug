package com.company.smartplugapp;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
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
import database.SimpleTime;
import database.StaticInfoEntry;
import events.CommandEvent;
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
    private String mWeekDayToConfig;        /** TODO guardarlo en el bundle para que sobreviva a los cambios de configuración */

    private static final String SAVE_BUNDLE_WEEK_DAY = "week_day";
    private static final String ARG_ID = "id";
    private static final int REQUEST_ON_TIME = 0;
    private static final int REQUEST_OFF_TIME = 1;


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

        if (savedInstanceState != null) {
            mWeekDayToConfig = savedInstanceState.getString(SAVE_BUNDLE_WEEK_DAY);
        }

        if (getArguments() != null) {
            mId = getArguments().getString(ARG_ID);
        }
    }


    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);

        outState.putString(SAVE_BUNDLE_WEEK_DAY, mWeekDayToConfig);
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

                /**
                 * Al hacer click se muestran los DialogFragments para elegir las horas
                 * de encendido y apagado.
                 */
                String timeString = mScheduleMondayText.getText().toString().split(" - ")[0];
                showTimePickerDialog("monday", timeString, true);
            }
        });

        mScheduleMondayBox.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View v) {
                resetScheduleTime ("monday");

                return true;
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


    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == REQUEST_ON_TIME) {
            /**
             * Se muestra el DialogFragment para la hora de apagado se haya aceptado la hora de encendido
             * o se la haya pasado.
             */
            String timeString = mScheduleMondayText.getText().toString().split(" - ")[1];
            if (mWeekDayToConfig.compareTo("monday") == 0)
                timeString = mScheduleMondayText.getText().toString().split(" - ")[1];
            else if (mWeekDayToConfig.compareTo("tuesday") == 0)
                timeString = mScheduleTuesdayText.getText().toString().split(" - ")[1];
            else if (mWeekDayToConfig.compareTo("wednesday") == 0)
                timeString = mScheduleWednesdayText.getText().toString().split(" - ")[1];
            else if (mWeekDayToConfig.compareTo("thursday") == 0)
                timeString = mScheduleThursdayText.getText().toString().split(" - ")[1];
            else if (mWeekDayToConfig.compareTo("friday") == 0)
                timeString = mScheduleFridayText.getText().toString().split(" - ")[1];
            else if (mWeekDayToConfig.compareTo("saturday") == 0)
                timeString = mScheduleSaturdayText.getText().toString().split(" - ")[1];
            else if (mWeekDayToConfig.compareTo("sunday") == 0)
                timeString = mScheduleSundayText.getText().toString().split(" - ")[1];


            showTimePickerDialog(mWeekDayToConfig, timeString, false);
        }




        if ( (resultCode == Activity.RESULT_OK) && (requestCode == REQUEST_ON_TIME) ) {
            /**
             * Se envía el comando SET(*_LOAD_ON_TIME) al Smart Plug
             */
            byte register = SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME;

            if (mWeekDayToConfig.compareTo("monday") == 0)
                register = SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME;
            else if (mWeekDayToConfig.compareTo("tuesday") == 0)
                register = SmartPlugCommHelper.Registers.TUESDAY_LOAD_ON_TIME;
            else if (mWeekDayToConfig.compareTo("wednesday") == 0)
                register = SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_ON_TIME;
            else if (mWeekDayToConfig.compareTo("thursday") == 0)
                register = SmartPlugCommHelper.Registers.THURSDAY_LOAD_ON_TIME;
            else if (mWeekDayToConfig.compareTo("friday") == 0)
                register = SmartPlugCommHelper.Registers.FRIDAY_LOAD_ON_TIME;
            else if (mWeekDayToConfig.compareTo("saturday") == 0)
                register = SmartPlugCommHelper.Registers.SATURDAY_LOAD_ON_TIME;
            else if (mWeekDayToConfig.compareTo("sunday") == 0)
                register = SmartPlugCommHelper.Registers.SUNDAY_LOAD_ON_TIME;


            SimpleTime onTime = TimePickerDialog.getIntentTime(data);

            EventBus.getDefault().post(new CommandEvent(mId, SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.SET,
                                                                                                            register,
                                                                                                            new byte[]{onTime.getHours(), onTime.getMinutes()})));

        }
        else if ((resultCode == Activity.RESULT_OK) && (requestCode == REQUEST_OFF_TIME) ) {
            /**
             * Se envía el comando SET(*_LOAD_OFF_TIME) al Smart Plug
             */
            byte register = SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME;

            if (mWeekDayToConfig.compareTo("monday") == 0)
                register = SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME;
            else if (mWeekDayToConfig.compareTo("tuesday") == 0)
                register = SmartPlugCommHelper.Registers.TUESDAY_LOAD_OFF_TIME;
            else if (mWeekDayToConfig.compareTo("wednesday") == 0)
                register = SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_OFF_TIME;
            else if (mWeekDayToConfig.compareTo("thursday") == 0)
                register = SmartPlugCommHelper.Registers.THURSDAY_LOAD_OFF_TIME;
            else if (mWeekDayToConfig.compareTo("friday") == 0)
                register = SmartPlugCommHelper.Registers.FRIDAY_LOAD_OFF_TIME;
            else if (mWeekDayToConfig.compareTo("saturday") == 0)
                register = SmartPlugCommHelper.Registers.SATURDAY_LOAD_OFF_TIME;
            else if (mWeekDayToConfig.compareTo("sunday") == 0)
                register = SmartPlugCommHelper.Registers.SUNDAY_LOAD_OFF_TIME;

            SimpleTime offTime = TimePickerDialog.getIntentTime(data);

            EventBus.getDefault().post(new CommandEvent(mId, SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.SET,
                    register,
                    new byte[]{offTime.getHours(), offTime.getMinutes()})));
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
        else
            mScheduleSundayBox.setAlpha(1.0f);

        if( (onOffTimesEntry.getEnabledTimes() & 0x02) == 0 )
            mScheduleMondayBox.setAlpha(0.3f);
        else
            mScheduleMondayBox.setAlpha(1.0f);

        if( (onOffTimesEntry.getEnabledTimes() & 0x04) == 0 )
            mScheduleTuesdayBox.setAlpha(0.3f);
        else
            mScheduleTuesdayBox.setAlpha(1.0f);

        if( (onOffTimesEntry.getEnabledTimes() & 0x08) == 0 )
            mScheduleWednesdayBox.setAlpha(0.3f);
        else
            mScheduleWednesdayBox.setAlpha(1.0f);

        if( (onOffTimesEntry.getEnabledTimes() & 0x10) == 0 )
            mScheduleThursdayBox.setAlpha(0.3f);
        else
            mScheduleThursdayBox.setAlpha(1.0f);

        if( (onOffTimesEntry.getEnabledTimes() & 0x20) == 0 )
            mScheduleFridayBox.setAlpha(0.3f);
        else
            mScheduleFridayBox.setAlpha(1.0f);

        if( (onOffTimesEntry.getEnabledTimes() & 0x40) == 0 )
            mScheduleSaturdayBox.setAlpha(0.3f);
        else
            mScheduleSaturdayBox.setAlpha(1.0f);

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


    private void showTimePickerDialog(String weekDayToConfig, String timeString, boolean isOnTime) {
        mWeekDayToConfig = weekDayToConfig;

        FragmentManager fm = getActivity().getSupportFragmentManager();

        TimePickerDialog tp = TimePickerDialog.getInstance("Hora de encendido", timeString + ":0");

        if (isOnTime) {
            tp.setTargetFragment(ConfigurationFragment.this, REQUEST_ON_TIME);
            tp.show(fm, "timePickerOn");
        }
        else {
            tp.setTargetFragment(ConfigurationFragment.this, REQUEST_OFF_TIME);
            tp.show(fm, "timePickerOff");
        }
    }


    private void resetScheduleTime (String weekDayToReset) {
        /**
         * Se envian dos comandos de RESET: RESET(*_LOAD_ON_TIME) y RESET(*_LOAD_OFF_TIME)
         * para el día correspondiente.
         */
        byte register1 = SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME;
        byte register2 = SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME;

        if (weekDayToReset.compareTo("monday") == 0) {
            register1 = SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME;
        }
        else if (weekDayToReset.compareTo("tuesday") == 0) {
            register1 = SmartPlugCommHelper.Registers.TUESDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.TUESDAY_LOAD_OFF_TIME;
        }
        else if (weekDayToReset.compareTo("wednesday") == 0) {
            register1 = SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_OFF_TIME;
        }
        else if (weekDayToReset.compareTo("thursday") == 0) {
            register1 = SmartPlugCommHelper.Registers.THURSDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.THURSDAY_LOAD_OFF_TIME;
        }
        else if (weekDayToReset.compareTo("friday") == 0) {
            register1 = SmartPlugCommHelper.Registers.FRIDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.FRIDAY_LOAD_OFF_TIME;
        }
        else if (weekDayToReset.compareTo("saturday") == 0) {
            register1 = SmartPlugCommHelper.Registers.SATURDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.SATURDAY_LOAD_OFF_TIME;
        }
        else if (weekDayToReset.compareTo("sunday") == 0) {
            register1 = SmartPlugCommHelper.Registers.SUNDAY_LOAD_ON_TIME;
            register2 = SmartPlugCommHelper.Registers.SUNDAY_LOAD_OFF_TIME;
        }

        EventBus.getDefault().post(new CommandEvent(mId, SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.RESET,
                register1)));

        EventBus.getDefault().post(new CommandEvent(mId, SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.RESET,
                register2)));
    }
}
