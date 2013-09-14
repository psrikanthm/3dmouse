package com.example.gesturecontrol;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import android.os.AsyncTask;

public class ClientConn extends AsyncTask<String, Void, String>{

	Socket clientSocket = null;
	BufferedOutputStream out = null;
	String ip_add ;
	int port_add ;
	
	public ClientConn(final String ip_addr, final int port){
		System.out.println("Inside client socket connect");
		ip_add = ip_addr;
		port_add = port ;
	}
	
	protected String doInBackground(String... params) {
		// TODO Auto-generated method stub
		try {
			clientSocket = new Socket(ip_add, port_add);
			out = new BufferedOutputStream(clientSocket.getOutputStream());
			byte[] m_txt = params[0].getBytes();
			out.write(m_txt, 0, m_txt.length);
			out.flush();
		}catch (Exception e) {
			System.err.println("Couldn't get I/O for the connection");
			e.printStackTrace();
			System.exit(1);
		}
		return null;
	}
}
