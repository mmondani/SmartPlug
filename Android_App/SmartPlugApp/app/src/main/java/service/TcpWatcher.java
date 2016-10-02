package service;

import android.content.Context;
import android.os.Handler;
import android.os.Message;

import com.company.smartplugapp.SmartPlugProvider;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.LinkedList;
import java.util.List;

import database.InstantaneousInfoEntry;
import events.AllMessagesSentEvent;
import events.CommandEvent;
import events.ResponseEvent;

/**
 * Created by mariano on 27/09/16.
 */
public class TcpWatcher {

    private Context mContext;
    private TcpClient mTcpClient;
    List<CommandEvent> mCommandQueue;
    private String mCurrentId;


    public TcpWatcher (Context context) {
        mContext = context;
        mCommandQueue = new LinkedList<>();

        if (!EventBus.getDefault().isRegistered(this))
            EventBus.getDefault().register(this);
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        if (EventBus.getDefault().isRegistered(this))
            EventBus.getDefault().unregister(this);
    }

    @Subscribe (threadMode = ThreadMode.POSTING)
    public void onCommandEvent (CommandEvent ev) {
        /**
         * Se agrega el comando a la queue.
         */
        mCommandQueue.add(ev);

        /**
         * Si no se está ejecutando otro comando, se indica que se puede ejecutar.
         */
        if (mTcpClient == null || !mTcpClient.isAlive()) {
            InstantaneousInfoEntry entry = SmartPlugProvider.getInstance(mContext).getInstantaneousInfoEntry(ev.getId());

            if (entry != null) {
                mCommandQueue.remove(0);
                mCurrentId = entry.getId();

                mTcpClient = new TcpClient(new TcpHandler(), entry.getIp(), 2000, ev.getData(), 4000);
                mTcpClient.start();
            }
        }
    }


    private class TcpHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case TcpClient.Messages.ERROR:

                    break;

                case TcpClient.Messages.SENT:

                    break;

                case TcpClient.Messages.TIMEOUT:
                    /** TODO Generar un evento que indique el timeout y el ID del Smart Plug que lo produjo*/

                    /**
                     * Si todavía hay comandos para enviar se los envía.
                     */
                    if (mCommandQueue.size() > 0){
                        CommandEvent ev = mCommandQueue.remove(0);

                        InstantaneousInfoEntry entry = SmartPlugProvider.getInstance(mContext).getInstantaneousInfoEntry(ev.getId());

                        if (entry != null) {
                            mCurrentId = entry.getId();

                            mTcpClient = new TcpClient(new TcpHandler(), entry.getIp(), 2000, ev.getData(), 4000);
                            mTcpClient.start();
                        }
                    }
                    else {
                        EventBus.getDefault().post(new AllMessagesSentEvent());
                    }

                    break;

                case TcpClient.Messages.RECEIVED:
                    /**
                     * Se recibió una respuesta desde el SmartPlug, se genera el evento en el EventBus
                     */
                    EventBus.getDefault().post(new ResponseEvent(mCurrentId, TcpClient.getResponseData(msg.getData())));

                    /**
                     * Se termina el Thread del cliente TCP.
                     */
                    mTcpClient.kill();
                    break;

                case TcpClient.Messages.KILLED:
                    /**
                     * Si todavía hay comandos para enviar se los envía.
                     */
                    if (mCommandQueue.size() > 0){
                        CommandEvent ev = mCommandQueue.remove(0);

                        InstantaneousInfoEntry entry = SmartPlugProvider.getInstance(mContext).getInstantaneousInfoEntry(ev.getId());

                        if (entry != null) {
                            mCurrentId = entry.getId();

                            mTcpClient = new TcpClient(new TcpHandler(), entry.getIp(), 2000, ev.getData(), 4000);
                            mTcpClient.start();
                        }
                    }
                    else {
                        EventBus.getDefault().post(new AllMessagesSentEvent());
                    }

                    break;
            }
        }
    }
}
