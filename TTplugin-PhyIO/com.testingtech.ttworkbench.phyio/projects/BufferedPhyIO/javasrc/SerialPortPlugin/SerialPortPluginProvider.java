package SerialPortPlugin;

import java.nio.charset.Charset;

import org.etsi.ttcn.tri.TriAddress;
import org.etsi.ttcn.tri.TriAddressList;
import org.etsi.ttcn.tri.TriComponentId;
import org.etsi.ttcn.tri.TriMessage;
import org.etsi.ttcn.tri.TriParameterList;
import org.etsi.ttcn.tri.TriPortId;
import org.etsi.ttcn.tri.TriPortIdList;
import org.etsi.ttcn.tri.TriStatus;
import org.etsi.ttcn.tri.TriTestCaseId;

import SerialCommunication.BufferedSerial;
import SerialCommunication.NoPortInUseException;
import SerialCommunication.PortAllreadyInUseException;

import com.testingtech.ttcn.tri.AbstractMsgBasedSA;
import com.testingtech.ttcn.tri.ISAPlugin;
import com.testingtech.ttcn.tri.TriMessageImpl;
import com.testingtech.ttcn.tri.TriStatusImpl;
import com.testingtech.ttcn.tri.extension.PortPluginProvider;


public class SerialPortPluginProvider extends AbstractMsgBasedSA implements
		PortPluginProvider {

	BufferedSerial serial = new BufferedSerial();
	public Thread recieverThread;
	private boolean isRecieverThreadRunning;
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public ISAPlugin getPortPlugin() {
		return this;
	}

	@Override
	public TriStatus triSend(TriComponentId componentId, TriPortId tsiPortId,
			TriAddress address, TriMessage sendMessage) {
		
		// convert the TriMessage to a sendable string
		String message = new String(sendMessage.getEncodedMessage(), Charset.forName("UTF-8")).trim();
		// show on console what we will send
		System.out.println("send : " + message);
		// send the actual message to the device
		serial.write(message);
		// set ok , since everything went all right
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triSendBC(TriComponentId componentId, TriPortId tsiPortId,
			TriMessage sendMessage) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triSendMC(TriComponentId componentId, TriPortId tsiPortId,
			TriAddressList addresses, TriMessage sendMessage) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}


	@Override
	public void triEnqueueMsg(TriPortId tsiPortId, TriAddress sutAddress,
			TriComponentId componentId, TriMessage receivedMessage) {
		getRB().getTriCommunicationTE().triEnqueueMsg(tsiPortId, sutAddress, componentId, receivedMessage);
	}
	
	
	@Override
	public TriStatus triSAReset() {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triExecuteTestcase(TriTestCaseId testCaseId,
			TriPortIdList tsiPorts) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triMap(TriPortId compPortId, TriPortId tsiPortId) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}
	
	@Override
	public TriStatus triMapParam(TriPortId compPortId, TriPortId tsiPortId,
			TriParameterList paramList) {
		System.out.print("Connecting to : "); // Will be the output for this function together with output B
		
		// convert the first entry of the paramlist to a string
		String connection = new String(paramList.get(0).getEncodedParameter(), Charset.forName("UTF-8")).trim();
		
		// get the index of where to cut the string
		int comma = connection.indexOf(", ");
		int comma2 = connection.indexOf(", ", comma + 1);
		int comma3 = connection.indexOf(", ", comma2 + 1);
		
		// first part of the string is the name of the port
		String PortName = connection.substring(0, comma);
		// the latter part is the baudrate
		int BaudRate = Integer.parseInt(connection.substring(comma + 2, comma2));
		
		System.out.println(PortName); // <- output B

		// filter out the beginning and end strings of the incomming messages
		String begin = connection.substring(comma2, comma3);
		String end = connection.substring(comma3, connection.length());
		
		System.out.print("Only considering incomming messages beginning with : '" + begin + "' and end with '" + end + "'" );
		
		try {
			serial.switchPort(PortName, BaudRate);  // try to connect
		} catch (PortAllreadyInUseException e) {
			e.printStackTrace();
			TriStatus status = TriStatusImpl.OK;
			status.setStatus(-1);
			return status; // if connection fails, then print the stacktrace and return an error status
		}
		try {Thread.sleep(2500);} catch (InterruptedException e) {} // wait, so that the device can set itself up
		
		startReceiverThread(compPortId, tsiPortId, begin, end);
		
		try {Thread.sleep(500);} catch (InterruptedException e) {} // wait a bit, for the thread
		
		return TriStatusImpl.OK; // return ok status
	}

	@Override
	public TriStatus triUnmap(TriPortId compPortId, TriPortId tsiPortId) {
		System.out.println("Disconnect : Disconnect from serial port");
		try {
			// try to close the connection, the 'try - catch' block will never be invoked,
			// as long as the port was mapped first. Of course one shouldn't unmap without
			// mapping in the first place
			isRecieverThreadRunning = false;
			recieverThread.join();
			serial.closeConnection();
		} catch (NoPortInUseException | InterruptedException e) {e.printStackTrace();}
		
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triEndTestCase() {
		return TriStatusImpl.OK;
	}

	
	/**
	 * Sends all incoming data to enqueueMsg, so it can be handled by "receive"
	 * @param end 
	 * @param begin 
	 */
	private void startReceiverThread(final TriPortId compPortId, final TriPortId tsiPortId, String begin, String end){
		
		recieverThread = new Thread("Receiver thread for " + serial.getPortName()){
			public void run() {
				
				String message;
				serial.clean();
				
				while(isRecieverThreadRunning){
					message = serial.receive(begin, end);
					
					// convert the received string to a message
					TriMessage rcvMessage = TriMessageImpl.valueOf(message.getBytes(Charset.forName("UTF-8")));	
					// forward the message
					triEnqueueMsg( tsiPortId, null, compPortId.getComponent(), rcvMessage );
					
				}
			}
		};
		isRecieverThreadRunning = true;
		recieverThread.start();
	}
	
}
