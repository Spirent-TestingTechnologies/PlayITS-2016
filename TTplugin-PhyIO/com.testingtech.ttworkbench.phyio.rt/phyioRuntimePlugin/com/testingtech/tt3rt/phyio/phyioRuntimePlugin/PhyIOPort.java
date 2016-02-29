package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.text.MessageFormat;
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
	
	private boolean connectedCOMPort = false;
	private Map<String,ByteArrayOutputStream> messageBuffers = new Hashtable<String, ByteArrayOutputStream>();

	private MappingTable<PhyPort> phyPorts = new MappingTable<PhyPort>();
	private Map<Integer, PhyPort> phyPortsIds = new Hashtable<Integer, PhyPort>();

	private TriPortId connectedTsiPortId;
	private TriPortId connectedCompPortId;

	@Override
	protected void clean() {
		disconnect();

		if (messageBuffers != null) {
			synchronized (messageBuffers) {
				messageBuffers.clear();
			}
		}
	}

	private TriStatus disconnect() {
		synchronized (phyPorts) {
			if (!connectedCOMPort) {
				return TriStatusImpl.OK;
			}
			// disconnect from COM port
			connectedCOMPort = false;
		}
		return super.triUnmap(connectedCompPortId, connectedTsiPortId);
	}

	private TriStatus connect(TriPortId compPortId, TriPortId tsiPortId) {
		if (!connectedCOMPort) {
			synchronized (phyPorts) {
				connectedCompPortId= compPortId;
				connectedTsiPortId = tsiPortId;
				TriStatus mapResult = super.triMap(compPortId, tsiPortId);
				if (mapResult.getStatus() == TriStatus.TRI_ERROR) {
					return new TriStatusImpl("Error mapping COM port: " + mapResult);
				}
				connectedCOMPort = true;
			}
		}
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triMap(TriPortId compPortId, TriPortId tsiPortId) {
		TriStatus res = virtualTriMap(compPortId, tsiPortId, 0);
		if (res.getStatus() != TriStatus.TRI_OK) {
			logDebug("Error during map: "+res);
			return res;
		}
		// wait 1 second
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// ignore
		}
		// send NL
		return triSend(null, null, null, TriMessageImpl.valueOf(PhyIOCodec.str2bytes("READY\n")));
	}

	public TriStatus triMapParam(TriPortId compPortId, TriPortId tsiPortId, TriParameterList paramList) {
		Value paramValue = asValue(paramList, 0);
		int id = ((IntegerValue)paramValue).getInt();

		// Contract: ONLY call super if the filter should forward this message/call.
//		return super.triMapParam(compPortId, tsiPortId, paramList);
		return virtualTriMap(compPortId, tsiPortId, id);
	}

	private TriStatus virtualTriMap(TriPortId compPortId, TriPortId tsiPortId, int id) {
		TriStatus mapResult = connect(compPortId, tsiPortId);
		if (mapResult.getStatus() == TriStatus.TRI_ERROR) {
			return new TriStatusImpl("Error mapping COM port: "+id+" "+ mapResult);
		}

		PhyPort phyPort = createPhyPort(tsiPortId, id);

		logDebug("-> Opened Sensor Port: "+phyPort);
		synchronized (phyPorts) {
			phyPorts.addOutgoingInfo(tsiPortId, compPortId.getComponent(), phyPort);
			phyPorts.addIncomingInfo(tsiPortId, compPortId.getComponent(), phyPort);
			phyPortsIds.put(phyPort.getId(), phyPort);
		}
		return TriStatusImpl.OK;
	}

	public TriStatus triUnmap(TriPortId compPortId, TriPortId tsiPortId) {
		if (connectedCOMPort) {
			synchronized (phyPorts) {
				connectedCOMPort = false;
				// Contract: ONLY call super if the filter should forward this message/call.
				return super.triUnmap(connectedCompPortId, connectedTsiPortId);
			}
		}
		return TriStatusImpl.OK;
	}

	public TriStatus triSend(TriComponentId componentId, TriPortId tsiPortId, TriAddress address, TriMessage sendMessage) {
		if (tsiPortId != null && componentId != null) {
			PhyPort phyPort = phyPorts.getOutgoingInfo(tsiPortId, componentId);
			String str = addPhyId(phyPort, sendMessage);
			logInfo("Sent: "+str);
		}
		return super.triSend(connectedCompPortId.getComponent(), connectedTsiPortId, address, sendMessage);
	}

	/**
	 * Prefix the message with SensorID and ProtocolID
	 * @return 
	 */
	private String addPhyId(PhyPort phyPort, TriMessage sendMessage) {
		String str = PhyIOCodec.bytes2str(sendMessage.getEncodedMessage());
//		int functionId = phyPort.getKind().getSupportedFunctions()[0].getId();
		// TODO add this maybe from PhyIOCodec
		str = PhyIOCodec.values(phyPort.getId(), str)+"\n";
		sendMessage.setEncodedMessage(PhyIOCodec.str2bytes(str));
		return str;
	}

	public void triEnqueueMsg(TriPortId tsiPortId, TriAddress sutAddress, TriComponentId componentId, TriMessage receivedMessage) {
		try {
			String portName = tsiPortId.getPortName();
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
		PhyPort phyPort = phyPortsIds.get(sensorId);
		if (!phyPort.getKind().isSupportingFunction(functionId)) {
			throw new RuntimeException(MessageFormat.format("unsupported function id {0} for sensor {1}", functionId, sensorId));
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

	private PhyPort createPhyPort(TriPortId tsiPortId, int id) {
		PhyPortKind kind = PhyPortKind.valueOf(localTypeName(tsiPortId));
		PhyPort phyPort = new PhyPort(id, kind);
		return phyPort;
	}

}
