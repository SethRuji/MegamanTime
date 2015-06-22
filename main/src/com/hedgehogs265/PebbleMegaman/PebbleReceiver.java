package com.hedgehogs265.PebbleMegaman;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.UUID;

/**
 * Created by Cluster on 11.11.2014.
 * Credit to Clusterrr for his source code for the android portion of this watchface. (https://github.com/ClusterM) 
 */

public class PebbleReceiver extends PebbleKit.PebbleDataReceiver
{
    public static final UUID PEBBLE_APP_UUID = UUID.fromString("1d2d41dc-c2e8-40bc-9678-9baf5354c6a7");
    public static final int MSG_BATTERY_REQUEST = 3;
    public static final int MSG_BATTERY_ANSWER = 4;

    public PebbleReceiver()
    {
        super(PEBBLE_APP_UUID);
    }

    @Override
    public void receiveData(Context context, int transactionId, PebbleDictionary pebbleTuples)
    {
        PebbleKit.sendAckToPebble(context, transactionId);
        if (pebbleTuples.contains(MSG_BATTERY_REQUEST)) {
            Intent batteryIntent = context.registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
            int level = batteryIntent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
            int scale = batteryIntent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
            long batteryLevel = Math.round(10.0 * (float) level / (float)scale);

            PebbleDictionary dictionary = new PebbleDictionary();
            dictionary.addInt32(MSG_BATTERY_ANSWER, (int) batteryLevel);
            PebbleKit.sendDataToPebble(context, PebbleReceiver.PEBBLE_APP_UUID, dictionary);
        }
    }
}
