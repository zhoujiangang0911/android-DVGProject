package com.wislight.vehicledb.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteOpenHelper;

public class DGVDBHelper extends SQLiteOpenHelper {
	private static String dbName = "DGV.db" ;
	private static int tableVersion =1;
	
	
	public DGVDBHelper(Context context) {
		super(context, dbName, null, tableVersion);
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		//id INTEGER PRIMARY KEY AUTOINCREMENT ,自增长
		//传感数据表tb_sensor 此表用来存储从采集子板传送上来的各种传感数据
		String sqltb_sensor = "CREATE TABLE tb_sensor( oem_id INTEGER,version INTEGER,dev_type" +
				" INTEGER,cmd INTEGER,uuid INTEGER ,date DATE,latitude FLOAT,longitude FLOAT," +
				"elevation FLOAT,speed FLOAT,angle FLOAT,sensor_id INTEGER," +
				"value_type INTEGER,value BLOB) ";
		//警告数据表tb_alar 此表用来存储从采集子板或EDR的各种警告数据
		String sqltb_alarm = "CREATE TABLE tb_alarm(oem_id INTEGER,version INTEGER,dev_type INTEGER" +
				",cmd INTEGER,uuid INTEGER ,date DATE,latitude FLOAT,longitude FLOAT," +
				"elevation FLOAT,speed FLOAT,angle FLOAT,sensor_id INTEGER," +
				"value_type INTEGER,value BLOB) ";
		//终端用户操作数据表tb_usr_act 此表用来记录用户的操作及数据
		String sqltb_usr_act = "CREATE TABLE tb_usr_act(sn CHAR(32),name VARCHAR(64),date DATE,uuid INTEGER," +
				"type INTEGER,value_type INTEGER,value BLOB)";
		//后台服务下发配置表tb_svr_cfg  此表用来记录后台服务器下发的配置信息
		String sqltb_svr_cfg = "CREATE TABLE tb_svr_cfg(cfg_xml TEXT,type INTEGER,date DATE)";
		//传感映射数据表tb_sensor_map 此表用来记录传感过程中功能标识ID与功能量编号的对应关系
		String sqltb_sensor_map = "CREATE TABLE tb_sensor_map(id INTEGER, data_type INTEGER,type_id INTEGER,map_name VARCHAR(64))";
		//警告映射数据表tb_alarm_map 此表用来记录警告标识ID与警告量编号的对应关系
		String sqltb_alarm_map = "CREATE TABLE tb_alarm_map(id INTEGER,type_id INTEGER,map_name VARCHAR(64))";
		db.execSQL(sqltb_alarm_map);
		db.execSQL(sqltb_sensor_map);
		db.execSQL(sqltb_svr_cfg);
		db.execSQL(sqltb_sensor);
		db.execSQL(sqltb_alarm);
		db.execSQL(sqltb_usr_act);
	}

	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
			
		
	}

}
