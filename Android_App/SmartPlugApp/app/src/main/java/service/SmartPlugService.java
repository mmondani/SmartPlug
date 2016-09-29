package service;

import android.app.Service;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.support.annotation.Nullable;

import com.company.smartplugapp.BasicFrame;
import com.company.smartplugapp.ByteArrayFrame;
import com.company.smartplugapp.ByteFrame;
import com.company.smartplugapp.FloatArrayFrame;
import com.company.smartplugapp.NoParamFrame;
import com.company.smartplugapp.SmartPlugCommHelper;
import com.company.smartplugapp.SmartPlugListItem;
import com.company.smartplugapp.SmartPlugProvider;
import com.company.smartplugapp.StringFrame;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.List;

import database.InstantaneousInfoEntry;
import database.OnOffTimesEntry;
import database.SimpleTime;
import database.StaticInfoEntry;
import events.CommandEvent;
import events.HeartbeatEvent;
import events.ResponseEvent;
import events.UpdateSmartPlugEvent;


public class SmartPlugService extends Service {

    private static boolean sRunning = false;
    private Runnable mRunnableBasicInfo = null;
    private Runnable mRunnableMeasurements = null;
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
             * Cada 10 minutos, la aplicación consulta a todos los Smart Plugs que tiene conectados para
             * conocer sus nuevos valores y determinar si otra aplicación cambió alguna de sus configuraciones.
             */
            mRunnableBasicInfo = new Runnable() {
                @Override
                public void run() {
                    /**
                     * Se determianan todos los Smart Plugs dados de alta en la base de datos.
                     */
                    List<SmartPlugListItem> smartPlugList = SmartPlugProvider.getInstance(getApplicationContext()).getSmartPlugs();

                    for (SmartPlugListItem smartPlug : smartPlugList) {
                        byte[] data;

                        /**
                         * Se pide el nombre del Smart Plug para ver si cambió.
                         */
                        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                                                                            SmartPlugCommHelper.Registers.DEVICE_ID);
                        EventBus.getDefault().post(new CommandEvent(smartPlug.getId(), data));


                        /**
                         * Se piden todos los horarios de encendido y apagado para ver si alguno cambió.
                         */
                        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                                                                                    SmartPlugCommHelper.Registers.ONOFF_TIMES);
                        EventBus.getDefault().post(new CommandEvent(smartPlug.getId(), data));


                        /**
                         * Se piden las mediciones de tensión, corriente, potencia activa y energía total acumulada.
                         */
                        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                                                                            SmartPlugCommHelper.Registers.CURRENT_MEASUREMENTS);
                        EventBus.getDefault().post(new CommandEvent(smartPlug.getId(), data));


