package SerialCommunication;
import com.fazecast.jSerialComm.SerialPort;

public interface SerialConnection {

	/**
	 * Starts the connection to the port
	 * <p>
	 * <b>Note: </b><ul><li>only one connection can be open at a time.
	 * <li> If there is already a connection to a port that one has to be closed first
	 * <li> If there is need to switch to another port while a programm is running, see: 
	 * <p><code>switchPort(String portName, int baud_rate)</code>
	 * 
	 * </ul>
	 * @param baud_rate Speed of the connection, standard most 
	 * of the time is 9600, but all of the following are possible
	 * 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 
	 * 28800, 38400, 57600, or 115200
	 * @return true if the connection is successful, false if not
	 * @throws PortAllreadyInUseException If the Port of choice is already in use 
	 * by another programm or thread and therefor blocked
	 */
	boolean begin(int baud_rate) throws PortAllreadyInUseException;

	/**
	 * Closes the connection to the Port. Always use this after 
	 * communication is done, so other software may use this 
	 * connection.
	 * @throws NoPortInUseException If there is no active connection to 
	 * any kind of port
	 */
	void closeConnection() throws NoPortInUseException;

	/**
	 * Sets the Port to the given Port. 
	 * <p>
	 * <b>Note: </b><li> Close the current connection with <code>closeConnection()</code> if there is 
	 * an open connection.
	 * <li> Doesn't start the communication with the port only changes Port to which it will eventually communicate
	 * <li> Use <code>begin(int baud_rate)</code> to start the communication
	 * @param portName Name of the Port as displayed by the system
	 * or as it is found in the Arduino IDE
	 */
	void setPortByName(String portName);

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
	 * @throws PortAllreadyInUseException If there is no active connection to 
	 * any kind of port
	 */
	void switchPort(String portName, int baud_rate) throws PortAllreadyInUseException;

	/**
	 * Returns the port name of the current connection
	 * @return Port name as String
	 */
	String getPortName();

	/**
	 * Returns the current serial port for further use
	 * @return SerialPort from jSerialComm
	 */
	SerialPort getSerialPort();

	/**
	 * Get the number of bytes (characters) 
	 * available for reading from the serial port.
	 * This is data that's already arrived and stored
	 * @return the number of bytes available to read
	 */
	int countAvailable();

	/**
	 * Tells if there are byte ready for reading on the port
	 * @return if there are bytes to read
	 */
	boolean available();

	/**
	 * Reads characters from the serial buffer into a string. 
	 * Returns an empty String if there is nothing available
	 * @return A string read from the serial buffer
	 */
	String readString();

	/**
	 * Reads characters from the serial buffer into a string. 
	 * Returns an empty String if there is nothing available
	 * @param limit limit the number of bytes that are read
	 * in case of unlimited incoming data or if the length of the
	 * incoming data is already know
	 * @return A string read from the serial buffer
	 */
	String readString(int limit);


	/**
	 * Reads characters from the serial buffer into a string
	 * Will be an infinite loop if incoming data is not bound
	 * @return A string read from the serial buffer
	 */
	String assuredReadString();


	/**
	 * Writes a String to the serial port connection that is currently active
	 * @param s the String which will be send to the serial port
	 */
	void write(String s);

	/**
	 * Writes a Char to the serial port connection that is currently active
	 * @param c the Char which will be send to the serial port
	 */
	void write(char c);

	/**
	 * Deletes all the currently saved Bytes on the input site of the connection.
	 * Everything the connected device has been send will be deleted.
	 * This is useful for bluetooth connections, as those kind of connections save
	 * Data that has been transmitted, even before the program has been started.
	 */
	void clean();

}