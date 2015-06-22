package com.clusterrr.PebbleMario;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

import java.util.UUID;

public class PebbleReceiver extends PebbleKit.PebbleDataReceiver
{
    public static final UUID PEBBLE_APP_UUID = UUID.fromString("43caa750-2896-4f46-94dc-1adbd4bc1ff3");
    public static final int MSG_BATTERY_REQUEST = 4;
    public static final int MSG_BATTERY_ANSWER = 5;

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
            dictionary.addInt8(MSG_BATTERY_ANSWER, (byte) batteryLevel);
            PebbleKit.sendDataToPebble(context, PebbleReceiver.PEBBLE_APP_UUID, dictionary);
        }
    }
}