                        /**
                         * Se pide si está encendida la carga o no.
                         */
                        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                                SmartPlugCommHelper.Registers.LOAD_STATE);
                        EventBus.getDefault().post(new CommandEvent(smartPlug.getId(), data));
                    }


                    mHandler.postDelayed(mRunnableBasicInfo, 10 * 60 * 1000);
                }
            };

            /**
             * La primera vez se ejecuta a los 2 segundos de iniciado el servicio.
             * TODO Cambiarlo a 6 segundos para permitir que haya recibido los heartbeats la primera vez que se instala la aplicación.
             */
            mHandler.postDelayed(mRunnableBasicInfo, 2 * 1000);


            /**
             * Cada 1 hora, se le solicita a todos los Smart Plugs las mediciones de energía y potencia
             * por hora.
             */
            mRunnableMeasurements = new Runnable() {
                @Override
                public void run() {


                    mHandler.postDelayed(mRunnableMeasurements, 10 * 60 * 1000);
                }
            };

            /**
             * La primera vez se ejecuta a los 10 segundos de iniciado el servicio.
             */
            mHandler.postDelayed(mRunnableMeasurements, 10 * 1000);
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
         * Si ya está en la base de datos se actualiza la información de la IP.
         */
        InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());
        entry.setIp(ev.getIp());

        smartPlugProvider.updateInstantaneousInfoEntry(entry);
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
                    entry.setLastUpdate(Calendar.getInstance().getTime());
                    smartPlugProvider.updateInstantaneousInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
                else if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_NODE_OFF) {
                    InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());
                    entry.setLoadState(0);
                    entry.setLastUpdate(Calendar.getInstance().getTime());
                    smartPlugProvider.updateInstantaneousInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.FLOAT_ARRAY_PARAM) {
                FloatArrayFrame frame = (FloatArrayFrame)basicFrame;

                /**
                 * Los comandos que pueden devolver este tipo de trama son los GET de los registros:
                 * CURRENT_MEASUREMENTS, PER_HOUR_ACTIVE_POWER y PER_HOUR_ENERGY
                 */
                if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET &&
                        frame.getRegister() == SmartPlugCommHelper.Registers.CURRENT_MEASUREMENTS) {
                    /**
                     * Si es un GET de CURRENT_MEASUREMENTS, en la trama tiene que haber 4 floats que
                     * corresponden a: tensión, corriente, potencia y energía total acumulada.
                     * Se actualiza la entrada en InstantaneousInfo con estos valores.
                     */
                    if (frame.getData().length == 4) {
                        InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());

                        /**
                         * Se chequean los rangos de las mediciones
                         */
                        if(frame.getData()[0] < 0.0f || frame.getData()[0] > 250.0f)
                            entry.setVoltage(0.0f);
                        else
                            entry.setVoltage(frame.getData()[0]);

                        if(frame.getData()[1] < 0.0f || frame.getData()[1] > 10.0f)
                            entry.setCurrent(0.0f);
                        else
                            entry.setCurrent(frame.getData()[1]);

                        if(frame.getData()[2] < 0.0f || frame.getData()[2] > 2500.0f)
                            entry.setPower(0.0f);
                        else
                            entry.setPower(frame.getData()[2]);

                        if(frame.getData()[3] < 0.0f || frame.getData()[3] > 1000000.f)
                            entry.setTotalEnergy(0.0f);
                        else
                            entry.setTotalEnergy(frame.getData()[3]);

                        entry.setLastUpdate(Calendar.getInstance().getTime());
                        smartPlugProvider.updateInstantaneousInfoEntry(entry);

                        /**
                         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                         */
                        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                    }
                }
                /** TODO Implementar la lectura de la trama de GET(PER_HOUR_ACTIVE_POWER) y GET(PER_HOUR_ENERGY) */
            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.BYTE_PARAM) {
                ByteFrame frame = (ByteFrame)basicFrame;

                /**
                 * Los comandos que pueden devolver este tipo de trama son los GET de los registros:
                 * ENABLE_ONOFF_TIME y LOAD_STATE
                 */
                if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET &&
                        frame.getRegister() == SmartPlugCommHelper.Registers.ENABLE_ONOFF_TIME) {
                    /**
                     * Se recibe un byte en el que cada bit representa si está habilitada la programación
                     * horaria para cada día. El bit 0 corresponde al domingo y el 6 al sábado.
                     */
                    OnOffTimesEntry entry = smartPlugProvider.getOnOffTimesEntry(ev.getId());
                    entry.setEnabledTimes(frame.getData());
                    smartPlugProvider.updateOnOffTimesEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
                else if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET &&
                        frame.getRegister() == SmartPlugCommHelper.Registers.LOAD_STATE) {
                    /**
                     * Se recibe un byte que indica si la carga está encendida (1) o no (0)
                     */
                    InstantaneousInfoEntry entry = smartPlugProvider.getInstantaneousInfoEntry(ev.getId());

                    if (frame.getData() == 0)
                        entry.setLoadState(0);
                    else
                        entry.setLoadState(1);

                    entry.setLastUpdate(Calendar.getInstance().getTime());
                    smartPlugProvider.updateInstantaneousInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.BYTE_ARRAY_PARAM) {
                ByteArrayFrame frame = (ByteArrayFrame)basicFrame;

                /**
                 * Los comandos que pueden devolver este tipo de trama son los GET de los registros:
                 * *_LOAD_ON_TIME, *_LOAD_OFF_TIME y ONOFF_TIMES
                 */
                if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET &&
                        frame.getRegister() == SmartPlugCommHelper.Registers.ONOFF_TIMES) {
                    /**
                     * Si es un GET(ONOFF_TIMES) la trama tiene que tener 29 bytes: 7 horarios de encendido
                     * (hora y minutos), 7 horarios de apagado (hora y minutos) y 1 byte que indica
                     * los días que está habilitada la programación horaria.
                     */
                    if (frame.getData().length == 29) {
                        OnOffTimesEntry entry = smartPlugProvider.getOnOffTimesEntry(ev.getId());
                        entry.getMondayLoadOnTime().setHours(frame.getData()[0]);
                        entry.getMondayLoadOnTime().setMinutes(frame.getData()[1]);
                        entry.getMondayLoadOffTime().setHours(frame.getData()[2]);
                        entry.getMondayLoadOffTime().setMinutes(frame.getData()[3]);

                        entry.getTuesdayLoadOnTime().setHours(frame.getData()[4]);
                        entry.getTuesdayLoadOnTime().setMinutes(frame.getData()[5]);
                        entry.getTuesdayLoadOffTime().setHours(frame.getData()[6]);
                        entry.getTuesdayLoadOffTime().setMinutes(frame.getData()[7]);

                        entry.getWednesdayLoadOnTime().setHours(frame.getData()[8]);
                        entry.getWednesdayLoadOnTime().setMinutes(frame.getData()[9]);
                        entry.getWednesdayLoadOffTime().setHours(frame.getData()[10]);
                        entry.getWednesdayLoadOffTime().setMinutes(frame.getData()[11]);

                        entry.getThursdayLoadOnTime().setHours(frame.getData()[12]);
                        entry.getThursdayLoadOnTime().setMinutes(frame.getData()[13]);
                        entry.getThursdayLoadOffTime().setHours(frame.getData()[14]);
                        entry.getThursdayLoadOffTime().setMinutes(frame.getData()[15]);

                        entry.getFridayLoadOnTime().setHours(frame.getData()[16]);
                        entry.getFridayLoadOnTime().setMinutes(frame.getData()[17]);
                        entry.getFridayLoadOffTime().setHours(frame.getData()[18]);
                        entry.getFridayLoadOffTime().setMinutes(frame.getData()[19]);

                        entry.getSaturdayLoadOnTime().setHours(frame.getData()[20]);
                        entry.getSaturdayLoadOnTime().setMinutes(frame.getData()[21]);
                        entry.getSaturdayLoadOffTime().setHours(frame.getData()[22]);
                        entry.getSaturdayLoadOffTime().setMinutes(frame.getData()[23]);

                        entry.getSundayLoadOnTime().setHours(frame.getData()[24]);
                        entry.getSundayLoadOnTime().setMinutes(frame.getData()[25]);
                        entry.getSundayLoadOffTime().setHours(frame.getData()[26]);
                        entry.getSundayLoadOffTime().setMinutes(frame.getData()[27]);

                        entry.setEnabledTimes(frame.getData()[28]);
                        smartPlugProvider.updateOnOffTimesEntry(entry);

                        /**
                         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                         */
                        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                    }
                }
                /** TODO Implementar la lectura de la trama para GET(*_LOAD_ON_TIME) y GET(*_LOAD_OFF_TIME) */
            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.STRING_PARAM) {
                StringFrame frame = (StringFrame)basicFrame;

                /**
                 * El comando que puede devolcer este tipo de trama es el GET(DEVICE_ID)
                 */
                if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET &&
                        frame.getRegister() == SmartPlugCommHelper.Registers.DEVICE_ID) {
                    /**
                     * Si es un GET(DEVICE_ID) la trama tiene que tener un String de 32 caracteres
                     */
                    StaticInfoEntry entry = smartPlugProvider.getStaticInfoEntry(ev.getId());
                    entry.setName(frame.getData());
                    smartPlugProvider.updateStaticInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
            }
        }
    }
}
