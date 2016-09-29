package com.company.smartplugapp;


import android.app.Activity;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;
import org.w3c.dom.Text;

import java.text.SimpleDateFormat;
import java.util.List;

import database.InstantaneousInfoEntry;
import events.CommandEvent;
import events.HeartbeatEvent;
import events.UpdateSmartPlugEvent;


/**
 * Se encarga de mostrar la lista de Smart Plugs encontrados. Los muestra a través
 * de un RecyclerView.
 */
public class SmartPlugListFragment extends Fragment{

    private OnItemClickedInterface mCLickListener = null;
    private RecyclerView mSmartPlugsList;
    private SmartPlugProvider mSmartPlugProvider;
    private SmartPlugAdapter mSmartPlugAdapter;


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
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mSmartPlugProvider = SmartPlugProvider.getInstance(getActivity());
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View v = inflater.inflate(R.layout.fragment_smart_plug_list, container, false);

        Toolbar toolbar = (Toolbar)v.findViewById(R.id.list_fragment_toolbar);
        toolbar.setTitle("Smart Plugs");
        ((AppCompatActivity)getActivity()).setSupportActionBar(toolbar);

        mSmartPlugsList = (RecyclerView)v.findViewById(R.id.frag_smart_plug_list_recycler);
        mSmartPlugsList.setLayoutManager(new LinearLayoutManager(getActivity()));
        mSmartPlugAdapter = new SmartPlugAdapter();
        mSmartPlugsList.setAdapter(mSmartPlugAdapter);

        return v;
    }


    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);

        try {
            mCLickListener = (OnItemClickedInterface)activity;
        }catch (ClassCastException cce) {
            throw new ClassCastException(activity.toString() + " debe implementar OnItemCLickedInterface");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mCLickListener = null;
    }

    @Override
    public void onStart() {
        super.onStart();

        /**
         * Cada vez que se vuelve a mostrar el Fragment se actualiza la información de los SmartPlugs.
         */
        mSmartPlugAdapter.setSmartPlugs(mSmartPlugProvider.getInstance(getActivity()).getSmartPlugs());

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


    /**
     * Se suscribe al evento UpdateSmartPlugEvent. Cada vez que se produce vuelve a obtener la lista
     * de Smart Plugs de la base de datos para mostrar la información actualizada.
     * @param ev Instancia del evento UpdateSmartPlugEvent.
     */
    @Subscribe (threadMode = ThreadMode.MAIN)
    public void onUpdateSmartPlugEvent (UpdateSmartPlugEvent ev) {
        mSmartPlugAdapter.setSmartPlugs(mSmartPlugProvider.getSmartPlugs());
    }


    public interface OnItemClickedInterface {
        void onItemClicked (SmartPlugListItem smartPlugItem);
    }



    // ViewHolder para el RecyclerView
    private class SmartPlugViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {

        private TextView mSmartPlugName;
        private TextView mSmartPlugLastUpdate;
        private ImageView mSmartPlugIcon;
        private ImageView mCommStateIcon;
        private SmartPlugListItem mSmartPlugListItem;


        public SmartPlugViewHolder(View v) {
            super(v);

            v.setOnClickListener(this);

            mSmartPlugIcon = (ImageView)v.findViewById(R.id.card_icon);
            mSmartPlugName = (TextView)v.findViewById(R.id.card_name);
            mSmartPlugLastUpdate = (TextView)v.findViewById(R.id.card_last_update);
            mCommStateIcon = (ImageView)v.findViewById(R.id.card_comm_icon);

            mSmartPlugIcon.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    byte[] data;

                    if (mSmartPlugListItem.isOn())
                        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.NODE_OFF);
                    else
                        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.NODE_ON);

                    EventBus.getDefault().post(new CommandEvent(mSmartPlugListItem.getId(), data));

                }
            });
        }

        public void bindSmartPlug (SmartPlugListItem smartPlug) {
            mSmartPlugListItem = smartPlug;

            if (smartPlug.getIconId() != 0) {
                mSmartPlugIcon.setImageResource(smartPlug.getIconId());
            }

            if (smartPlug.isOn())
                mSmartPlugIcon.setAlpha(1.0f);
            else
                mSmartPlugIcon.setAlpha(0.3f);


            if (smartPlug.getName() != null)
                mSmartPlugName.setText(smartPlug.getName());

            mSmartPlugLastUpdate.setText(new SimpleDateFormat("HH:mm:ss - dd/MM/yyyy").format(smartPlug.getLastUpdate()));

            if (smartPlug.getConnectionState() == 0)
                mCommStateIcon.setImageResource(R.drawable.ok);
            else
                mCommStateIcon.setImageResource(R.drawable.error);
        }

        @Override
        public void onClick(View v) {
            /**
             * Se presionó un Smart Plug. Se debe mostrar una vista de detalle.
             */
            if(mCLickListener != null)
                mCLickListener.onItemClicked(mSmartPlugListItem);
        }
    }

    // Adapter para el RecyclerView
    private class SmartPlugAdapter extends RecyclerView.Adapter<SmartPlugViewHolder> {

        private List<SmartPlugListItem> mSmartPlugs;

        @Override
        public SmartPlugViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            View v = LayoutInflater.from(getActivity()).inflate(R.layout.smart_plug_card, parent, false);

            return new SmartPlugViewHolder(v);
        }

        @Override
        public void onBindViewHolder(SmartPlugViewHolder holder, int position) {
            SmartPlugListItem smartPlug = mSmartPlugs.get(position);

            holder.bindSmartPlug(smartPlug);
        }

        @Override
        public int getItemCount() {
            return mSmartPlugs.size();
        }

        public void setSmartPlugs (List<SmartPlugListItem> smartPlugs) {
            mSmartPlugs = smartPlugs;
            mSmartPlugAdapter.notifyDataSetChanged();
        }
    }
}
