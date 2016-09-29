package com.company.smartplugapp;


import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.widget.TimePicker;

import database.SimpleTime;


public class TimePickerDialog extends DialogFragment {

    private static final String ARG_TITLE = "title";
    private static final String ARG_INITIAL_TIME = "initial_time";
    private static final String EXTRA_TIME = "time";

    private String mTitle;
    private SimpleTime mInitialTime;


    public static TimePickerDialog getInstance (String title, String initialTime) {
        TimePickerDialog dialog = new TimePickerDialog();

        Bundle args = new Bundle();
        args.putString(ARG_TITLE, title);
        args.putString(ARG_INITIAL_TIME, initialTime);

        dialog.setArguments(args);

        return dialog;
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (getArguments() != null) {
            mTitle = getArguments().getString(ARG_TITLE);
            mInitialTime = new SimpleTime(getArguments().getString(ARG_INITIAL_TIME));
        }
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final TimePicker timePicker = new TimePicker(getActivity());

        timePicker.setCurrentHour((int)mInitialTime.getHours());
        timePicker.setCurrentMinute((int) mInitialTime.getMinutes());
        timePicker.setIs24HourView(true);

        return new AlertDialog.Builder(getActivity())
                .setView(timePicker)
                .setTitle(mTitle)
                .setPositiveButton("OK", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        SimpleTime time = new SimpleTime();
                        time.setHours(timePicker.getCurrentHour().byteValue());
                        time.setMinutes(timePicker.getCurrentMinute().byteValue());

                        sendResult(Activity.RESULT_OK, time);
                    }
                })
                .setNegativeButton("PASAR", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        sendResult(Activity.RESULT_CANCELED, null);
                    }
                })
                .create();
    }



    public static SimpleTime getIntentTime (Intent i) {
        return new SimpleTime(i.getStringExtra(EXTRA_TIME));
    }


    private void sendResult (int resultCode, SimpleTime time) {
        if (getTargetFragment() == null)
            return;

        Intent i = new Intent();

        if(resultCode == Activity.RESULT_OK)
            i.putExtra(EXTRA_TIME, time.toString());

        getTargetFragment().onActivityResult(getTargetRequestCode(), resultCode, i);
    }
}
