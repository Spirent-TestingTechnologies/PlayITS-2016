import java.io.IOException;

import com.fazecast.jSerialComm.SerialPort;


public class FirstSteps {

	public static void main(String[] args) throws IOException, InterruptedException, PortAllreadyInUseException, NoPortInUseException {
		System.out.println("Program started");
		
		
		Serial s = new Serial(SerialPort.getCommPorts()[0].getSystemPortName(), 9600);
		Thread.sleep(2000); // <- es wird am Anfang zum Kommunikationsaufbau ein kleiner Delay benötigt
							//	  sonst kommt die erste Eingabe einfach nicht an
		
		while(true){
			s.write("1, 2, 3"); // <- ist so, als ob man in die Kommandozeile eingibt
			System.out.print(s.assuredReadString());
			Thread.sleep(2000);
		}
//			System.out.print(s.assuredReadString());
		
//		System.out.println("Finished successfully");
	
	}
	
}