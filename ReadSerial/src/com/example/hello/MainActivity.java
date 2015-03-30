package com.example.hello;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;
public class MainActivity extends Activity {

	 
	
	
	protected void onCreate(Bundle savedInstanceState) {
		 super.onCreate(savedInstanceState);

	        /* Create a TextView and set its content.
	         * the text is retrieved by calling a native
	         * function.
	         */
	        TextView  tv = new TextView(this);
	        tv.setText( stringFromJNI("/dev/ttyPCH0"));
	        setContentView(tv);
	        
	}
	public native String  stringFromJNI(String s);
	public native String  unimplementedStringFromJNI();
	static 
    {
	        System.loadLibrary("Hello");
    }

}
