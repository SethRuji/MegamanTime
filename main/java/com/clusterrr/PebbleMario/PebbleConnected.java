package com.clusterrr.PebbleMario;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

/**
 * Created by Cluster on 11.11.2014.
 */
public class PebbleConnected extends BroadcastReceiver
{
    @Override
    public void onReceive(Context context, Intent intent)
    {
        Intent batteryIntent = context.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        int level = batteryIntent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryIntent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
        long batteryLevel = Math.round(10.0 * (float) level / (float)scale);

        PebbleDictionary dictionary = new PebbleDictionary();
        dictionary.addInt8(PebbleReceiver.MSG_BATTERY_ANSWER, (byte) batteryLevel);
        PebbleKit.sendDataToPebble(context, PebbleReceiver.PEBBLE_APP_UUID, dictionary);
    }
}
