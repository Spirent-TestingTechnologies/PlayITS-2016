package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.text.MessageFormat;
import java.util.Collection;
import java.util.Hashtable;
import java.util.Map;

import org.etsi.ttcn.tci.IntegerValue;
import org.etsi.ttcn.tci.Value;
import org.etsi.ttcn.tri.TriAddress;
import org.etsi.ttcn.tri.TriComponentId;
import org.etsi.ttcn.tri.TriMessage;
import org.etsi.ttcn.tri.TriParameter;
import org.etsi.ttcn.tri.TriParameterList;
import org.etsi.ttcn.tri.TriPortId;
import org.etsi.ttcn.tri.TriStatus;

import com.testingtech.ttcn.tri.ISAPlugin;
import com.testingtech.ttcn.tri.TciValueContainer;
import com.testingtech.ttcn.tri.TriAddressImpl;
import com.testingtech.ttcn.tri.TriMessageImpl;
import com.testingtech.ttcn.tri.TriStatusImpl;
import com.testingtech.ttcn.tri.extension.PortPluginProvider;
import com.testingtech.ttcn.tri.tools.MapObject;
import com.testingtech.ttcn.tri.tools.MappingTable;

@SuppressWarnings("serial")
public class PhyIOPort extends PhyIOAbstractPort implements PortPluginProvider {
	
	private Map<String,ByteArrayOutputStream> messageBuffers = new Hashtable<String, ByteArrayOutputStream>();

	private MappingTable<PhyPort> phyPorts = new MappingTable<PhyPort>();


	@Override
	protected void clean() {
		synchronized (phyPorts) {
			for (PhyPort port : phyPorts.getIncomingInfoKeySet()) {
				if (port instanceof PhyConfigPort) {
					((PhyConfigPort)port).unmap();
				}
			}
		}

		if (messageBuffers != null) {
			synchronized (messageBuffers) {
				messageBuffers.clear();
			}
		}
	}

	@Override
	public TriStatus triMap(TriPortId compPortId, TriPortId tsiPortId) {
		return new TriStatusImpl("Map without parameters not supported for PhyIO ports.");
	}

