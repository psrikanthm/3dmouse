package com.example.gesturecontrol;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity implements SensorEventListener{

	Button send;
	TextView display;
	EditText etIp, etPort;
	int port=0;
	String ip_address;
	SensorManager sm;
	float gravity[] = {0.0f, 0.0f, 0.0f};
	float accel_prev[] = {0.0f, 0.0f, 0.0f};

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		send = (Button) findViewById(R.id.bSend);
		etIp = (EditText)findViewById(R.id.etIP);
		etPort = (EditText)findViewById(R.id.etPort);
		display = (TextView) findViewById(R.id.tvDisplay);

		WifiManager wifiManager = (WifiManager) getSystemService(WIFI_SERVICE);
		WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		int ip = wifiInfo.getIpAddress();
		
		String ipString = String.format("%d.%d.%d.%d", (ip & 0xff),(ip >> 8 & 0xff),(ip >> 16 & 0xff),(ip >> 24 & 0xff));
		display.setText("IP: " + ipString);

		sm = (SensorManager) getSystemService(Context.SENSOR_SERVICE);

		send.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				ip_address=etIp.getText().toString();
				String port_str= etPort.getText().toString();
				port = Integer.parseInt(port_str);
	//			conn.makeConnection(ip_address, port);
			}
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub

	}
	protected void onResume() {
		// register this class as a listener for the orientation and
		// accelerometer sensors
		super.onResume();
		sm.registerListener(this, sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),SensorManager.SENSOR_DELAY_NORMAL);
	}

	@Override
	protected void onPause() {
		// unregister listener
		super.onPause();
		sm.unregisterListener(this);
	}

	@Override
	public void onSensorChanged(SensorEvent event) {
		// TODO Auto-generated method stub
		if(event.sensor.getType() == Sensor.TYPE_ACCELEROMETER){
			final float alpha = 0.99f ;
			final float beta = 1.0f ;
			float linear_acceleration[] = {0.0f, 0.0f, 0.0f};

			// Isolate the force of gravity with the low-pass filter.
			gravity[0] = alpha * gravity[0] + (1 - alpha) * event.values[0];
			gravity[1] = alpha * gravity[1] + (1 - alpha) * event.values[1];
			gravity[2] = alpha * gravity[2] + (1 - alpha) * event.values[2];

			// Remove the gravity contribution with the high-pass filter.
			linear_acceleration[0] = event.values[0] - gravity[0];
			linear_acceleration[1] = event.values[1] - gravity[1];
			linear_acceleration[2] = event.values[2] - gravity[2];
		
			//Exponential Smoothening with Low Pass Filter
			linear_acceleration[0] = accel_prev[0] + beta * (linear_acceleration[0] - accel_prev[0]);
			linear_acceleration[1] = accel_prev[1] + beta * (linear_acceleration[1] - accel_prev[1]);
			linear_acceleration[2] = accel_prev[2] + beta * (linear_acceleration[2] - accel_prev[2]);
			
			accel_prev[0] = linear_acceleration[0];
			accel_prev[1] = linear_acceleration[1];
			accel_prev[2] = linear_acceleration[2];

			String sendMessage = linear_acceleration[0] + "," + linear_acceleration[1] + "," + linear_acceleration[2] ;
			display.setText(sendMessage);
			if(port!=0)
				new ClientConn(ip_address, port).execute(sendMessage);

		}
	}
}