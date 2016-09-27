package service;

import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Message;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Arrays;

/**
 * Permite enviar un mensaje a una socket TCP. Desde el socket de destino se puede esperar recibir
 * una respuesta o no. Si se espera recibir una respuesta se debe indicar un tiempo de timeout en
 * milisegundos.
 * El estado de la conexión TCP y la recepción de una respuesta se indican a la clase que inició el
 * cliente mediante mensajes en un Hanlder (los mensajes que pueden ser enviados en el Hanlder
 * están declarados en la clase Messages).
 */
public class TcpClient extends Thread {

    private Handler mHandler;
    private String mDestIp;
    private int mPort;
    private byte[] mData;
    private int mResponseTimeout;
    private boolean mWaitForAnswer;
    private CountDownTimer mResponseTimeoutTimer;
    private DataOutputStream mOutStream;
    private DataInputStream mInStream;
    private boolean mRunning;

    public static final String PARAM_DATA = "data";
    public static final String PARAM_IP = "ip";
    public static final String PARAM_PORT = "port";


    /**
     * Tipos de mensajes que va a enviar el cliente TCP hacia la clase que lo inició usando el
     * Handler provisto.
     */
    public class Messages {
        /**
         * Se produjo algún tipo en la comunicación .
         */
        public static final int ERROR = 1;

        /**
         * Se estableció la conexión con el host remoto.
         */
        public static final int CONNECTING = 2;

        /**
         * Se envió el mensaja TCP
         */
        public static final int SENT = 3;

        /**
         * Se recibió una respuesta. Este Message va acompañado de un Bundle que contiene la IP
         * del origen de la respuesta, el puerto del origen de la respuesta y la data de la respuesta.
         */
        public static final int RECEIVED = 4;

        /**
         * Se produjo un timeout esperando la respuesta.
         */
        public static final int TIMEOUT = 5;

        /**
         * Indica que el Thread se terminó de ejecutar.
         */
        public static final int KILLED = 6;
    }


    /**
     * Constructor de la clase TcpClient. Se usa este constructor si no se espera una respuesta
     * del destinatario del mensaje TCP.
     * @param handler Instancia de {@link Handler} en la que el cliente TCP va a enviar los mensajes del
     *                estado de la comunicación.
     * @param destIP  Instancia de {@link String} con la dirección IP de destino.
     * @param port    Puerto TCP de destino.
     * @param data    Array de byte que contiene la data a enviar al socket de destino.
     */
    public TcpClient (Handler handler, String destIP, int port, byte[] data) {
        mHandler = handler;
        mDestIp = destIP;
        mPort = port;
        mData = data;
        mResponseTimeout = 0;
        mWaitForAnswer = false;
    }


    /**
     * Constructor de la clase TcpClient. Se usa este constructor si se espera una respuesta
     * del destinatario del mensaje TCP. Se debe indicar el timeout de la espera de la respuesta.
     * @param handler Instancia de {@link Handler} en la que el cliente TCP va a enviar los mensajes del
     *                estado de la comunicación.
     * @param destIP  Instancia de {@link String} con la dirección IP de destino.
     * @param port    Puerto TCP de destino.
     * @param data    Array de byte que contiene la data a enviar al socket de destino.
     * @param responseTimeout Timeout para esperar la respuesta del mensaje TCP. Se expresa en
     *                        milisegundos.
     */
    public TcpClient (Handler handler, String destIP, int port, byte[] data, int responseTimeout) {
        mHandler = handler;
        mDestIp = destIP;
        mPort = port;
        mData = data;
        mResponseTimeout = responseTimeout;
        mWaitForAnswer = true;

        /**
         * Si se espera una respuesta, se crea una instancia de CountDownTimer para el timeout.
         */
        mResponseTimeoutTimer = new CountDownTimer(mResponseTimeout, 100) {
            @Override
            public void onTick(long millisUntilFinished) {

            }

            @Override
            public void onFinish() {
                /**
                 * Si finaliza el CountDownTimer, se termina la ejecución del Thread y se envía un
                 * mensaje al handler indicando que se produjo un timeout.
                 */
                mRunning = false;
                mHandler.sendEmptyMessage(Messages.TIMEOUT);
            }
        };
    }


