package ExternFunctionProvider;

import com.testingtech.ttcn.annotation.ExternalFunction;
import com.testingtech.ttcn.tri.AnnotationsExternalFunctionPlugin;

import SerialCommunication.NoPortInUseException;
import SerialCommunication.PortAllreadyInUseException;
import SerialCommunication.Serial;


@ExternalFunction.Definitions(PhyIOCommunication.class)
public class PhyIOCommunication extends AnnotationsExternalFunctionPlugin {
	
	Serial serial = new Serial();
	
	// Aufgrund des Kompilierens der TT-Workbench, kann man keine Richtige Klasse nutzen
	// und muss daher anscheinend alles geschickt in eine einzele Funktion als eine 
	// Art Subklasse schreieben, bei der man der Überfunktion angibt, was gemacht werden soll
	
	@ExternalFunction(name = "Serial", module = "PhyIO")
	public String Serial(String mode, String command) throws NoSuchModeException{
		
		String returnString = "";
		
		switch(mode.toUpperCase()){
			case "CONNECT":
				returnString = connect(command);
				break;
			case "SEND":
				serial.write(command);
				break;
			case "RECIEVE":
				returnString = serial.assuredReadString();
				break;
			case "STOP":
				returnString = stop();
				break;
			default:
				throw new NoSuchModeException("There is no mode" + mode);
		}
		return returnString;
	}
	
	
	private String connect(String connection){
		// finde die Tennstelle zwischen Portname und BroadRate
		// diese liegt beim Komma
		int comma = connection.indexOf(", ");
		
		// nimm den ersten Teil für den Namen des Ports
		String PortName = connection.substring(0, comma);
		// den hinteren Teil für die Baudrate und parse ihn zum Integer
		int BaudRate = Integer.parseInt(connection.substring(comma + 2, connection.length()));
		try {
			serial.switchPort(PortName, BaudRate);
			return "Connected";
		} catch (PortAllreadyInUseException e) {
			return "Connection failed";
		}
	}
	
	private String stop(){
		try {
			serial.closeConnection();
			return "Closed";
		} catch (NoPortInUseException e) {
			return "Closing failed";
		}
	}
	
	// hier an dieser Stele möglicherweise ParseInts oder Parse Floats
	// oder genau abgestimmte Funktionen die nur spezielle Ausgaben der PhyIO überprüfen
	
	// irgendetwas in der Richung :
	// bool check(zu überprüfen, das was es sein soll)

}
