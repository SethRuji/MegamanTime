package com.hedgehogs265.PebbleMegaman;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.BatteryManager;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

/**
 * Created by Cluster on 11.11.2014.
 * Credit to Clusterrr for his source code for the android portion of this watchface. (https://github.com/ClusterM) 
 */
public class BatteryLevelReceiver extends BroadcastReceiver
{
    @Override
    public void onReceive(Context context, Intent intent)
    {
        int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = intent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
        long batteryLevel = Math.round(10.0 * (float) level / (float)scale);

        PebbleDictionary dictionary = new PebbleDictionary();
        dictionary.addInt32(PebbleReceiver.MSG_BATTERY_ANSWER, (int)batteryLevel);
        PebbleKit.sendDataToPebble(context, PebbleReceiver.PEBBLE_APP_UUID, dictionary);
    }
}
