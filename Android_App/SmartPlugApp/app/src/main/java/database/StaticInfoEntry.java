package database;


import com.company.smartplugapp.R;

public class StaticInfoEntry {
    private String mId;
    private String mName;
    private int mIconId;

    public StaticInfoEntry (String id, String name, int iconId) {
        mId = id;
        mName = name;
        mIconId = iconId;
    }

    public StaticInfoEntry () {
        mId = null;
        mName = "Sin nombre";
        mIconId = R.drawable.device_tv;
    }


    public String getId() {
        return mId;
    }

    public void setId(String id) {
        mId = id;
    }

    public String getName() {
        return mName;
    }

    public void setName(String name) {
        mName = name;
    }

    public int getIconId() {
        return mIconId;
    }

    public void setIconId(int iconId) {
        mIconId = iconId;
    }
}
