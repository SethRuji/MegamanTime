package com.clusterrr.PebbleMario;

import android.app.Activity;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.os.Bundle;
import android.view.Menu;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Intent batteryIntent = registerReceiver(null, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
        int level = batteryIntent.getIntExtra(BatteryManager.EXTRA_LEVEL, -1);
        int scale = batteryIntent.getIntExtra(BatteryManager.EXTRA_SCALE, -1);
        long batteryLevel = Math.round(10.0 * (float) level / (float) scale);

        PebbleDictionary dictionary = new PebbleDictionary();
        dictionary.addInt8(PebbleReceiver.MSG_BATTERY_ANSWER, (byte) batteryLevel);
        PebbleKit.sendDataToPebble(this, PebbleReceiver.PEBBLE_APP_UUID, dictionary);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return false;
    }
}
