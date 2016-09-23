package SerialCommunication;

public class NoPortInUseException extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Is thrown, if there is no Port connected.
	 * @param message adds the Error message to the StackTrace
	 */
	public NoPortInUseException(String message) {
		super(message);
	}

}
