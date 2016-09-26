package service;

import android.os.Bundle;

import org.greenrobot.eventbus.EventBus;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;
import java.util.Calendar;

import com.company.smartplugapp.SmartPlugCommHelper;
import com.company.smartplugapp.SmartPlugCommHelper.HeartbeatFrame;
import events.HeartbeatEvent;


/**
 * Clase que implementa un servidor UDP que escucha el puerto pasado como
 * parámetro en su constructor.
 * Funciona en un thread separado y genera el evento HeartbeatEvent en el EventBus
 * cuando llega un datagrama en el puerto que se está escuchando.
 * Para detener su funcionamiento se debe llamar al método kill.
 */
public class UdpServer extends Thread {

    private int mPort;
    private boolean mRunning = false;


    /**
     * Constructor de la clase UdpServer.
     *
     * @param port Puerto en el que se debe escuchar.
     */
    public UdpServer(int port) {
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
                InetAddress address = packet.getAddress();
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

    /**
     * Postea una instancia del evento HeartbeatEvent en el EventBus.
     * @param address IP desde la que proviene el heartbeat.
     * @param port Puerto UDP desde el que proviene el heartbeat.
     * @param data Payload del datagrama.
     */
    private void emitNewPacket (InetAddress address, int port, byte[] data) {

        /**
         * Se usa la clase SmartPlugCommHelper para parsear la data recibida
         */
        HeartbeatFrame heartbeatFrame = SmartPlugCommHelper.getInstance().parseHeartBeat(data);

        /**
         * Se genera un HeartbeatEvent.
         */
        EventBus.getDefault().post(new HeartbeatEvent(heartbeatFrame.getId(), address.toString(), Calendar.getInstance().getTime()));

    }
}