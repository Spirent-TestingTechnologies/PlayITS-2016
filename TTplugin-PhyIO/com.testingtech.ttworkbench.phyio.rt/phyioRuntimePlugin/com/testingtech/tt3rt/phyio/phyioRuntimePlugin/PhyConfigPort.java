package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import org.etsi.ttcn.tri.TriParameterList;
import org.etsi.ttcn.tri.TriPortId;
import org.etsi.ttcn.tri.TriStatus;

import com.testingtech.ttcn.tri.TriStatusImpl;

public class PhyConfigPort extends PhyPort {

	private PhyIOPort phyIOPort;
	private Object connectingObject = new Object();

	public PhyConfigPort(PhyIOPort phyIOPort, int deviceID, int sensorID, PhyPortKind kind) {
		super(deviceID, sensorID, kind);
		this.phyIOPort = phyIOPort;
	}

	private boolean connectedCOMPort = false;
	private TriPortId connectedTsiPortId;
	private TriPortId connectedCompPortId;

	public TriStatus map(TriPortId compPortId, TriPortId tsiPortId, TriParameterList paramList) {
		if (!connectedCOMPort) {
			synchronized (connectingObject) {
				connectedCompPortId= compPortId;
				connectedTsiPortId = tsiPortId;
				TriStatus mapResult = phyIOPort.triMapParamRS232(compPortId, tsiPortId, paramList);
				if (mapResult.getStatus() == TriStatus.TRI_ERROR) {
					return new TriStatusImpl("Error mapping COM port: " + mapResult);
				}
				connectedCOMPort = true;
			}
		}
		return TriStatusImpl.OK;
	}

	
	public TriStatus unmap() {
		synchronized (connectingObject) {
			if (!connectedCOMPort) {
				return TriStatusImpl.OK;
			}
			// disconnect from COM port
			connectedCOMPort = false;
		}
		return phyIOPort.triUnmap(connectedCompPortId, connectedTsiPortId);
	}

	public TriPortId getConnectedCompPortId() {
		return connectedCompPortId;
	}
	
	public TriPortId getConnectedTsiPortId() {
		return connectedTsiPortId;
	}
	
	public boolean isConnectedCOMPort() {
		synchronized (connectingObject) {
			return connectedCOMPort;
		}
	}

	@Override
	public String toString() {
		return "PhyConfigPort [connectedCOMPort=" + connectedCOMPort
				+ ", connectedTsiPortId=" + connectedTsiPortId
				+ ", connectedCompPortId=" + connectedCompPortId + ", deviceID=" + getDeviceID() + "] ";
	}
}
