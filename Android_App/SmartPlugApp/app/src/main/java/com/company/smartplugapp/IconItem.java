package com.company.smartplugapp;

import java.util.UUID;

/**
 * Created by mariano on 29/09/16.
 */
public class IconItem {
    private UUID mId;
    private int mResId;

    public IconItem (int resId) {
        mId = UUID.randomUUID();
        mResId = resId;
    }

    public int getId() {
        return mId.hashCode();
    }


    public int getResId() {
        return mResId;
    }

    public void setResId(int resId) {
        mResId = resId;
    }



    public static IconItem[] IconItems = {
            new IconItem(R.drawable.device_tv),
            new IconItem(R.drawable.device_bulb),
            new IconItem(R.drawable.device_computer),
            new IconItem(R.drawable.device_router),
            new IconItem(R.drawable.device_coffee_maker),
            new IconItem(R.drawable.device_fan),
            new IconItem(R.drawable.device_heater),
            new IconItem(R.drawable.device_air_conditioning)
    };

    public static IconItem getItem (int id) {
        for (IconItem item : IconItems) {
            if (item.getId() == id)
                return item;
        }

        return null;
    }

    public static int getCount () {
        return IconItems.length;
    }
}
