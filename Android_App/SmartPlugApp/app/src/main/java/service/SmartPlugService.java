package service;

import android.app.Service;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.support.annotation.Nullable;


public class SmartPlugService extends Service {

    private static boolean sRunning = false;
    private Runnable mRunnable = null;
    private UdpServer mUdpServer = null;
    private WifiManager.MulticastLock mMulticastLock;


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

        if (sRunning == false) {
            sRunning = true;

            /**
             * Se crea una instancia del servidor UDP para escuchar los heartbeats de los Smart Plugs
             */
            if (mUdpServer == null)
                mUdpServer = new UdpServer(55555);


            /** Se debe crear un MulticastLock para que la aplicación pueda recibir mensajes UDP en la dirección
             * de broadcast.
             * En el Manifest se debe agregar el permiso: android.permission.CHANGE_WIFI_MULTICAST_STATE
             */
            WifiManager wm = (WifiManager) getApplicationContext().getSystemService(getApplicationContext().WIFI_SERVICE);
            mMulticastLock = wm.createMulticastLock("lock");

            mMulticastLock.acquire();

            /**
             * Si el servidor UDP no está corriendo, se lo inicia.
             */
            if (!mUdpServer.isRunning())
                mUdpServer.start();


            mRunnable = new Runnable() {
                @Override
                public void run() {


                    mHandler.postDelayed(mRunnable, 5 * 1000);
                }
            };

            mHandler.postDelayed(mRunnable, 5 * 1000);
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

        super.onDestroy();
    }
}
