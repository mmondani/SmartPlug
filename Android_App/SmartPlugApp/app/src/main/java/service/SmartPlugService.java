package service;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.IBinder;
import android.os.PowerManager;
import android.support.annotation.Nullable;

import database.MeasurementsEntry;
import events.AllMessagesSentEvent;
import events.TcpTimeoutEvent;
import events.WiFiStateEvent;
import smartPlugComm.BasicFrame;
import smartPlugComm.ByteArrayFrame;
import smartPlugComm.ByteFloatArrayFrame;
import smartPlugComm.ByteFrame;
import smartPlugComm.FloatArrayFrame;
import smartPlugComm.NoParamFrame;
import smartPlugComm.SmartPlugCommHelper;
import com.company.smartplugapp.SmartPlugListItem;
import com.company.smartplugapp.SmartPlugProvider;
import smartPlugComm.StringFrame;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.Calendar;
import java.util.List;

import database.InstantaneousInfoEntry;
import database.OnOffTimesEntry;
import database.StaticInfoEntry;
import events.CommandEvent;
import events.HeartbeatEvent;
import events.ResponseEvent;
import events.UpdateSmartPlugEvent;


public class SmartPlugService extends Service {

    private static boolean sRunning = false;
    private static int sNumberOfRuns = 0;
    private Runnable mRunnableBasicInfo = null;
    private Runnable mRunnableMeasurements = null;
    private WifiManager.MulticastLock mMulticastLock;
    private UdpWatcher mUdpWatcher;
    private TcpWatcher mTcpWatcher;
    private PowerManager.WakeLock mWakeLock;
    private static boolean mWifiIsOn = false;


    public SmartPlugService (){

    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        throw new UnsupportedOperationException("Falta implementar");
    }


    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        List<SmartPlugListItem> smartPlugList;
        boolean messagesToSend = false;

        /**
         * Solo puede estar corriendo un servicio.
         */
        sRunning = true;

        /**
         * El servicio ejecuta su lógica solamente si está conectado a WiFi.
         */
        if (mWifiIsOn) {
            if (!mWakeLock.isHeld())
                mWakeLock.acquire();
            if (!mMulticastLock.isHeld())
                mMulticastLock.acquire();

            /**
             * Cada 10 minutos, la aplicación consulta a todos los Smart Plugs que tiene conectados para
             * conocer sus nuevos valores y determinar si otra aplicación cambió alguna de sus configuraciones.
             */
            sNumberOfRuns++;
            /**
             * Se determianan todos los Smart Plugs dados de alta en la base de datos.
             */
            smartPlugList = SmartPlugProvider.getInstance(getApplicationContext()).getSmartPlugs();

            for (SmartPlugListItem smartPlug : smartPlugList) {
                queryInitialValues(smartPlug.getId());

                messagesToSend = true;
            }


            /**
             * Cada aproximadamente 1 hora, se le solicita a todos los Smart Plugs las mediciones de energía y potencia
             * por hora.
             */
            /**
             * Se determianan todos los Smart Plugs dados de alta en la base de datos.
             */
            if (sNumberOfRuns >= 6) {
                sNumberOfRuns = 0;

                smartPlugList = SmartPlugProvider.getInstance(getApplicationContext()).getSmartPlugs();

                for (SmartPlugListItem smartPlug : smartPlugList) {
                    byte[] data;
                    Calendar calendar = Calendar.getInstance();

                    /**
                     * Se piden las mediciones del día de la fecha
                     */
                    byte day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
                    byte month = (byte) (calendar.get(Calendar.MONTH) + 1);
                    byte year = (byte) (calendar.get(Calendar.YEAR) - 2000);


                    EventBus.getDefault().post(new CommandEvent(smartPlug.getId(), SmartPlugCommHelper.getInstance().
                            getRawData(SmartPlugCommHelper.Commands.GET,
                                    SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                                    new byte[]{day, month, year}
                            )
                    ));

                    EventBus.getDefault().post(new CommandEvent(smartPlug.getId(), SmartPlugCommHelper.getInstance().
                            getRawData(SmartPlugCommHelper.Commands.GET,
                                    SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                                    new byte[]{day, month, year}
                            )
                    ));

                    messagesToSend = true;
                }
            }
        }

