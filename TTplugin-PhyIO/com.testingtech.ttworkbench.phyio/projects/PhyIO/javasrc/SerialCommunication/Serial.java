package SerialCommunication;

import java.io.PrintWriter;
import java.util.Scanner;

import com.fazecast.jSerialComm.*;


public class Serial {
	
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
	
	
	/**
	 * Starts the connection to the port
	 * @param baud_rate Speed of the connection, standard most 
	 * of the time is 9600, but all of the following are possible
	 * 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 
	 * 28800, 38400, 57600, or 115200
	 * @return true if the connection is successful, false if not
	 * @throws PortAllreadyInUseException 
	 */
	public boolean begin(int baud_rate) throws PortAllreadyInUseException{
		setBaudRate(baud_rate);
		
		if(comPort.openPort()){
			try {Thread.sleep(100);} catch(Exception e){}
			return true;
		}
		throw new PortAllreadyInUseException("The port is currently in use and therefore blocked by another Software\n"
				+ "close that connection first before opening it here");
		
	}
	
	/**
	 * Closes the connection to the Port. Always use this after 
	 * communication is done, so other software may use this 
	 * connection.
	 * @throws NoPortInUseException 
	 */
	public void closeConnection() throws NoPortInUseException {
		try{
			comPort.closePort();
		} catch(NullPointerException n) {
			throw new NoPortInUseException("Error while closing, There was no connection to a port");
		}
		
	}
	
	/**
	 * Sets the Port to the given Port. 
	 * Close the connection with 'closeConnection()' if there is 
	 * an open connection.
	 * @param portName Name of the Port as displayed by the system
	 * or as it is found in the Arduino IDE
	 */
	public void setPortByName(String portName){
		this.portName = portName;
		comPort = SerialPort.getCommPort(this.portName);
	}
	
	/**
	 * Closes the current connection (if there is any) and switches to 
	 * the given port with the given baud rate
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
	public void switchPort(String portName, int baud_rate) throws PortAllreadyInUseException{
		try{  // try is for the case, that there is no connection
			comPort.closePort();
		} catch(NullPointerException n) {}
		
		setPortByName(portName);
		begin(baud_rate);
	}
	
	
	private void setBaudRate(int baud_rate){
		// helper function to set the baud rate
		this.baud_rate = baud_rate;
		comPort.setBaudRate(this.baud_rate);
	}
	
	/**
	 * Returns the port name of the current connection
	 * @return Port name as String
	 */
	public String getPortName(){
		return portName;
	}
	
	/**
	 * Returns the current serial port for further use
	 * @return SerialPort from jSerialComm
	 */
	public SerialPort getSerialPort(){
		return comPort;
	}
	
	
// --------------------- Interaction with the Serial Port below ----------------------
	
	
	/**
	 * Get the number of bytes (characters) 
	 * available for reading from the serial port.
	 * This is data that's already arrived and stored
	 * @return the number of bytes available to read
	 */
	public int countAvailable(){
		return comPort.bytesAvailable();
	}
	
	/**
	 * Tells if there are byte ready for reading on the port
	 * @return if there are bytes to read
	 */
	public boolean available(){
		return comPort.bytesAvailable() != 0;
	}	
	
	/**
	 * Reads characters from the serial buffer into a string
	 * Will be an infinite loop if incoming data is not bound
	 * @return A string read from the serial buffer
	 */
	public String readString(){
		String out="";
		Scanner in = new Scanner(comPort.getInputStream());
		try
		{
		   while(in.hasNext())
		      out += (in.next()+"\n");
		   	in.close();
		} catch (Exception e) { e.printStackTrace(); }
		return out;
	}
	
	/**
	 * Reads characters from the serial buffer into a string
	 * @param limit limit the number of bytes that are read
	 * in case of unlimited incoming data or if the length of the
	 * incoming data is already know
	 * @return A string read from the serial buffer
	 */
	public String readString(int limit){
		String out="";
		int count=0;
		Scanner in = new Scanner(comPort.getInputStream());
		try
		{
		   while(in.hasNext()&&count<=limit){
		      out += (in.next()+"\n");
		      count++;
		   }
		   	in.close();
		} catch (Exception e) { e.printStackTrace(); }
		return out;
	}
	
	/**
	 * Reads characters from the serial buffer into a string
	 * Will be an infinite loop if incoming data is not bound
	 * @return A string read from the serial buffer
	 */
	public String assuredReadString(){
		while(!available()){
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {e.printStackTrace();}
		}
		return readString();
	}
	
	
	/**
	 * Writes a String to the serial port connection that is currently active
	 * @param s the String which will be send to the serial port
	 */
	public void write(String s){
		//writes the entire string at once.
		try{Thread.sleep(5);} catch(Exception e){}
		PrintWriter pout = new PrintWriter(comPort.getOutputStream());
		pout.print(s);
		pout.flush();
	}
	
	/**
	 * Writes a Char to the serial port connection that is currently active
	 * @param c the Char which will be send to the serial port
	 */
	public void write(char c){
		//writes the entire string at once.
		try{Thread.sleep(5);} catch(Exception e){}
		PrintWriter pout = new PrintWriter(comPort.getOutputStream());
		pout.write(c);
		pout.flush();
	}
	
	/**
	 * Deletes all the currently saved Bytes on the input site of the connection.
	 * Everything the connected device has been send will be deleted.
	 * This is useful for bluetooth connections, as those kind of connections save
	 * Data that has been transmitted, even before the program has been started.
	 */
	public void clean() {
		readString();
	}
	
}