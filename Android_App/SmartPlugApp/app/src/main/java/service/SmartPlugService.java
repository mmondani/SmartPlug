package service;

import android.app.Service;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.support.annotation.Nullable;

import com.company.smartplugapp.BasicFrame;
import com.company.smartplugapp.NoParamFrame;
import com.company.smartplugapp.SmartPlugCommHelper;
import com.company.smartplugapp.SmartPlugProvider;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import database.InstantaneousInfoEntry;
import events.HeartbeatEvent;
import events.ResponseEvent;
import events.UpdateSmartPlugEvent;


public class SmartPlugService extends Service {

    private static boolean sRunning = false;
    private Runnable mRunnable = null;
    private WifiManager.MulticastLock mMulticastLock;
    private UdpWatcher mUdpWatcher;
    private TcpWatcher mTcpWatcher;


    public SmartPlugService (){

    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        throw new UnsupportedOperationException("Falta implementar");
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        final Handler mHandler = new Handler();

        /**
         * Solo puede estar corriendo un servicio.
         */
        if (sRunning == false) {
            sRunning = true;

            /**
             * Se registra el servicio en el EventBus para empezar a recibir los eventos a los
             * que está suscrito.
             */
            EventBus.getDefault().register(this);

            /**
             * Se crea una instancia de UdpWatcher, el cual va a recibir los heartbeat UDP desde
             * los Smart Plugs.
             */
            mUdpWatcher = new UdpWatcher();


            /**
             * Se crea una instancia de TcpWatcher, el cual se va a encargar de enviar y recibir los
             * mensajes TCP con los SmartPlugs.
             */
            mTcpWatcher = new TcpWatcher(getApplicationContext());


            /** Se debe crear un MulticastLock para que la aplicación pueda recibir mensajes UDP en la dirección
             * de broadcast.
             * En el Manifest se debe agregar el permiso: android.permission.CHANGE_WIFI_MULTICAST_STATE
             */
            WifiManager wm = (WifiManager) getApplicationContext().getSystemService(getApplicationContext().WIFI_SERVICE);
            mMulticastLock = wm.createMulticastLock("lock");

            mMulticastLock.acquire();


            /**
             * Cada N minutos, la aplicación consulta a todos los Smart Plugs que tiene conectados para
             * conocer sus nuevos valores.
             */
            mRunnable = new Runnable() {
                @Override
                public void run() {


                    mHandler.postDelayed(mRunnable, 5 * 60 * 1000);
                }
            };

            mHandler.postDelayed(mRunnable, 5 * 60 * 1000);
        }
        return Service.START_STICKY;

    }

    @Override
    public void onCreate() {
        super.onCreate();
    }



    @Override
    public void onDestroy() {
        if (mMulticastLock.isHeld())
            mMulticastLock.release();

        EventBus.getDefault().unregister(this);

        super.onDestroy();
    }


    /**
     * Se suscribe al evento HeartbeatEvent. Cada vez que reciba un evento, va a actualizar la base
     * de datos con la IP desde donde recibió el mensaje y la fecha en la que lo recibió.
     * @param ev Instancia del evento HeartbeatEvent.
     */
    @Subscribe(threadMode = ThreadMode.POSTING)
    public void onHeartbeatEvent (HeartbeatEvent ev) {
        SmartPlugProvider smartPlugProvider = SmartPlugProvider.getInstance(getApplicationContext());

        if (!smartPlugProvider.isSmartPlugInDb(ev.getId())) {
            /**
             * Si el ID que se recibió no está en la base de datos, se lo agrega
             */
            smartPlugProvider.addSmartPlug(ev.getId(), ev.getIp());
        }

        /**
         * Si ya está en la base de datos se actualiza la información de la IP y del last update.
         */
        InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());
        entry.setIp(ev.getIp());
        entry.setLastUpdate(ev.getDate());

        smartPlugProvider.updateInstantaneousInfoEntry(entry);

        /**
         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
         */
        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
    }


    /**
     * Se suscribe al evento ResponseEvent. Cuando llega una respuesta actualiza los valores en la
     * base de datos.
     * @param ev Instancia del evento ResponseEvent.
     */
    @Subscribe (threadMode = ThreadMode.POSTING)
    public void onResponseEvent (ResponseEvent ev) {
        SmartPlugProvider smartPlugProvider = SmartPlugProvider.getInstance(getApplicationContext());

        /**
         * Se parsea la data recibida de acuerdo al comando y al registro.
         */
        BasicFrame basicFrame = SmartPlugCommHelper.getInstance().parseFrame(ev.getData());

        if (basicFrame != null) {
            if (basicFrame.getFrameType() == BasicFrame.Types.NO_PARAM) {
                NoParamFrame frame = (NoParamFrame)basicFrame;

                if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_NODE_ON) {
                    InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());
                    entry.setLoadState(1);
                    smartPlugProvider.updateInstantaneousInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
                else if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_NODE_OFF) {
                    InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());
                    entry.setLoadState(0);
                    smartPlugProvider.updateInstantaneousInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
            }
        }
    }
}
