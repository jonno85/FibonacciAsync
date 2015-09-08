package com.fib;

import com.cnh.fib.R;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class FibActivity extends Activity {
	private Handler mhandler = new Handler();
	private Button bCalcAsync = null;
	private Button bCalc = null;
	private EditText eNumber = null;
	private TextView tResult = null;
	private static TextView tResultasync = null;
	
	static {
		System.loadLibrary("fib");
	}
	
	public native long fibN(long n);
	
	public native boolean asyncFibN(long n);
	
	void callback(int result){
		final int res = result;
		System.out.println("result: "+res);
		final int r = result;
		mhandler.post(new Runnable() {
			
			@Override
			public void run() {
				tResultasync.setText("Result Async: " + r);
			}
		});	
	}

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bCalcAsync = (Button)findViewById(R.id.ButtonAsync);
        bCalc = (Button)findViewById(R.id.button1);
		eNumber = (EditText)findViewById(R.id.editText1);
		tResultasync = (TextView)findViewById(R.id.textView1);
		tResult = (TextView)findViewById(R.id.TextView02);
		
		bCalcAsync.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				long number = Integer.parseInt(eNumber.getText().toString());
				boolean result = asyncFibN(number);
				Log.w("Activity", "Async Method called " + result);
			}
		});
		
		bCalc.setOnClickListener(new OnClickListener() {
			
			@Override
			public void onClick(View v) {
				long number = Integer.parseInt(eNumber.getText().toString());
				long res = fibN(number);
				tResult.setText("Result: "+res);
				
			}
		});
    }
}