	public TriStatus triMapParam(TriPortId compPortId, TriPortId tsiPortId, TriParameterList paramList) {
		int deviceId;
		int sensorId;
		
		if ("PhyIOAUX.PhyConfig".equals(tsiPortId.getPortTypeName())) {
			deviceId = ((IntegerValue)asValue(paramList, 1)).getInt();
			sensorId = 0;
		} else {
			deviceId = ((IntegerValue)asValue(paramList, 0)).getInt();
			sensorId = ((IntegerValue)asValue(paramList, 1)).getInt();
		}

		TriStatus res = virtualTriMap(compPortId, tsiPortId, deviceId, sensorId, paramList);
		if (res.getStatus() != TriStatus.TRI_OK) {
			logDebug("Error during map: "+res);
			return res;
		}

		// if PhyConfig port, initialize the Arduino PhyIO stack by sending NL
		if (sensorId == 0) {
			// wait 1 second to connect
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// ignore
			}
			// send NL
			res = triSend(compPortId.getComponent(), tsiPortId, null, TriMessageImpl.valueOf(PhyIOCodec.str2bytes("READY\n")));
		}
		return res;
	}

	private TriStatus virtualTriMap(TriPortId compPortId, TriPortId tsiPortId, int deviceID, int sensorID, TriParameterList paramList) {
		PhyPort phyPort = createPhyPort(tsiPortId, deviceID, sensorID);

		TriStatus mapResult = TriStatusImpl.OK;
		if (phyPort instanceof PhyConfigPort) {
			mapResult = ((PhyConfigPort)phyPort).map(compPortId, tsiPortId, paramList);
			if (mapResult.getStatus() == TriStatus.TRI_ERROR) {
				return new TriStatusImpl("Error mapping COM port: "+sensorID+" "+ mapResult);
			}
		}

		logDebug("-> Opened Sensor Port: "+phyPort);
		synchronized (phyPorts) {
			phyPorts.addOutgoingInfo(tsiPortId, compPortId.getComponent(), phyPort);
			phyPorts.addIncomingInfo(tsiPortId, compPortId.getComponent(), phyPort);
		}
		return TriStatusImpl.OK;
	}

	public TriStatus triUnmap(TriPortId compPortId, TriPortId tsiPortId) {
		PhyPort phyPort = phyPorts.getOutgoingInfo(tsiPortId, compPortId.getComponent());
		
		if (phyPort != null) {
			return phyPort.unmap();
		}
		return TriStatusImpl.OK;
	}

	public TriStatus triSend(TriComponentId componentId, TriPortId tsiPortId, TriAddress address, TriMessage sendMessage) {
		if (tsiPortId != null && componentId != null) {
			PhyPort phyPort = phyPorts.getOutgoingInfo(tsiPortId, componentId);
			String str = addPhyId(phyPort, sendMessage);
			logInfo("Sent: "+str);
			PhyConfigPort phyConfigPort = null;
			if (phyPort instanceof PhyConfigPort) {
				phyConfigPort = (PhyConfigPort) phyPort;
			} else {
				synchronized (phyPorts) {
					for (PhyPort port : phyPorts.getIncomingInfoKeySet()) {
						if (port instanceof PhyConfigPort && ((PhyConfigPort)port).getDeviceID() == phyPort.getDeviceID()) {
							phyConfigPort = (PhyConfigPort) port;
						}
					}
				}
			}
			if (phyConfigPort != null) {
				return super.triSend(phyConfigPort.getConnectedCompPortId().getComponent(), phyConfigPort.getConnectedTsiPortId(), address, sendMessage);
			} else {
				return new TriStatusImpl("Cannot send data on port "+tsiPortId+". No PhyConfig mapped port for DeviceID "+phyPort.getDeviceID());
			}
		}
		return new TriStatusImpl("Cannot send data on port "+tsiPortId+". No RS232 port mapped ");
	}

	/**
	 * Prefix the message with SensorID. SensorID is the address and thus should be handled by the port plugin
	 * @return the message prefixed with sensorID
	 */
	private String addPhyId(PhyPort phyPort, TriMessage sendMessage) {
		String str = PhyIOCodec.bytes2str(sendMessage.getEncodedMessage());
		str = PhyIOCodec.values(phyPort.getSensorID(), str)+"\n";
		sendMessage.setEncodedMessage(PhyIOCodec.str2bytes(str));
		return str;
	}

	public void triEnqueueMsg(TriPortId tsiPortId, TriAddress sutAddress, TriComponentId componentId, TriMessage receivedMessage) {
		try {
			String portName = tsiPortId.getPortName()+tsiPortId.getPortIndex();
			ByteArrayOutputStream msgBuff;
			synchronized (messageBuffers) {
				msgBuff = messageBuffers.get(portName);
				if (msgBuff == null) {
					msgBuff = new ByteArrayOutputStream(100000);
					messageBuffers.put(portName, msgBuff);
				}
			}

			// if message delimiter found enqueue message
			byte[] msg = receivedMessage.getEncodedMessage();
			msgBuff.write(msg);

			int len = -1;

			while (msgBuff.size() > 0) {
				byte[] queue = msgBuff.toByteArray();
				len = detectMessageLength(queue);

				if (len < 0 || msgBuff.size() < len) {
					break;
				}   
				byte[] message = new byte[len];
				System.arraycopy(queue, 0, message, 0, len);

				msgBuff.reset();
				msgBuff.write(queue, len, queue.length-len);

				// TODO add SUT address (SensorID and Sensor Module name - e.g. 01/Motor)
				processMessage(tsiPortId, sutAddress, componentId,  message);
			}
		} catch (Throwable e) {
			logError("Error while enqueue "+e.toString());
		}
	}

	private void processMessage(TriPortId tsiPortId, TriAddress sutAddress, TriComponentId componentId, byte[] message) {
		// enqueue to Test System
		String str = PhyIOCodec.bytes2str(message).trim();
		logDebug("Recv: "+str);
		if (str.startsWith("#")) {
			return;
		}
		String[] elements = str.split(",");
		int sensorId = Integer.parseInt(elements[0].trim());
		int functionId = Integer.parseInt(elements[1].trim());
		PhyPort phyPortConfig = phyPorts.getOutgoingInfo(tsiPortId, componentId);
		int deviceId = phyPortConfig.getDeviceID();
		PhyPort phyPort = null;
		Collection<PhyPort> outgoingInfoValues = phyPorts.getOutgoingInfoValues();
		for (PhyPort phy : outgoingInfoValues) {
			if (phy.getDeviceID() == deviceId && phy.getSensorID() == sensorId) {
				phyPort = phy;
				break;
			}
		}
		if (phyPort == null) {
			throw new RuntimeException(MessageFormat.format("Unknown sensor id {0} on port {1}", sensorId, tsiPortId));
		}
		if (phyPort.getSensorID() != sensorId) {
			throw new RuntimeException(MessageFormat.format("Invalid sensor id {0} expected {1} on port {2}", sensorId, phyPort.getSensorID(), tsiPortId));
		}
		if (!phyPort.getKind().isSupportingFunction(functionId)) {
			throw new RuntimeException(MessageFormat.format("Unsupported function id {0} for sensor {1} on port {2}", functionId, sensorId, tsiPortId));
		}
		MapObject info = phyPorts.getIncomingInfo(phyPort);
		
		// remove the sensor ID and send data to codec
		int indexOfComma = str.indexOf(',');
		String newStrMessage = str.substring(indexOfComma+1).trim();
		TriMessage triMessage = TriMessageImpl.valueOf(PhyIOCodec.str2bytes(newStrMessage));
		super.triEnqueueMsg(info.getTsiPortId(), new TriAddressImpl((""+sensorId).getBytes()), info.getComponentId(), triMessage);
	}

	private int detectMessageLength(byte[] queue) throws IOException {
		// detect message size, search for NL
		for (int i = 0; i < queue.length; i++) {
			if (queue[i] == 10) { // char #10 = NewLine
				return i+1;
			}
		}
		return -1;
	}

	private Value asValue(TriParameterList paramList, int index) {
		TriParameter triParameter = paramList.get(index);
		Value value = null;
		if (triParameter instanceof TciValueContainer) {
			value = ((TciValueContainer)triParameter).getValue();
		}
		return value;
	}

	private String localTypeName(TriPortId tsiPortId) {
		String portTypeName = tsiPortId.getPortTypeName();
		int lastIndexOfDot = portTypeName.lastIndexOf('.');
		return lastIndexOfDot < 0 ? portTypeName : portTypeName.substring(lastIndexOfDot+1);
	}

	@Override
	public ISAPlugin getPortPlugin() {
		return this;
	}

	private PhyPort createPhyPort(TriPortId tsiPortId, int deviceID, int sensorID) {
		PhyPortKind kind = PhyPortKind.valueOf(localTypeName(tsiPortId));
		PhyPort phyPort;
		if (sensorID == 0) {
			phyPort = new PhyConfigPort(this, deviceID, sensorID, kind);
		} else {
			phyPort = new PhyPort(deviceID, sensorID, kind);
		}
		return phyPort;
	}

	public TriStatus triMapParamRS232(TriPortId compPortId, TriPortId tsiPortId, TriParameterList paramList) {
		return super.triMapParam(compPortId, tsiPortId, paramList);
	}

}
