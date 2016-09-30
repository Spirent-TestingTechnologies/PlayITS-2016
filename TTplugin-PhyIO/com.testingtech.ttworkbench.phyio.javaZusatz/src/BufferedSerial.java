import com.fazecast.jSerialComm.SerialPort;

public class BufferedSerial implements SerialConnection {
	
	private Serial serial;
	private StringBuffer buffer;
	private Thread listeningThread;
	private boolean listeningThreadIsRunning;
	
// -------------------------------- Constructors -----------------------------------
	
	
	/**
	 * Initializes the Class but does nothing else.
	 * Just in case one is undecided
	 */
	public BufferedSerial() {
		//empty constructor if port undecided
		serial = new Serial();
		buffer = new StringBuffer();
	}
	
	/**
	 * Initializes the Class and sets the name,
	 * but does not open the connection
	 * @param portName Name of the Port as displayed by the system
	 * or as it is found in the Arduino IDE
	 */
	public BufferedSerial(String portName) {
		//make sure to set baud rate after
		serial = new Serial(portName);
		buffer = new StringBuffer();
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
	public BufferedSerial(String portName, int baudRate) {
		//preferred constructor
		serial = new Serial(portName, baudRate);
		buffer = new StringBuffer();
		startListeningThread();
	}
	

// ------------------------ Setup for the Serial Port below -------------------------
	

	@Override
	public boolean begin(int baud_rate) throws PortAllreadyInUseException {
		boolean succededToConnect = serial.begin(baud_rate);
		startListeningThread();
		return succededToConnect;
	}

	@Override
	public void closeConnection() throws NoPortInUseException {
		try {
			listeningThreadIsRunning = false;
			listeningThread.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		serial.closeConnection();
	}

	@Override
	public void setPortByName(String portName) {
		serial.setPortByName(portName);
	}

	@Override
	public void switchPort(String portName, int baud_rate) throws PortAllreadyInUseException {
		try {closeConnection();} catch (NoPortInUseException e) {} 
		serial.switchPort(portName, baud_rate);
		startListeningThread();
	}

	@Override
	public String getPortName() {
		return serial.getPortName();
	}

	@Override
	public SerialPort getSerialPort() {
		return serial.getSerialPort();
	}

	@Override
	public int countAvailable() {
		return buffer.length();
	}

	@Override
	public boolean available() {
		return buffer.length() > 0;
	}

	@Override
	public String readString() {
		String message = "";
		synchronized(buffer){
			message = buffer.toString();
			buffer = new StringBuffer();
		}
		return message;
	}

	@Override
	public String readString(int limit) {
		String message = "";
		synchronized(buffer){
			message = buffer.substring(0, limit-1);
			buffer = new StringBuffer();
		}
		return message;
	}

	@Override
	public String assuredReadString() {
		while(!available()){
			try {Thread.sleep(10);} 
			catch (InterruptedException e) {e.printStackTrace();}
		}
		return readString();
	}

	@Override
	public void write(String s) {
		serial.write(s);
	}

	@Override
	public void write(char c) {
		serial.write(c);
	}

	@Override
	public void clean() {
		synchronized(buffer){
			buffer = new StringBuffer();
		}
	}

	
	
	/**
	 * Reads the next Character from the serial input and returns it as a String.
	 * <p>
	 * <ul><b>Note: </b><ul>This method does not care if there actually is something in the buffer. 
	 * It will read the next byte even if there is no Input to read.</ul></ul>
	 * <p>
	 * For a method that only returns a char if there is an actual input, take a look at <code> assuredReadChar() </code>
	 * @return next Character from serial as String
	 */
	public char readChar(){
		char message;
		try{
			synchronized(buffer){
				message = buffer.charAt(0);
				buffer = new StringBuffer(buffer.substring(1));
			}
		} catch (IndexOutOfBoundsException i){
			message = 0;
		}
		return message;		
	}
	
	
	/**
	 * Reads until a certain string is found in the buffer and returns the string that has been
	 * read until that point. The rest of the buffer will be left untouched.
	 * <p>
	 * <ul><b>Note : </b>
	 * This method will block until that matching string is found inside the buffer.
	 * </ul><p>
	 * <b>Example : </b><ul>
	 * Lets say that "something to read" is in the buffer and "to" is the given match.
	 * Then "something to" will be returned and " read" will be left in the buffer.
	 * </ul>
	 * @param match the string that has to be matched
	 * @return the input that has been read up to the point of finding the given match
	 */
	public String readUntil(String match){
		
		String message = "";
		int matchlen = match.length();
		
		// read as many chars as needed to even have the length of 'match'
		for(int i = 0; i < matchlen; i++){
			message += assuredReadChar();
		}
		
		// read as many chars as needed until the last characters in 'message' are the same as 'match'
		while(!message.substring(message.length() - matchlen).equals(match)){
			message += assuredReadChar();
		}
		return message;
	}
	
	/**
	 * Reads everything out of the serial port that starts and ends with a certain String. 
	 * This makes sure, that everything of a specific message type is recieved and nothing else.
	 * <p>
	 * This is especially good for bluetooth and similar connection types, as those kind of connections
	 * tend to sometimes send messages cut into peaces and not as one complete chunk.
	 * Exidental cutting of the messages on the sending site leads to incomplete messages on
	 * the receiving site and this method tries to avoid such things
	 * <p>
	 * <ul><b>Note: <ul>It can happen that messages will be disgarded and lost, if they are incomplete and a 
	 * new message with the start string arrives. </b></ul></ul>
	 * @param begin The String the expected message will start with
	 * @param end The String the expected message will end with
	 * @return A recieved String that had the given beginning and end. <br>
	 * Those Strings are cutted from the returned String
	 */
	public String receive(String begin, String end){
		String message = "";
		
		// toss everything away, that until 'begin' is found
		readUntil(begin);
		// save everything up to 'end'
		message = readUntil(end);
		
		// find the newest 'begin' in the string
		int index = message.lastIndexOf(begin);
		// find the latest index after the occurence
		if(index == -1){
			index = 0;
		}
		else{
			index += begin.length();
		}
		// toss away 'end' at the end of the message and everything before the newest 'begin'
		// including 'begin'
		message = message.substring(index, message.length()-end.length());
	
		return message;
	}
	
	/**
	 * Reads the next Byte from the serial input and returns it as a char.
	 * <p>
	 * <ul><b>Note: </b>This method blocks until there is Input to read.</ul>
	 * @return next Byte from serial as char
	 */
	public char assuredReadChar(){
		while(!available()){
			try {Thread.sleep(10);} 
			catch (InterruptedException e) {e.printStackTrace();}
		}
		return readChar();
	}
	
	
	/**
	 * Starts a thread, that reads everything that gets into connection and saves it
	 * in a buffer
	 */
	private void startListeningThread(){
		listeningThread = new Thread("listening thread"){

			String message;
			
			public void run(){
				
				while(listeningThreadIsRunning){
					
					// as long as nothing is available, wait a little
					while(!serial.available()){
						try {Thread.sleep(10);} catch (InterruptedException e) {e.printStackTrace();}
						// has to check inside this loop, or else this thread would run forever, 
						// once nothing is received anymore, even if isRecieverThreadRunning is set to false
						if(!listeningThreadIsRunning){
							break;
						}
					}
					// has to check before reading, so this thread isn't providing empty data
					if(listeningThreadIsRunning){
						message = serial.readString();
						synchronized (buffer) {
							buffer.append(message);
						}
					}
				}
			}
		};
		listeningThreadIsRunning = true;
		listeningThread.start();
	}

}
