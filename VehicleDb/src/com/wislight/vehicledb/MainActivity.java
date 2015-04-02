package com.wislight.vehicledb;


import java.sql.Date;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.wislight.vehicledb.db.DGVDBHelper;
import com.wislight.vehicledb.entity.Sensor;

import android.app.Activity;
import android.content.ContentValues;
import android.content.Intent;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.provider.Settings.System;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity implements OnClickListener{
	private TextView textView;
	private DGVDBHelper dgvdbHelper;
	private EditText editTextVersion;
	private EditText editTextOemId;
	private EditText editTextDeyType;
	private EditText editTextCmd;
	private EditText editTextUUID;
	private EditText editTextDate;
	private EditText editTextLatitude;
	private EditText editTextLongitude;
	private EditText editTextElevation;
	private EditText editTextSpeed;
	private EditText editTextAngle;
	private EditText editTextSensorId;
	private EditText editTextValueType;
	private EditText editTextVelue;
	private Button btnInsert;
	private Button btnSelect;
	private Button btnUpdate;
	private Button btnDelete;
	private Uri uri ;
	private Sensor sensor;
	private Date date;
	private Map<String, String> map;
	private List<Map<String, String>> lists = new ArrayList<Map<String,String>>();
	
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
//		textView = (TextView) findViewById(R.id.text);
//		ActivityManager manager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
//		textView.setText(manager.getLargeMemoryClass()+"");
		dgvdbHelper = new DGVDBHelper(this);
		date = new Date(java.lang.System.currentTimeMillis());
		Log.i("-----",date+"" );
		initView();
		long time=java.lang.System.currentTimeMillis();//long now = android.os.SystemClock.uptimeMillis();  
        SimpleDateFormat format=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");  
        Date d1=new Date(time);  
        String t1=format.format(d1);  
        Log.e("msg", t1);  
	}
	
	
	
	
	
	private void initView() {
		editTextVersion = (EditText) findViewById(R.id.tv_version);
		editTextOemId = (EditText) findViewById(R.id.tv_oem_id);
		editTextDeyType = (EditText) findViewById(R.id.tv_dev_type);
		editTextCmd = (EditText) findViewById(R.id.tv_cmd);
		editTextUUID = (EditText) findViewById(R.id.tv_uuid);
		editTextDate = (EditText) findViewById(R.id.tv_date);
		editTextLatitude = (EditText) findViewById(R.id.tv_latitude);
		editTextLongitude = (EditText) findViewById(R.id.tv_longitude);
		editTextElevation = (EditText) findViewById(R.id.tv_elevation);
		editTextSpeed = (EditText) findViewById(R.id.tv_speed);
		editTextAngle = (EditText) findViewById(R.id.tv_angle);
		editTextSensorId = (EditText) findViewById(R.id.tv_sensor_id);
		editTextValueType = (EditText) findViewById(R.id.tv_value_type);
		editTextVelue = (EditText) findViewById(R.id.tv_value);
		btnInsert = (Button) findViewById(R.id.btn_insert);
		btnSelect = (Button) findViewById(R.id.btn_select);
		btnDelete= (Button) findViewById(R.id.btn_delete);
		btnUpdate = (Button) findViewById(R.id.btn_update);
		btnDelete.setOnClickListener(this);
		btnUpdate.setOnClickListener(this);
		btnInsert.setOnClickListener(this);
		btnSelect.setOnClickListener(this);
		
	}





	@Override
	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.btn_insert:
			sensor = new Sensor(2, 2, 3, 4, 5, date, 2.0f, 23.6f, 122, 45, 45, 44, 45, 45);
			SQLiteDatabase db = dgvdbHelper.getReadableDatabase();
			String insertsql = "insert into tb_sensor(version,oem_id,dev_type,cmd,uuid,date" +
					",latitude,longitude,elevation,speed,angle,sensor_id,value_type" +
					") values(?,?,?,?,?,?,?,?,?,?,?,?,?)";
			db.execSQL(insertsql, new Object[]{sensor.getSensorid(),
			sensor.getOemid(),sensor.getDevtype(),sensor.getCmd(),sensor.getUuid(),sensor.getDate(),
			sensor.getLatitude(),sensor.getLongitude(),sensor.getElevation(),sensor.getSpeed(),sensor.getAngle(),
			sensor.getSensorid(),sensor.getValuetype()
			});
			break;
		case R.id.btn_select:
			Sensor sensor = new Sensor();
			SQLiteDatabase db2= dgvdbHelper.getReadableDatabase();
			Cursor cursor = db2.rawQuery("select * from tb_sensor", null);
			while (cursor.moveToNext()) {
				map = new HashMap<String, String>();
			int Version = cursor.getInt(0); //获取第一列的值,第一列的索引从0开始
			int  OemId  = cursor.getInt(1);//获取第二列的值
			int DeyType = cursor.getInt(2);//获取第三列的值
			map.put("Version", cursor.getInt(0)+"");
			map.put("OemId", cursor.getInt(1)+"");
			map.put("Version", cursor.getInt(2)+"");
			map.put("DeyType", cursor.getInt(3)+"");
			map.put("Version", cursor.getInt(4)+"");
			map.put("Version", cursor.getInt(5)+"");
			map.put("Version", cursor.getInt(6)+"");
			map.put("Version", cursor.getInt(7)+"");
			lists.add(map);
			Log.i("--zhoujg77","--"+Version );
			Log.i("zhoujg77","--"+OemId );
			Log.i("zhoujg77","--"+DeyType );
			}
			cursor.close();
			db2.close(); 
			
			break;
		case R.id.btn_update:
			SQLiteDatabase dbup = dgvdbHelper.getWritableDatabase();
			ContentValues values = new ContentValues();
			values.put("Version", 5464654);//key为字段名，value为值
			dbup.update("tb_sensor", values, "oem_id=?", new String[]{"2"});
			dbup.close();
			
			break;
		case R.id.btn_delete:
			SQLiteDatabase dbdelete = dgvdbHelper.getWritableDatabase();
			dbdelete.delete("tb_sensor", "cmd=?", new String[]{"4"});
			dbdelete.close();
			break;
		default:
			break;
		}
		
		
	}
	/**
	 *		 SQLiteDatabase db = ....;
		db.beginTransaction();//开始事务
		try {
			db.execSQL("insert into person(name, age) values(?,?)", new Object[]{"林计钦", 4});
			db.execSQL("update person set name=? where personid=?", new Object[]{"abc", 1});
			db.setTransactionSuccessful();//调用此方法会在执行到endTransaction() 时提交当前事务，如果不调用此方法会回滚事务
		} finally {
			db.endTransaction();//由事务的标志决定是提交事务，还是回滚事务
		}
		db.close(); 
	 * 
	 * 
	 */
	
	public String getRealPathFromURI(Uri contentUri) {
	    String res = null;
	    String[] proj = { MediaStore.Images.Media.DATA };
	    Cursor cursor = getContentResolver().query(contentUri, proj, null, null, null);
	    if(cursor.moveToFirst()){;
	       int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
	       res = cursor.getString(column_index);
	    }
	    cursor.close();
	    return res;
	}

	
}
