import java.io.IOException;
import java.util.Scanner;

import com.fazecast.jSerialComm.SerialPort;


public class FirstSteps {
	
	static BufferedSerial serial;
	public static Thread recieverThread;
	private static boolean isRecieverThreadRunning;
	private static Thread inputThread;

	public static void main(String[] args) throws IOException, InterruptedException, PortAllreadyInUseException, NoPortInUseException {
		System.out.println("Program started");
		
		
		serial = new BufferedSerial(SerialPort.getCommPorts()[0].getSystemPortName(), 9600);
		Thread.sleep(2000); // <- es wird am Anfang zum Kommunikationsaufbau ein kleiner Delay benötigt
							//	  sonst kommt die erste Eingabe einfach nicht an
		
		
		
		startReceiverThread();
		startInputStream();
		
		recieverThread.join();
		inputThread.join();
		
		serial.closeConnection();
		
		System.out.println("Finished successfully");
	
	}
	
	private static void  startInputStream(){
		inputThread = new Thread("Input thread"){
			public void run(){
				
				String input = "";
				@SuppressWarnings("resource")
				Scanner scan = new Scanner(System.in);
				
				while(!input.contains("END")){
					if(scan.hasNext()){
						input = scan.nextLine();
						System.out.println(input);
					}
				}
				isRecieverThreadRunning = false;
			}
		};
		inputThread.start();
		System.out.println("---- input thread just started");
	}
	
	
	private static void startReceiverThread(){
		
		recieverThread = new Thread("Receiver thread for " + serial.getPortName()){
			public void run() {
				
				String message;
				serial.clean();
				
				while(isRecieverThreadRunning){

						message = serial.receive("?", "!");
						
						System.out.println("recieved : " + message);
						
				}
			}
		};
		
		isRecieverThreadRunning = true;
		recieverThread.start();
		System.out.println("---- reciever thread just started");
	}
	
}