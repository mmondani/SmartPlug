package com.company.smartplugapp;


import android.app.Activity;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.DialogFragment;
import android.support.v7.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;


/**
 * A simple {@link DialogFragment} subclass.
 */
public class AskingDialog extends DialogFragment {

    private String mTitle;
    private String mMessage;

    private static final String ARG_TITLE = "title";
    private static final String ARG_MESSAGE = "message";


    public static AskingDialog getInstance (String title, String message) {
        AskingDialog fragment = new AskingDialog();

        Bundle args = new Bundle();
        args.putString(ARG_TITLE, title);
        args.putString(ARG_MESSAGE, message);
        fragment.setArguments(args);

        return fragment;
    }


    public AskingDialog() {
        // Required empty public constructor
    }


    @NonNull
    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {

        if (getArguments() != null) {
            mTitle = getArguments().getString(ARG_TITLE);
            mMessage = getArguments().getString(ARG_MESSAGE);
        }

        return new AlertDialog.Builder(getActivity())
                .setTitle(mTitle)
                .setMessage(mMessage)
                .setPositiveButton("SI", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        sendResult (Activity.RESULT_OK);
                    }
                })
                .setNegativeButton("NO", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        sendResult (Activity.RESULT_CANCELED);
                    }
                }).create();
    }


    private void sendResult (int resultCode) {
        if (getTargetFragment() == null)
            return;

        getTargetFragment().onActivityResult(getTargetRequestCode(), resultCode, null);
    }

}
