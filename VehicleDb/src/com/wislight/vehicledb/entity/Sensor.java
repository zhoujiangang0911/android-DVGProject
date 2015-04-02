package com.wislight.vehicledb.entity;

import java.io.Serializable;
import java.sql.Blob;
import java.sql.Date;

import android.provider.ContactsContract.Contacts.Data;


public class Sensor implements Serializable{
	 
	private int version; 
	private int oemid; 
	private int devtype; 
	private int cmd; 
	private int uuid; 
	private Date  date;
	private float latitude; 
	private float longitude; 
	private float elevation; 
	private float speed; 
	private float angle; 
	private int sensorid; 
	private int valuetype; 
	private Blob value;
	
	
	
	
	public Sensor() {
		super();
	}
	public Sensor(int version, int oemid, int devtype, int cmd, int uuid,
			Date time, float d, float e, float elevation,
			float speed, float angle, int sensorid, int valuetype, int i) {
		super();
		this.version = version;
		this.oemid = oemid;
		this.devtype = devtype;
		this.cmd = cmd;
		this.uuid = uuid;
		this.date = time;
		this.latitude = d;
		this.longitude = e;
		this.elevation = elevation;
		this.speed = speed;
		this.angle = angle;
		this.sensorid = sensorid;
		this.valuetype = valuetype;
	}
	public int getVersion() {
		return version;
	}
	public void setVersion(int version) {
		this.version = version;
	}
	public int getOemid() {
		return oemid;
	}
	public void setOemid(int oemid) {
		this.oemid = oemid;
	}
	public int getDevtype() {
		return devtype;
	}
	public void setDevtype(int devtype) {
		this.devtype = devtype;
	}
	public int getCmd() {
		return cmd;
	}
	public void setCmd(int cmd) {
		this.cmd = cmd;
	}
	public int getUuid() {
		return uuid;
	}
	public void setUuid(int uuid) {
		this.uuid = uuid;
	}
	public Date getDate() {
		return date;
	}
	public void setDate(Date date) {
		this.date = date;
	}
	public float getLatitude() {
		return latitude;
	}
	public void setLatitude(float latitude) {
		this.latitude = latitude;
	}
	public float getLongitude() {
		return longitude;
	}
	public void setLongitude(float longitude) {
		this.longitude = longitude;
	}
	public float getElevation() {
		return elevation;
	}
	public void setElevation(float elevation) {
		this.elevation = elevation;
	}
	public float getSpeed() {
		return speed;
	}
	public void setSpeed(float speed) {
		this.speed = speed;
	}
	public float getAngle() {
		return angle;
	}
	public void setAngle(float angle) {
		this.angle = angle;
	}
	public int getSensorid() {
		return sensorid;
	}
	public void setSensorid(int sensorid) {
		this.sensorid = sensorid;
	}
	public int getValuetype() {
		return valuetype;
	}
	public void setValuetype(int valuetype) {
		this.valuetype = valuetype;
	}
	public Blob getValue() {
		return value;
	}
	public void setValue(Blob value) {
		this.value = value;
	}
	
	
	
	
}
