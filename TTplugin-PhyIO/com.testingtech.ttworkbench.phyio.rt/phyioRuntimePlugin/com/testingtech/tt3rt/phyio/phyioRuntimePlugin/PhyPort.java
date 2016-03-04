package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

public class PhyPort {
	private PhyPortKind kind;
	private int deviceID;
	private int sensorID;
	
	public PhyPort(int deviceID, int sensorID, PhyPortKind kind) {
		this.deviceID = deviceID;
		this.sensorID = sensorID;
		this.kind = kind;
	}

	public int getDeviceID() {
		return deviceID;
	}

	public int getSensorID() {
		return sensorID;
	}

	public PhyPortKind getKind() {
		return kind;
	}

	@Override
	public String toString() {
		return "PhyPort [kind=" + kind + ", deviceID=" + deviceID + ", sensorID=" + sensorID + "]";
	}
}
