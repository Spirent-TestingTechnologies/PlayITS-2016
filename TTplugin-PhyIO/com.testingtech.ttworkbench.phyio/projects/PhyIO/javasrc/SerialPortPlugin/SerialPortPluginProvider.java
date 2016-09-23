package SerialPortPlugin;

import java.nio.charset.Charset;

import org.etsi.ttcn.tri.TriAddress;
import org.etsi.ttcn.tri.TriAddressList;
import org.etsi.ttcn.tri.TriComponentId;
import org.etsi.ttcn.tri.TriException;
import org.etsi.ttcn.tri.TriMessage;
import org.etsi.ttcn.tri.TriParameter;
import org.etsi.ttcn.tri.TriParameterList;
import org.etsi.ttcn.tri.TriPortId;
import org.etsi.ttcn.tri.TriPortIdList;
import org.etsi.ttcn.tri.TriSignatureId;
import org.etsi.ttcn.tri.TriStatus;
import org.etsi.ttcn.tri.TriTestCaseId;

import SerialCommunication.PortAllreadyInUseException;
import SerialCommunication.Serial;

import com.testingtech.ttcn.tri.AbstractSA;
import com.testingtech.ttcn.tri.ISAPlugin;
import com.testingtech.ttcn.tri.TriStatusImpl;
import com.testingtech.ttcn.tri.extension.PortPluginProvider;


public class SerialPortPluginProvider extends AbstractSA implements
		PortPluginProvider {

	Serial serial = new Serial();
	
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
		

		System.out.println("send : we send something");
		
		String str = new String(sendMessage.getEncodedMessage(), Charset.forName("UTF-8")).trim();
		
		System.out.println("send : " + str);
		
		System.out.println("send : " + serial.getPortName());
		
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
	public TriStatus triCall(TriComponentId componentId, TriPortId tsiPortId,
			TriAddress sutAddress, TriSignatureId signatureId,
			TriParameterList parameterList) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triCallBC(TriComponentId componentId, TriPortId tsiPortId,
			TriSignatureId signatureId, TriParameterList parameterList) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triCallMC(TriComponentId componentId, TriPortId tsiPortId,
			TriAddressList sutAddresses, TriSignatureId signatureId,
			TriParameterList parameterList) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triReply(TriComponentId componentId, TriPortId tsiPortId,
			TriAddress sutAddress, TriSignatureId signatureId,
			TriParameterList parameterList, TriParameter returnValue) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triReplyBC(TriComponentId componentId,
			TriPortId tsiPortId, TriSignatureId signatureId,
			TriParameterList parameterList, TriParameter returnValue) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triReplyMC(TriComponentId componentId,
			TriPortId tsiPortId, TriAddressList sutAddresses,
			TriSignatureId signatureId, TriParameterList parameterList,
			TriParameter returnValue) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triRaise(TriComponentId componentId, TriPortId tsiPortId,
			TriAddress sutAddress, TriSignatureId signatureId,
			TriException exception) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triRaiseBC(TriComponentId componentId,
			TriPortId tsiPortId, TriSignatureId signatureId, TriException exc) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triRaiseMC(TriComponentId componentId,
			TriPortId tsiPortId, TriAddressList sutAddresses,
			TriSignatureId signatureId, TriException exc) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
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
		
		System.out.println("We are connecting");
		
		String connection = new String(paramList.get(0).getEncodedParameter(), Charset.forName("UTF-8")).trim();
		
		int comma = connection.indexOf(", ");
		
		// nimm den ersten Teil für den Namen des Ports
		String PortName = connection.substring(0, comma);
		// den hinteren Teil für die Baudrate und parse ihn zum Integer
		int BaudRate = Integer.parseInt(connection.substring(comma + 2, connection.length()));
		try {
			serial.switchPort(PortName, BaudRate);
		} catch (PortAllreadyInUseException e) {
			e.printStackTrace();
		}
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triUnmap(TriPortId compPortId, TriPortId tsiPortId) {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

	@Override
	public TriStatus triEndTestCase() {
		// TODO Auto-generated method stub
		return TriStatusImpl.OK;
	}

}
