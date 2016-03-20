package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import org.etsi.ttcn.tri.TriParameterList;
import org.etsi.ttcn.tri.TriPortId;
import org.etsi.ttcn.tri.TriStatus;

import com.testingtech.ttcn.tri.TriStatusImpl;

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

	public TriStatus map(TriPortId compPortId, TriPortId tsiPortId, TriParameterList paramList) {
		return TriStatusImpl.OK;
	}
	public TriStatus unmap() {
		return TriStatusImpl.OK;
	}
}
