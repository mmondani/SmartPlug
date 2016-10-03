package events;

/**
 * Created by mariano on 03/10/16.
 */
public class WiFiStateEvent {
    private boolean mWiFiState;

    public WiFiStateEvent (boolean state) {
        mWiFiState = state;
    }

    public boolean isWiFiState() {
        return mWiFiState;
    }

    public void setWiFiState(boolean wiFiState) {
        mWiFiState = wiFiState;
    }
}
