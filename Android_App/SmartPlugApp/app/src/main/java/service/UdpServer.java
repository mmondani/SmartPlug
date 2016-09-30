package service;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.nio.ByteBuffer;


/**
 * Clase que implementa un servidor UDP que escucha el puerto pasado como
 * parámetro en su constructor.
 * Funciona en un thread separado y genera el evento HeartbeatEvent en el EventBus
 * cuando llega un datagrama en el puerto que se está escuchando.
 * Para detener su funcionamiento se debe llamar al método kill.
 */
public class UdpServer extends Thread {

    private int mPort;
    private Handler mHandler;
    private boolean mRunning = false;

    private static final String PARAM_DATA = "data";
    private static final String PARAM_IP = "ip";
    private static final String PARAM_PORT = "port";

    public static final class Messages {
        public static final int RECEIVED = 1;
    }


    /**
     * Constructor de la clase UdpServer.
     *
     * @param port Puerto en el que se debe escuchar.
     */
    public UdpServer(Handler handler, int port) {
        mHandler = handler;
        mPort = port;
    }


    /**
     * Método que se ejecuta al llamar a UdpServer.start(). Implementa la lógica del server UDP.
     */
    @Override
    public void run() {
        byte[] message = new byte[600];
        DatagramPacket packet = new DatagramPacket(message, message.length);

        try {
            /**
             * Se crea una instancia de DatagramSocket escuchando en el puerto pasado
             * en el constructor.
             */
            DatagramSocket socket = new DatagramSocket(mPort);
            mRunning = true;

            /**
             * El servidor se ejecutará hasta que se llame al método kill.
             */
            while (mRunning) {
                /**
                 * Se bloquea el thread esperando la recepción de un datagrama.
                 */
                socket.receive(packet);

                /**
                 * Se obtiene el datagrama recibido, la IP desde la que provino
                 * y el puerto.
                 */
                ByteBuffer data = ByteBuffer.allocate(packet.getLength());
                data.put(packet.getData(), 0, packet.getLength());
                String address = packet.getAddress().getHostAddress();
                int port = packet.getPort();

                /**
                 * Llegó un heartbeat al puerto que se estaba escuchando.
                 * Se generará el evento correspondiente.
                 */
                emitNewPacket(address, port, data.array());
            }

            /**
             * Cuando termina la ejecución del servidor, si el socket está abierto,
             * se lo cierra.
             */
            if (socket != null) {
                socket.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Detiene la ejecución del servidor.
     */
    public void kill() {
        this.interrupt();
        mRunning = false;
    }

    /**
     * Retorna si el servidor se encuentra ejecutándose o no.
     * @return false si no se está ejecutando.
     *          true si se está ejecutando.
     */
    public boolean isRunning () {
        return (mRunning);
    }


    public static String getResponseIp (Bundle bundle) {
        return bundle.getString(PARAM_IP);
    }

    public static int getResponsePort (Bundle bundle) {
        return bundle.getInt(PARAM_PORT);
    }

    public static byte[] getResponseData (Bundle bundle) {
        return bundle.getByteArray(PARAM_DATA);
    }


    /**
     * Postea una instancia del evento HeartbeatEvent en el EventBus.
     * @param address IP desde la que proviene el heartbeat.
     * @param port Puerto UDP desde el que proviene el heartbeat.
     * @param data Payload del datagrama.
     */
    private void emitNewPacket (String address, int port, byte[] data) {

        if (mHandler != null) {
            Message msg = new Message();

            msg.what = Messages.RECEIVED;

            Bundle bundle = new Bundle();
            bundle.putByteArray(PARAM_DATA, data);
            bundle.putString(PARAM_IP, address);
            bundle.putInt(PARAM_PORT, port);

            msg.setData(bundle);

            mHandler.sendMessage(msg);
        }
    }
}