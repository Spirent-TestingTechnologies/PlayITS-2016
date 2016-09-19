package ExternFunctionProvider;

import com.testingtech.ttcn.annotation.ExternalFunction;
import com.testingtech.ttcn.tri.AnnotationsExternalFunctionPlugin;

import SerialCommunication.PortAllreadyInUseException;
import SerialCommunication.Serial;


@ExternalFunction.Definitions(PhyIOCommunication.class)
public class PhyIOCommunication extends AnnotationsExternalFunctionPlugin {
	
	Serial serial = new Serial();
	
	
	//the following was just to check the functionality of what I had in mind
	@ExternalFunction(name = "test", module = "PhyIO")
	public boolean test(String s){
		if(s.contains("bla")){
			return true;
		}
		return false;
	}
	
	@ExternalFunction(name = "SerialConnect", module = "PhyIO")
	public void SerialConnect(String PortName, int BaudRate) throws PortAllreadyInUseException{
		serial.switchPort(PortName, BaudRate);
	}
	
	@ExternalFunction(name = "SerialSend", module = "PhyIO")
	public void SerialSend(String message) {
		serial.write(message);
	}
	
	

}
