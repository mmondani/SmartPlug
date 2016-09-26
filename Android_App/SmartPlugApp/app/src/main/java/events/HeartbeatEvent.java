package events;

import java.util.Date;

/**
 * Clase usada por el EventBus para indicar que llegó un heartbeat desde la IP mIp del
 * Smart Plug mId con fecha mDate.
 */
public class HeartbeatEvent {
    private String mId;
    private String mIp;
    private Date mDate;

    /**
     * Constructor de la clase HeartbeatEvent
     * @param id id del Smart Plug que generó el heartbeat.
     * @param ip dirección IP del Smart Plug que generó el hearbeat.
     * @param date fecha y hora en la que se recibió el heartbeat.
     */
    public HeartbeatEvent (String id, String ip, Date date) {
        mId = id;
        mIp = ip;
        mDate = date;
    }

    /**
     * Getter del ID del Smart Plug que generó el heartbeat.
     * @return ID del Smart Plug.
     */
    public String getId() {
        return mId;
    }

    /**
     * Setter del ID del Smart Plug
     * @param id ID del Smart Plug.
     */
    public void setId(String id) {
        mId = id;
    }

    /**
     * Getter de la IP del Smart Plug.
     * @return IP del Smart Plug.
     */
    public String getIp() {
        return mIp;
    }

    /**
     * Setter de la IP del Smart Plug.
     * @param ip IP del Smart Plug.
     */
    public void setIp(String ip) {
        mIp = ip;
    }

    /**
     * Getter de la fecha en la que llegó el heartbeat .
     * @return fecha y hora en la que llegó el heartbeat.
     */
    public Date getDate() {
        return mDate;
    }

    /**
     * Setter de la fecha en la que llegó el heartbeat.
     * @param date fecha y hora en la que llegó el heartbeat.
     */
    public void setDate(Date date) {
        mDate = date;
    }
}
