package SerialCommunication;

public class PortAllreadyInUseException extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public PortAllreadyInUseException(String message) {
		super(message);
	}

}
