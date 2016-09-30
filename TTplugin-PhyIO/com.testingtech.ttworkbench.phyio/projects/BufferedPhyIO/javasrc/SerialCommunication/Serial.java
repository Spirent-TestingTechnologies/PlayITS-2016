package SerialCommunication;
import java.io.PrintWriter;
import java.util.Scanner;

import com.fazecast.jSerialComm.*;


public class Serial implements SerialConnection {
	
	private SerialPort comPort;
	private String portName;
	private int baud_rate;
	
// -------------------------------- Constructors -----------------------------------
	
	
	/**
	 * Initializes the Class but does nothing else.
	 * Just in case one is undecided
	 */
	public Serial() {
		//empty constructor if port undecided
	}
	
	/**
	 * Initializes the Class and sets the name,
	 * but does not open the connection
	 * @param portName Name of the Port as displayed by the system
	 * or as it is found in the Arduino IDE
	 */
	public Serial(String portName) {
		//make sure to set baud rate after
		setPortByName(portName);
	}
	
	/**
	 * Initializes the Class and starts the connection to the given
	 * port name with the set baud rate
	 * 
	 * @param portName Name of the Port as displayed by the system
	 * or as it is found in the Arduino IDE
	 * 
	 * @param baud_rate Speed of the connection, standard most 
	 * of the time is 9600, but all of the following are possible
	 * 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 
	 * 28800, 38400, 57600, or 115200
	 * @throws PortAllreadyInUseException 
	 */
	public Serial(String portName, int baud_rate) {
		//preferred constructor
		setPortByName(portName);
		try {
			begin(baud_rate);
		} catch (PortAllreadyInUseException e) {
			e.printStackTrace();
		}
	}
	

// ------------------------ Setup for the Serial Port below -------------------------
	
	
	@Override
	public boolean begin(int baud_rate) throws PortAllreadyInUseException{
		setBaudRate(baud_rate);
		
		if(comPort.openPort()){
			try {Thread.sleep(100);} catch(Exception e){}
			return true;
		}
		throw new PortAllreadyInUseException("The port is currently in use and therefore blocked by another Software\n"
				+ "close that connection first before opening it here");
		
	}

	@Override
	public void closeConnection() throws NoPortInUseException {
		try{
			comPort.closePort();
		} catch(NullPointerException n) {
			throw new NoPortInUseException("Error while closing, There was no connection to a port");
		}
		
	}
	
	@Override
	public void setPortByName(String portName){
		this.portName = portName;
		comPort = SerialPort.getCommPort(this.portName);
	}
	
	@Override
	public void switchPort(String portName, int baud_rate) throws PortAllreadyInUseException{
		try{  // try is for the case, that there is no connection
			closeConnection();
		} catch(NoPortInUseException n) {}
		
		setPortByName(portName);
		begin(baud_rate);
	}
	
	private void setBaudRate(int baud_rate){
		// helper function to set the baud rate
		this.baud_rate = baud_rate;
		comPort.setBaudRate(this.baud_rate);
	}
	
	@Override
	public String getPortName(){
		return portName;
	}
	
	@Override
	public SerialPort getSerialPort(){
		return comPort;
	}
	
	
// --------------------- Interaction with the Serial Port below ----------------------
	
	
	@Override
	public int countAvailable(){
		return comPort.bytesAvailable();
	}

	@Override
	public boolean available(){
		return comPort.bytesAvailable() != 0;
	}	

	@Override
	public String readString(){
		String message="";
		Scanner in = new Scanner(comPort.getInputStream());
		try{
			while(in.hasNext())
				message += in.next();
			in.close();
		} catch (Exception e) {e.printStackTrace();}
		return message;
	}

	@Override
	public String readString(int limit){
		String message="";
		int count=0;
		Scanner in = new Scanner(comPort.getInputStream());
		try{
			while(in.hasNext()&&count<=limit){
				message += in.next();
				count++;
			}
			in.close();
		} catch (Exception e) {e.printStackTrace();}
		return message;
	}

	@Override
	public String assuredReadString(){
		while(!available()){
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {e.printStackTrace();}
		}
		return readString();
	}
	
	@Override
	public void write(String s){
		//writes the entire string at once.
		try{Thread.sleep(5);} catch(Exception e){}
		PrintWriter pout = new PrintWriter(comPort.getOutputStream());
		pout.print(s);
		pout.flush();
	}
	
	@Override
	public void write(char c){
		//writes the entire string at once.
		try{Thread.sleep(5);} catch(Exception e){}
		PrintWriter pout = new PrintWriter(comPort.getOutputStream());
		pout.write(c);
		pout.flush();
	}
	
	@Override
	public void clean() {
		readString();
	}
	
}
