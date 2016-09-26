package service;

import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import org.greenrobot.eventbus.EventBus;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.util.Calendar;

import events.HeartbeatEvent;


public class UdpServer extends Thread {

    private int mPort;
    private boolean mRunning = false;

    private static final String BUNDLE_MSG = "msg";
    private static final String BUNDLE_ADDRESS = "address";
    private static final String BUNDLE_PORT = "port";



    public static class Messages {
        public static final int RECEIVED = 1;
    }




    public UdpServer(int port) {
        mPort = port;
    }


    @Override
    public void run() {
        byte[] message = new byte[600];
        DatagramPacket packet = new DatagramPacket(message, message.length);

        try {
            DatagramSocket socket = new DatagramSocket(mPort);
            mRunning = true;

            while (mRunning) {
                socket.receive(packet);

                ByteBuffer data = ByteBuffer.allocate(packet.getLength());
                data.put(packet.getData(), 0, packet.getLength());
                InetAddress address = packet.getAddress();
                int port = packet.getPort();

                /**
                 * Llegó un heartbeat al puerto que se estaba escuchando.
                 */
                emitNewPacket(address, port, data.array());
            }

            if (socket != null) {
                socket.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void kill () {
        this.interrupt();
        mRunning = false;
    }

    public boolean isRunning () {
        return (mRunning);
    }


    public static String getAddress (Bundle msg) {
        return msg.getString(BUNDLE_ADDRESS);
    }

    public static int getPort (Bundle msg) {
        return msg.getInt(BUNDLE_PORT);
    }

    public static byte[] getData (Bundle msg) {
        return msg.getByteArray(BUNDLE_MSG);
    }

    private void emitNewPacket (InetAddress address, int port, byte[] data) {

        /**
         * Se usa la clase SmartPlugCommHelper para parsear la data recibida
         * TODO: Implementar la clase SmartPlugCommHelper
         */

        /**
         * Se genera un HeartbeatEvent.
         */
        EventBus.getDefault().post(new HeartbeatEvent("123456", address.toString(), Calendar.getInstance().getTime()));

    }
}