        /**
         * Si no se cargó ningún comando para enviar, se liberan los locks, si estaban retenidos.
         */
        if (!messagesToSend) {
            if (mWakeLock.isHeld())
                mWakeLock.release();

            if (mMulticastLock.isHeld())
                mMulticastLock.release();
        }

        return Service.START_STICKY;

    }

    @Override
    public void onCreate() {
        super.onCreate();

        sNumberOfRuns = 0;
        sRunning = false;

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


        /**
         * Se debe crear un MulticastLock para que la aplicación pueda recibir mensajes UDP en la dirección
         * de broadcast.
         * En el Manifest se debe agregar el permiso: android.permission.CHANGE_WIFI_MULTICAST_STATE
         */
        WifiManager wm = (WifiManager) getApplicationContext().getSystemService(getApplicationContext().WIFI_SERVICE);
        mMulticastLock = wm.createMulticastLock("lock");

        if (!mMulticastLock.isHeld())
            mMulticastLock.acquire();

        /**
         * Se crea un WakeLock para mantener despierta a la CPU cada vez que se inicia el servicio
         * para que pueda enviar todos los mensajes que tenga que mandar.
         * Cuando termina de mandar todos los mensajes, se libera el lock.
         */
        PowerManager pm = (PowerManager)getSystemService(POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "MyWakeLock");


        /**
         * Se determina si está conectado a una red WiFi o no.
         * Se deben agregar en el AndroidManifest los permisos: android.permission.ACCESS_WIFI_STATE y
         * android.permission.ACCESS_NETWORK_STATE
         * TODO: implementar la lógica con el SSID de la red para saber si está en la red de los Smart Plugs o en otro.
         */
        ConnectivityManager cm = (ConnectivityManager)getApplicationContext().getSystemService(CONNECTIVITY_SERVICE);
        NetworkInfo ni = cm.getActiveNetworkInfo();

        if (ni != null && ni.getType() == ConnectivityManager.TYPE_WIFI)
            EventBus.getDefault().post(new WiFiStateEvent(true));
        else
            EventBus.getDefault().post(new WiFiStateEvent(false));

        /**
         * Se configuran las alarmas para disparar el servicio, cada 10 minutos.
         */
        Calendar calendar = Calendar.getInstance();
        AlarmManager am = (AlarmManager)getSystemService(ALARM_SERVICE);
        Intent serviceIntent = new Intent(getApplicationContext(), SmartPlugService.class);

        PendingIntent servicePendingIntent = PendingIntent.getService(
                getApplicationContext(),
                1,
                serviceIntent,
                PendingIntent.FLAG_CANCEL_CURRENT
        );

        am.setRepeating(
                AlarmManager.RTC_WAKEUP,
                calendar.getTimeInMillis(),
                10 * 60 * 1000,
                servicePendingIntent
        );
    }


    public static boolean isRunning () {
        return sRunning;
    }

    @Override
    public void onDestroy() {
        cleanup ();
        super.onDestroy();
    }


    /**
     * Cuando se terminan de mandar todos los mensajes, si estaba retenido el wake-lock y/o
     * el multicast-lock, se los libera.
     * @param ev Instancia de AllMessagesSentEvent.
     */
    @Subscribe (threadMode = ThreadMode.POSTING)
    public void onAllMeassagesSent (AllMessagesSentEvent ev) {
        if (mWakeLock.isHeld())
            mWakeLock.release();

        if (mMulticastLock.isHeld())
            mMulticastLock.release();
    }


    /**
     * Se suscribe al evento que informa el estado de la conexión a WiFi.
     * @param ev Instancia del evento WiFiStateEvent.
     */
    @Subscribe (threadMode = ThreadMode.POSTING)
    public void onWiFiStateEvent (WiFiStateEvent ev) {
        if (ev.isWiFiState())
            mWifiIsOn = true;
        else
            mWifiIsOn = false;
    }



    /**
     * Se suscribe al evento que informa el timeout en un mensaje TCP hacia un Smart Plug.
     * Este timeout se va a sumar en la columna correspondiente de InstantaneousInfo.
     * Si hay más de 5 timeouts en el ID correspondiente, se pone en 1 el ConnectionState
     * para indicar error.
     * @param ev Instancia del evento TcpTimeoutEvent.
     */
    @Subscribe (threadMode = ThreadMode.POSTING)
    public void onTcpTimeoutEvent (TcpTimeoutEvent ev) {
        InstantaneousInfoEntry entry = SmartPlugProvider.getInstance(getApplicationContext()).getInstantaneousInfoEntry(ev.getId());

        if (entry != null) {
            entry.setTimeouts(entry.getTimeouts() + 1);

            if (entry.getTimeouts() > 5) {
                entry.setConnectionState(1);

                /**
                 * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                 */
                EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
            }

            SmartPlugProvider.getInstance(getApplicationContext()).updateInstantaneousInfoEntry(entry);
        }
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

            /**
             * Se le van a pedir las mediciones históricas de los últimos 7 días tanto de potencia
             * como de enegía.
             */
            queryWeekMeasurements(ev.getId());
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
                    entry.setTimeouts(0);
                    entry.setConnectionState(0);
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
                    entry.setTimeouts(0);
                    entry.setConnectionState(0);
                    smartPlugProvider.updateInstantaneousInfoEntry(entry);

                    /**
                     * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                     */
                    EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                }
                else if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_RESET) {
                    if ( (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME) ||
                            (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_OFF_TIME) ||
                            (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_OFF_TIME) ||
                            (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_OFF_TIME) ||
                            (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_OFF_TIME) ||
                            (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_OFF_TIME) ||
                            (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_OFF_TIME) ) {
                        /**
                         * Si se resetea una de estas horas se debe poner en 0 los minutos y los segundos
                         * y clerear el bit de enabledTimes
                         */
                        OnOffTimesEntry entry = smartPlugProvider.getOnOffTimesEntry(ev.getId());

                        if (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME) {
                            entry.getMondayLoadOnTime().setHours((byte)0);
                            entry.getMondayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x02));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME){
                            entry.getMondayLoadOffTime().setHours((byte)0);
                            entry.getMondayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x02));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_ON_TIME) {
                            entry.getTuesdayLoadOnTime().setHours((byte)0);
                            entry.getTuesdayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x04));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_OFF_TIME){
                            entry.getTuesdayLoadOffTime().setHours((byte)0);
                            entry.getTuesdayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x04));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_ON_TIME) {
                            entry.getWednesdayLoadOnTime().setHours((byte)0);
                            entry.getWednesdayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x08));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_OFF_TIME){
                            entry.getWednesdayLoadOffTime().setHours((byte)0);
                            entry.getWednesdayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x08));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_ON_TIME) {
                            entry.getThursdayLoadOnTime().setHours((byte)0);
                            entry.getThursdayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x10));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_OFF_TIME){
                            entry.getThursdayLoadOffTime().setHours((byte)0);
                            entry.getThursdayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x10));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_ON_TIME) {
                            entry.getFridayLoadOnTime().setHours((byte)0);
                            entry.getFridayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x20));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_OFF_TIME){
                            entry.getFridayLoadOffTime().setHours((byte)0);
                            entry.getFridayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x20));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_ON_TIME) {
                            entry.getSaturdayLoadOnTime().setHours((byte)0);
                            entry.getSaturdayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x40));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_OFF_TIME){
                            entry.getSaturdayLoadOffTime().setHours((byte)0);
                            entry.getSaturdayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x40));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_ON_TIME) {
                            entry.getSundayLoadOnTime().setHours((byte)0);
                            entry.getSundayLoadOnTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x01));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_OFF_TIME){
                            entry.getSundayLoadOffTime().setHours((byte)0);
                            entry.getSundayLoadOffTime().setMinutes((byte) 0);
                            entry.setEnabledTimes((byte) (entry.getEnabledTimes() & ~0x01));
                        }

                        smartPlugProvider.updateOnOffTimesEntry(entry);

                        /**
                         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                         */
                        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                    }
                    else if (frame.getRegister() == SmartPlugCommHelper.Registers.DEVICE_ID) {
                        /**
                         * Si se resetea el Device ID se debe enviar un GET(DEVICE_ID) para
                         * averiguar cuál es el nuevo ID.
                         */
                        EventBus.getDefault().post(new CommandEvent(ev.getId(),
                                SmartPlugCommHelper.getInstance().getRawData(
                                        SmartPlugCommHelper.Commands.GET,
                                        SmartPlugCommHelper.Registers.DEVICE_ID
                                )));
                    }
                    else if (frame.getRegister() == SmartPlugCommHelper.Registers.TOTAL_ENERGY) {
                        /**
                         * Si se recibió una respuesta a el comando RESET(TOTAL_ENERGY)
                         * se pone en 0 este valor en la tabla InstantaneousMeasurements
                         */
                        InstantaneousInfoEntry entry = SmartPlugProvider.getInstance(getApplicationContext())
                                .getInstantaneousInfoEntry(ev.getId());

                        entry.setTotalEnergy(0.0f);
                        entry.setLastUpdate(Calendar.getInstance().getTime());
                        entry.setTimeouts(0);
                        entry.setConnectionState(0);
                        SmartPlugProvider.getInstance(getApplicationContext())
                                .updateInstantaneousInfoEntry(entry);
                    }
                    else if (frame.getRegister() == SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER) {
                        /**
                         * Si se recibió una respuesta a el comando RESET(PER_HOUR_ACTIVE_POWER)
                         * se deben borrar todas las entradas para este tipo de medición y el ID
                         * correspondiente.
                         */
                        SmartPlugProvider.getInstance(getApplicationContext()).removeMeasurementsEntries(
                                ev.getId(), MeasurementsEntry.MeasurementType.ACTIVE_POWER);
                    }
                    else if (frame.getRegister() == SmartPlugCommHelper.Registers.PER_HOUR_ENERGY) {
                        /**
                         * Si se recibió una respuesta a el comando RESET(PER_HOUR_ENERGY)
                         * se deben borrar todas las entradas para este tipo de medición y el ID
                         * correspondiente.
                         */
                        SmartPlugProvider.getInstance(getApplicationContext()).removeMeasurementsEntries(
                                ev.getId(), MeasurementsEntry.MeasurementType.ENERGY);
                    }
                    else if (frame.getRegister() == SmartPlugCommHelper.Registers.ALL_REGISTERS) {
                        /**
                         * Si se recibió una respuesta a el comando RESET (ALL_REGISTERS) se debe:
                         *  - Pedir nombre del dispositivo.
                         *  - Pedir estado de la carga.
                         *  - Pedir horarios de encendido y apagado.
                         *  - Eliminar las entradas de mediciones históricas para energía y para potencia activa.
                         */

                        queryInitialValues(ev.getId());
                        queryWeekMeasurements(ev.getId());

                        SmartPlugProvider.getInstance(getApplicationContext()).removeMeasurementsEntries(
                                ev.getId(), MeasurementsEntry.MeasurementType.ACTIVE_POWER);

                        SmartPlugProvider.getInstance(getApplicationContext()).removeMeasurementsEntries(
                                ev.getId(), MeasurementsEntry.MeasurementType.ENERGY);
                    }
                }
            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.FLOAT_ARRAY_PARAM) {
                FloatArrayFrame frame = (FloatArrayFrame)basicFrame;

                /**
                 * Los comandos que pueden devolver este tipo de trama son los GET de los registros:
                 * CURRENT_MEASUREMENTS
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
                        entry.setTimeouts(0);
                        entry.setConnectionState(0);
                        smartPlugProvider.updateInstantaneousInfoEntry(entry);

                        /**
                         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                         */
                        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                    }
                }

            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.BYTE_FLOAT_ARRAY_PARAM) {
                ByteFloatArrayFrame frame = (ByteFloatArrayFrame) basicFrame;

                /**
                 * Los comandos que pueden devolver este tipo de trama son los GET de los registros:
                 * PER_HOUR_ACTIVE_POWER y PER_HOUR_ENERGY.
                 */

                if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET &&
                        (frame.getRegister() == SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER ||
                                frame.getRegister() == SmartPlugCommHelper.Registers.PER_HOUR_ENERGY) ) {
                    /**
                     * Si es un GET de PER_HOUR_ACTIVE_POWER o PER_HOUR_ENERGY, lo que se reciben son
                     * 3 bytes con la fecha y 24 floats con las mediciones de las 24 horas
                     *
                     * Se arma la fecha a partir de los bytes recibidos en la trama
                     */
                    if (frame.getDataBytes().length == 3 && frame.getDataFloats().length == 24) {
                        String date = String.format("%d/%d/%d", frame.getDataBytes()[0],
                                                                frame.getDataBytes()[1],
                                                                frame.getDataBytes()[2]);

                        /**
                         * Se chequea si existe la entrada en la tabla Measurements para este ID, esta fecha
                         * y este tipo de medición.
                         * Si existe, se la actualiza. Si no existe se la crea.
                         */
                        if (frame.getRegister() == SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER) {
                            if (smartPlugProvider.existMeasurementsEntry(ev.getId(), date, MeasurementsEntry.MeasurementType.ACTIVE_POWER)) {
                                MeasurementsEntry entry = smartPlugProvider.getMeasurementsEntry(ev.getId(), date, MeasurementsEntry.MeasurementType.ACTIVE_POWER);

                                entry.setMeasurements(frame.joinFloats(";"));

                                smartPlugProvider.updateMeasurementsEntry(entry);
                            }
                            else {
                                /**
                                 * Si hay que crearla, se debe chequear que no haya más de 20 entradas.
                                 * Si hay más se borra la más vieja.
                                 */
                                List<MeasurementsEntry> currentEntries = smartPlugProvider.getMeasurementsEntries(ev.getId(), MeasurementsEntry.MeasurementType.ACTIVE_POWER);

                                if (currentEntries != null && currentEntries.size() >= 20) {
                                    /**
                                     * Se borra la primera de la lista de entradas, ya que es la más antigua.
                                     */
                                    smartPlugProvider.removeMeasurementsEntry(ev.getId(),
                                            currentEntries.get(0).getDate(),
                                            MeasurementsEntry.MeasurementType.ACTIVE_POWER);
                                }

                                MeasurementsEntry entry = new MeasurementsEntry();
                                entry.setId(ev.getId());
                                entry.setDate(date);
                                entry.setMeasurementType(MeasurementsEntry.MeasurementType.ACTIVE_POWER);
                                entry.setMeasurements(frame.joinFloats(";"));

                                smartPlugProvider.addMeasurementsEntry(entry);
                            }
                        }
                        else {
                            if (smartPlugProvider.existMeasurementsEntry(ev.getId(), date, MeasurementsEntry.MeasurementType.ENERGY)) {
                                MeasurementsEntry entry = smartPlugProvider.getMeasurementsEntry(ev.getId(), date, MeasurementsEntry.MeasurementType.ENERGY);

                                entry.setMeasurements(frame.joinFloats(";"));

                                smartPlugProvider.updateMeasurementsEntry(entry);
                            }
                            else {
                                /**
                                 * Si hay que crearla, se debe chequear que no haya más de 20 entradas.
                                 * Si hay más se borra la más vieja.
                                 */
                                List<MeasurementsEntry> currentEntries = smartPlugProvider.getMeasurementsEntries(ev.getId(), MeasurementsEntry.MeasurementType.ENERGY);

                                if (currentEntries != null && currentEntries.size() >= 20) {
                                    /**
                                     * Se borra la primera de la lista de entradas, ya que es la más antigua.
                                     */
                                    smartPlugProvider.removeMeasurementsEntry(ev.getId(),
                                            currentEntries.get(0).getDate(),
                                            MeasurementsEntry.MeasurementType.ENERGY);
                                }

                                MeasurementsEntry entry = new MeasurementsEntry();
                                entry.setId(ev.getId());
                                entry.setDate(date);
                                entry.setMeasurementType(MeasurementsEntry.MeasurementType.ENERGY);
                                entry.setMeasurements(frame.joinFloats(";"));

                                smartPlugProvider.addMeasurementsEntry(entry);
                            }
                        }

                        /**
                         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                         */
                        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                    }

                }
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
                    entry.setTimeouts(0);
                    entry.setConnectionState(0);
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
                 * *_LOAD_ON_TIME, *_LOAD_OFF_TIME y ONOFF_TIMES.
                 * Además lo pueden devolver los SET de los registros: *_LOAD_ON_TIME, *_LOAD_OFF_TIME.
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
                        entry.getTuesdayLoadOnTime().setHours(frame.getData()[2]);
                        entry.getTuesdayLoadOnTime().setMinutes(frame.getData()[3]);
                        entry.getWednesdayLoadOnTime().setHours(frame.getData()[4]);
                        entry.getWednesdayLoadOnTime().setMinutes(frame.getData()[5]);
                        entry.getThursdayLoadOnTime().setHours(frame.getData()[6]);
                        entry.getThursdayLoadOnTime().setMinutes(frame.getData()[7]);
                        entry.getFridayLoadOnTime().setHours(frame.getData()[8]);
                        entry.getFridayLoadOnTime().setMinutes(frame.getData()[9]);
                        entry.getSaturdayLoadOnTime().setHours(frame.getData()[10]);
                        entry.getSaturdayLoadOnTime().setMinutes(frame.getData()[11]);
                        entry.getSundayLoadOnTime().setHours(frame.getData()[12]);
                        entry.getSundayLoadOnTime().setMinutes(frame.getData()[13]);

                        entry.getMondayLoadOffTime().setHours(frame.getData()[14]);
                        entry.getMondayLoadOffTime().setMinutes(frame.getData()[15]);
                        entry.getTuesdayLoadOffTime().setHours(frame.getData()[16]);
                        entry.getTuesdayLoadOffTime().setMinutes(frame.getData()[17]);
                        entry.getWednesdayLoadOffTime().setHours(frame.getData()[18]);
                        entry.getWednesdayLoadOffTime().setMinutes(frame.getData()[19]);
                        entry.getThursdayLoadOffTime().setHours(frame.getData()[20]);
                        entry.getThursdayLoadOffTime().setMinutes(frame.getData()[21]);
                        entry.getFridayLoadOffTime().setHours(frame.getData()[22]);
                        entry.getFridayLoadOffTime().setMinutes(frame.getData()[23]);
                        entry.getSaturdayLoadOffTime().setHours(frame.getData()[24]);
                        entry.getSaturdayLoadOffTime().setMinutes(frame.getData()[25]);
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
                else if ( (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET  || frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET) &&
                        ((frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME) ||
                        (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_OFF_TIME) ||
                        (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_OFF_TIME) ||
                        (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_OFF_TIME) ||
                        (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_OFF_TIME) ||
                        (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_OFF_TIME) ||
                        (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_ON_TIME) || (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_OFF_TIME)) ) {
                    /**
                     * Si es un GET o un SET de *_LOAD_ON_TIME, *_LOAD_OFF_TIME, la trama tiene que
                     * tener 2 bytes: horas y minutos.
                     */
                    if (frame.getData().length == 2) {
                        OnOffTimesEntry entry = smartPlugProvider.getOnOffTimesEntry(ev.getId());

                        if (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_ON_TIME) {
                            entry.getMondayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getMondayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x02));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.MONDAY_LOAD_OFF_TIME){
                            entry.getMondayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getMondayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x02));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_ON_TIME) {
                            entry.getTuesdayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getTuesdayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x04));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.TUESDAY_LOAD_OFF_TIME){
                            entry.getTuesdayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getTuesdayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x04));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_ON_TIME) {
                            entry.getWednesdayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getWednesdayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x08));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.WEDNESDAY_LOAD_OFF_TIME){
                            entry.getWednesdayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getWednesdayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x08));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_ON_TIME) {
                            entry.getThursdayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getThursdayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x10));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.THURSDAY_LOAD_OFF_TIME){
                            entry.getThursdayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getThursdayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x10));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_ON_TIME) {
                            entry.getFridayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getFridayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x20));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.FRIDAY_LOAD_OFF_TIME){
                            entry.getFridayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getFridayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x20));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_ON_TIME) {
                            entry.getSaturdayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getSaturdayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x40));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SATURDAY_LOAD_OFF_TIME){
                            entry.getSaturdayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getSaturdayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x40));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_ON_TIME) {
                            entry.getSundayLoadOnTime().setHours(frame.getData()[0]);
                            entry.getSundayLoadOnTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x01));
                        }
                        else if (frame.getRegister() == SmartPlugCommHelper.Registers.SUNDAY_LOAD_OFF_TIME){
                            entry.getSundayLoadOffTime().setHours(frame.getData()[0]);
                            entry.getSundayLoadOffTime().setMinutes(frame.getData()[1]);

                            if (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET)
                                entry.setEnabledTimes((byte) (entry.getEnabledTimes() | 0x01));
                        }

                        smartPlugProvider.updateOnOffTimesEntry(entry);

                        /**
                         * Se postea una instancia del evento UpdateSmartPlugEvent indicando que se modificó una entrada.
                         */
                        EventBus.getDefault().post(new UpdateSmartPlugEvent(ev.getId()));
                    }
                }
            }
            else if (basicFrame.getFrameType() == BasicFrame.Types.STRING_PARAM) {
                StringFrame frame = (StringFrame)basicFrame;

                /**
                 * El comando que puede devolver este tipo de trama es el GET(DEVICE_ID) o SET(DEVICE_ID)
                 */
                if ( (frame.getCommand() == SmartPlugCommHelper.Commands.RESP_GET || frame.getCommand() == SmartPlugCommHelper.Commands.RESP_SET) &&
                        frame.getRegister() == SmartPlugCommHelper.Registers.DEVICE_ID) {
                    /**
                     * Si es un GET(DEVICE_ID) la trama tiene que tener un String de 33 caracteres
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


    public static void queryWeekMeasurements (String id) {
        /**
         * Se le van a pedir las mediciones históricas de los últimos 7 días tanto de potencia
         * como de enegía.
         *
         * Se obtiene la fecha actual
         */
        Calendar calendar = Calendar.getInstance();
        byte day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        byte month = (byte) (calendar.get(Calendar.MONTH) + 1);
        byte year = (byte) (calendar.get(Calendar.YEAR) - 2000);

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));

        /**
         * Se pregunta por las mediciones del día anterior
         */
        calendar.add(Calendar.DAY_OF_YEAR, -1);
        day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        month = (byte) (calendar.get(Calendar.MONTH) + 1);
        year = (byte) (calendar.get(Calendar.YEAR) - 2000);


        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));

        /**
         * Se pregunta por las mediciones del día anterior
         */
        calendar.add(Calendar.DAY_OF_YEAR, -1);
        day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        month = (byte) (calendar.get(Calendar.MONTH) + 1);
        year = (byte) (calendar.get(Calendar.YEAR) - 2000);


        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));

        /**
         * Se pregunta por las mediciones del día anterior
         */
        calendar.add(Calendar.DAY_OF_YEAR, -1);
        day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        month = (byte) (calendar.get(Calendar.MONTH) + 1);
        year = (byte) (calendar.get(Calendar.YEAR) - 2000);


        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));

        /**
         * Se pregunta por las mediciones del día anterior
         */
        calendar.add(Calendar.DAY_OF_YEAR, -1);
        day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        month = (byte) (calendar.get(Calendar.MONTH) + 1);
        year = (byte) (calendar.get(Calendar.YEAR) - 2000);


        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));

        /**
         * Se pregunta por las mediciones del día anterior
         */
        calendar.add(Calendar.DAY_OF_YEAR, -1);
        day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        month = (byte) (calendar.get(Calendar.MONTH) + 1);
        year = (byte) (calendar.get(Calendar.YEAR) - 2000);


        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));

        /**
         * Se pregunta por las mediciones del día anterior
         */
        calendar.add(Calendar.DAY_OF_YEAR, -1);
        day = (byte) calendar.get(Calendar.DAY_OF_MONTH);
        month = (byte) (calendar.get(Calendar.MONTH) + 1);
        year = (byte) (calendar.get(Calendar.YEAR) - 2000);


        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ACTIVE_POWER,
                        new byte[]{day, month, year}
                )
        ));

        EventBus.getDefault().post(new CommandEvent(id, SmartPlugCommHelper.getInstance().
                getRawData(SmartPlugCommHelper.Commands.GET,
                        SmartPlugCommHelper.Registers.PER_HOUR_ENERGY,
                        new byte[]{day, month, year}
                )
        ));
    }

    public static void queryInitialValues (String id) {
        byte[] data;

        /**
         * Se pide el nombre del Smart Plug para ver si cambió.
         */
        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                SmartPlugCommHelper.Registers.DEVICE_ID);
        EventBus.getDefault().post(new CommandEvent(id, data));


        /**
         * Se piden todos los horarios de encendido y apagado para ver si alguno cambió.
         */
        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                SmartPlugCommHelper.Registers.ONOFF_TIMES);
        EventBus.getDefault().post(new CommandEvent(id, data));


        /**
         * Se piden las mediciones de tensión, corriente, potencia activa y energía total acumulada.
         */
        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                SmartPlugCommHelper.Registers.CURRENT_MEASUREMENTS);
        EventBus.getDefault().post(new CommandEvent(id, data));


        /**
         * Se pide si está encendida la carga o no.
         */
        data = SmartPlugCommHelper.getInstance().getRawData(SmartPlugCommHelper.Commands.GET,
                SmartPlugCommHelper.Registers.LOAD_STATE);
        EventBus.getDefault().post(new CommandEvent(id, data));
    }


    private void cleanup () {
        if (mMulticastLock.isHeld())
            mMulticastLock.release();

        if (mWakeLock.isHeld())
            mWakeLock.release();

        EventBus.getDefault().unregister(this);

        sRunning = false;
    }
}
