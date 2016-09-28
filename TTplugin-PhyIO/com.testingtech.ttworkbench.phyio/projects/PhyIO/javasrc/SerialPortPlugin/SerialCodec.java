package SerialPortPlugin;

import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;

import org.etsi.ttcn.tci.BooleanValue;
import org.etsi.ttcn.tci.FloatValue;
import org.etsi.ttcn.tci.IntegerValue;
import org.etsi.ttcn.tci.RecordValue;
import org.etsi.ttcn.tci.TciCDProvided;
import org.etsi.ttcn.tci.Type;
import org.etsi.ttcn.tci.Value;
import org.etsi.ttcn.tri.TriMessage;

import com.testingtech.ttcn.extension.CodecProvider;
import com.testingtech.ttcn.tri.AbstractCodecPlugin;
import com.testingtech.ttcn.tri.TriMessageImpl;

import de.tu_berlin.cs.uebb.muttcn.runtime.RB;

public class SerialCodec extends AbstractCodecPlugin implements CodecProvider {

	@Override
	public Value decode(TriMessage message, Type decodingHypothesis) {
		String received = msg2str(message);
		
		System.out.print("decode : receiving the following message -> " + received);
		// checks for debugging lines
		if (received.startsWith("#"))  
			return null;
		
		// create a new value object from 
		Value value = decodingHypothesis.newInstance();
		
		System.out.println("decode : trying to parse into the type -> " + value.getType().getName());
		
		// split the incoming message along the commas
		String[] elements = received.split(",");
		
		// create a new counter for the decoding
		Counter idx = new Counter();
		return decodeParameters(value, elements, idx);
	}

	@Override
	public TriMessage encode(Value value) {
		
		TriMessage message;
		String mes; // <- the unconverted String that will be send as bytes
		String valueType = value.getType().getName();
		
		// The following if statements check for the type of the value
		// then encode it accordingly
		if(valueType == "SerialConfig"){
			// convert the type to a useable string
			mes = SerialConfig2String(value.toString());
			// convert the usable string to TriMessage
			message = str2msg(mes);
		}
		else if(valueType == "SerialMessage"){
			mes = SerialMessage2String(value.toString());
			message = str2msg(mes);
		}
		else{
			// if it doesn't fit any type, then just send the String
			message = str2msg(value.toString());
		}
		return message;
	}
	
	public TciCDProvided getCodec(RB RB, String encodingRule) {
		return this;
	}

	
	/**
	 * Converts a String that is generated by reading the SerialMessage as a String
	 * to a more String that can be interpreted by the PhyIO
	 * @param SerialMessageAsString A SerialMessage as String-view
	 * @return A more useful String for the PhyIO
	 */
	private String SerialMessage2String(String SerialMessageAsString) {
		String mes = SerialMessageAsString;
		mes = mes.replace("{", "");
		mes = mes.replace("moduleID", "");
		mes = mes.replace("params", "");
		mes = mes.replace("functionID", "");
		mes = mes.replace(" := ", "");
		mes = mes.replace("}", "");
		mes = mes.substring(9, mes.length());
		return mes;
	}

	/**
	 * Converts a String that is generated by reading the SerialConfig as a String
	 * to a more String that can be interpreted by the PhyIO
	 * @param SerialConfigAsString A SerialConfig as String-view
	 * @return A more useful String for the PhyIO
	 */
	private String SerialConfig2String(String SerialConfigAsString){
		String mes = SerialConfigAsString;
		mes = mes.replaceAll("\"", "");
		mes = mes.replaceAll("BaudRate := ", "");
		mes = mes.replaceAll(" }", "");
		mes = mes.substring(14, mes.length());
		return mes;
	}
	
	/**
	 * Converts a String into a TriMessage
	 * @param s the String that will be converted to a TriMessage. 
	 * This string will always be treated as "UTF-8"-encoded
	 * @return the converted given string as TriMessage 
	 */
	private TriMessage str2msg(String s){
		return TriMessageImpl.valueOf(s.getBytes(Charset.forName("UTF-8")));
	}

	/**
	 * Converts a TriMessage into a String
	 * @param message assumes that this message is encoded with "UTF-8"
	 * @return the converted message as String
	 */
	private String msg2str(TriMessage message){
		try {
			return new String(message.getEncodedMessage(), "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return null;
		}
	}
	
	/**
	 * Parses a String list into a given value and fills it accordingly
	 * This is needed, so that 
	 * @param value the type that needs to be filled
	 * @param elements the elements that will fill the value
	 * @param idx a counter that needs to be passed along
	 * @return a completely filled Value-type-object
	 */
	private Value decodeParameters(Value value, String[] elements,  Counter idx){
		
		// if it is any standard type, then simply convert that type to a value type
		if(value instanceof IntegerValue){
			int i = Integer.parseInt(elements[idx.inc()].trim());
			((IntegerValue)value).setInt(i);
		} 
		else if(value instanceof FloatValue){
			float f = Float.parseFloat(elements[idx.inc()].trim());
			((FloatValue)value).setFloat(f);
		}
		else if(value instanceof BooleanValue){
			int b = Integer.parseInt(elements[idx.inc()].trim());
			((BooleanValue)value).setBoolean(b != 0);
		} 
		
		// if it is a record, than go through all elements a and turn those into Value-type-objects
		// then put those converted elements into the record
		else if(value instanceof RecordValue){
			String[] names = ((RecordValue)value).getFieldNames();
			
			for(String name : names) {
				Value field = ((RecordValue)value).getField(name);
				field = decodeParameters(field, elements, idx);
				((RecordValue)value).setField(name, field);
			}
			
		// if there is no matching type, then simply return null
		} else {
			logWarn("Could not decode: type " + value.getType().getName() + " not supported.");
			return null;
		}
		
		return value;
	}
	
	/**
	 * Nested class for counting by reference
	 * Simple counter, needed because decodeParameters() needs a counter of which only a reference is passed over. 
	 */
	private class Counter {
		private int i = 0;
		public int inc() { return i++; }
	}
}