    @Override
    public void run() {

        /**
         * Se indica que el cliente está corriendo.
         */
        mRunning = true;

        try {
            /**
             * Se crea un socket TCP apuntando a la IP y puerto destinos.
             */
            InetAddress serverAddres = InetAddress.getByName(mDestIp);

            mHandler.sendEmptyMessage(Messages.CONNECTING);

            Socket socket = new Socket(serverAddres, mPort);

            try {
                mOutStream = new DataOutputStream(socket.getOutputStream());
                mInStream = new DataInputStream(socket.getInputStream());

                /**
                 * Se carga en el stream de salida los datos a enviar.
                 */
                if (mOutStream != null) {
                    mOutStream.write(mData, 0, mData.length);
                    mOutStream.flush();
                }

                /**
                 * Se envía un mensaje al handler indicando que se envió el paquete TCP.
                 */
                mHandler.sendEmptyMessage(Messages.SENT);


                /**
                 * Si se espera una respuesta, se inicia el timer de timeout.
                 */
                if (mWaitForAnswer) {
                    // Se espera la respuesta
                    byte[] responseBuffer = new byte[300];

                    mResponseTimeoutTimer.start();

                    /**
                     * Se comienza la recepción de la respuesta.
                     */
                    while (mRunning) {
                        if (socket.getInputStream().available() > 0) {
                            int bytesRead = mInStream.read(responseBuffer, 0, responseBuffer.length);

                            if (bytesRead > 0) {
                                // Se crea un array de byte con el tamaño de los datos leidos.
                                byte[] data = Arrays.copyOf(responseBuffer, bytesRead);


                                if (new String(data).compareTo("*HELLO*") == 0)
                                    continue;

                                /**
                                 * Cuando se recibe una respuesta, en el Handler se envía un mensaje
                                 * junto con un Bundle con la información de la respuesta recibida.
                                 */
                                Message msg = new Message();
                                msg.what = Messages.RECEIVED;
                                Bundle bundle = new Bundle();
                                bundle.putByteArray(PARAM_DATA, data);
                                bundle.putString(PARAM_IP, mDestIp);
                                bundle.putInt(PARAM_PORT, socket.getPort());
                                msg.setData(bundle);

                                mHandler.sendMessage(msg);

                                data = null;
                            }
                        }
                    }


                }

                mHandler.sendEmptyMessage(Messages.KILLED);

            }catch (Exception e) {
                mHandler.sendEmptyMessage(Messages.ERROR);

            } finally {
                mOutStream.flush();
                mOutStream.close();
                mInStream.close();
                socket.close();
            }

        }catch (Exception e) {
            mHandler.sendEmptyMessage(Messages.ERROR);
        }

    }


    /**
     * Permite detener la ejecución del cliente TCP.
     */
    public void kill () {
        mRunning = false;
        mResponseTimeoutTimer.cancel();
    }

    /**
     * Extrae la data de la respuesta recibida.
     * @param bundle Instancia de {@link Bundle} recibida con el mensaje Messages.RECEIVED en el Handler.
     * @return array de byte de la data de la respuesta.
     */
    public static byte[] getResponseData (Bundle bundle) {
        return bundle.getByteArray(PARAM_DATA);
    }

    /**
     * Extrae la dirección IP desde la que se recibió la respuesta.
     * @param bundle Instancia de {@link Bundle} recibida con el mensaje Messages.RECEIVED en el Handler.
     * @return dirección IP desde la que se recibió la respuesta.
     */
    public static String getResponseIp (Bundle bundle) {
        return bundle.getString(PARAM_IP);
    }


    /**
     * EXtrae el puerto TCP desde el que se recibió la respuesta.
     * @param bundle Instancia de {@link Bundle} recibida con el mensaje Messages.RECEIVED en el Handler.
     * @return puerto TCP desde el que se recibió la respuesta.
     */
    public static int getResponsePort (Bundle bundle) {
        return bundle.getInt(PARAM_PORT);
    }
}
