package service;

import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import com.company.smartplugapp.SmartPlugCommHelper;

import org.greenrobot.eventbus.EventBus;

import java.util.Calendar;

import events.HeartbeatEvent;

/**
 * Created by mariano on 27/09/16.
 */
public class UdpWatcher {
    private UdpServer mUdpServer = null;


    public UdpWatcher () {

        /**
         * Se crea una instancia del servidor UDP para escuchar los heartbeats de los Smart Plugs
         */
        if (mUdpServer == null)
            mUdpServer = new UdpServer(new UdpHandler(), 55555);

        /**
         * Si el servidor UDP no está corriendo, se lo inicia.
         */
        if (!mUdpServer.isRunning())
            mUdpServer.start();
    }


    private class UdpHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch(msg.what) {
                case UdpServer.Messages.RECEIVED:

                    /**
                     * Se obtienen los datos del mensaje recibido por UDP
                     */
                    Bundle bundle = msg.getData();
                    byte[] data = UdpServer.getResponseData(bundle);
                    String address = UdpServer.getResponseIp(bundle);

                    /**
                     * Se usa la clase SmartPlugCommHelper para parsear la data recibida
                     */
                    SmartPlugCommHelper.HeartbeatFrame heartbeatFrame = SmartPlugCommHelper.getInstance().parseHeartBeat(data);

                    /**
                     * Se genera un HeartbeatEvent.
                     */
                    EventBus.getDefault().post(new HeartbeatEvent(heartbeatFrame.getId(), address, Calendar.getInstance().getTime()));

                    break;
            }
        }
    }
}
