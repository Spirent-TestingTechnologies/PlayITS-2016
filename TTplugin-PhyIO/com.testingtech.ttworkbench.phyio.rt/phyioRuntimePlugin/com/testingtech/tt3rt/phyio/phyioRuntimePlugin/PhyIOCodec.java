package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyIOConstants.*;

import java.nio.charset.Charset;
import java.text.MessageFormat;

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
import com.testingtech.util.StringUtil;

import de.tu_berlin.cs.uebb.muttcn.runtime.Anytype;
import de.tu_berlin.cs.uebb.muttcn.runtime.RB;

public class PhyIOCodec extends AbstractCodecPlugin implements CodecProvider {

	private static final String COMMA_DELIM = ",";
	private static final Charset UTF8Charset = Charset.forName("UTF-8");
	
	/**
	 * Simple counter, needed because decodeParameters() needs a counter of which only a reference is passed over. 
	 */
	private class Counter {
		private int i = 0;
		public int inc() { return i++; }
	}

	@Override
	public Value decode(TriMessage message, Type decodingHypothesis) {
		String str = bytes2str(message.getEncodedMessage());
		if (str.startsWith("#"))
			return null;
		
		Value value = decodingHypothesis.newInstance();
		
		String[] variantElements = value.getType().getTypeEncodingVariant().split(COMMA_DELIM);
		
		int moduleIdHypothesis = getModuleID(variantElements[0].trim());
		//functionID not needed
		
		String[] elements = str.split(COMMA_DELIM);
		Counter idx = new Counter();
		int moduleIdMessage = Integer.parseInt(elements[idx.inc()].trim());
		int functionIdMessage = Integer.parseInt(elements[idx.inc()].trim());
		
		if(moduleIdMessage != moduleIdHypothesis || functionIdMessage != RESULT)
			return null;
		
		return decodeParameters(value, elements, idx);
	}
	
	Value decodeParameters(Value value, String[] elements,  Counter idx)
	{
		if(value instanceof IntegerValue){
			int v = Integer.parseInt(elements[idx.inc()].trim());
			((IntegerValue)value).setInt(v);
		} else if(value instanceof FloatValue){
			float v = Float.parseFloat(elements[idx.inc()].trim());
			((FloatValue)value).setFloat(v);
		} else if(value instanceof BooleanValue){
			int v = Integer.parseInt(elements[idx.inc()].trim());
			((BooleanValue)value).setBoolean(v != 0);
		} else if(value instanceof RecordValue){
			String[] names = ((RecordValue)value).getFieldNames();
			
			for(String name : names) {
				Value field = ((RecordValue)value).getField(name);
				field = decodeParameters(field, elements, idx);
				((RecordValue)value).setField(name, field);
			}
		} else {
			logWarn("Could not decode: type " + value.getType().getName() + " not supported.");
			return null;
		}
		
		return value;
	}

	@Override
	public TriMessage encode(Value value) {
		
		if(value instanceof Anytype) {
			Anytype v = (Anytype)value;
			value = v.getVariant(v.getPresentVariant());
		}
		
		String typeEncoding = value.getType().getTypeEncoding(); // ""
		
		if (!"PhyIO".equals(typeEncoding)) {
			TciCDProvided codec = getCodec(typeEncoding);
			if (codec != null) {
				return codec.encode(value);
			} else {
				tciErrorReq(MessageFormat.format("Unknown encoding '{0}' for type {1}", typeEncoding, value.getType()));
				return null;
			}
		}
		String typeEncodingVariant = value.getType().getTypeEncodingVariant();
		
		if(typeEncodingVariant == null)
			return null;
		
		String moduleFunction = translateVariant(value.getType().getTypeEncodingVariant());
		String parameters = encodeParameters(value);
		
		if (moduleFunction != null && parameters != null) {
			String out = moduleFunction;
			if(!parameters.equals(""))
				out += ", " + parameters;
			return TriMessageImpl.valueOf(str2bytes(out));
		} else {
			tciErrorReq(MessageFormat.format("Unable to encode '{0}' for type {1}", value, value.getType()));
			return null;
		}
	}
	
	String translateVariant(String variant) {
		if (variant == null)
			return null;
		
		String[] elements = variant.split(COMMA_DELIM);
		
		int module = getModuleID(elements[0].trim());
		int function = getFunctionID(elements[1].trim());
		
		if(module == -1 || function == -1)
			return null;
		
		return module + ", " + function;
	}
	
	int getModuleID(String moduleIdentifier) {
		if(moduleIdentifier.equals("CONFIG")){
			return PhyModule.GeneralConfig01.getId();
		}
		else if (moduleIdentifier.equals("COLOR")){
			return PhyModule.ColorView01.getId();
		}
		else if (moduleIdentifier.equals("ECHO")){
			return PhyModule.PingEcho01.getId();
		}
		else if (moduleIdentifier.equals("DISTANCE")){
			return PhyModule.PingEcho02.getId();
		}
		else if (moduleIdentifier.equals("LED")){
			return PhyModule.LED01.getId();
		}
		else if (moduleIdentifier.equals("RELAY")){
			return PhyModule.Relay01.getId();
		}
		else if (moduleIdentifier.equals("MOTOR")){
			return PhyModule.Motor01.getId();
		}
		else if (moduleIdentifier.equals("BUTTON")){
			return PhyModule.PushButton01.getId();
		}
		else if (moduleIdentifier.equals("DOOR")){
			return PhyModule.Door01.getId();
		}
		else if (moduleIdentifier.equals("LIGHTSENSOR")){
			return PhyModule.LightSensor01.getId();
		}
		else if (moduleIdentifier.equals("RFID")){
			return PhyModule.RFIDSensor01.getId();
		}
		else if (moduleIdentifier.equals("THEFT")){
			return PhyModule.TheftDetection01.getId();
		}
		else{
			logWarn("Module identifier unknown: " + moduleIdentifier);
			return -1;
		}
	}
	
	int getFunctionID(String functionIdentifier) {
		if(functionIdentifier.equals("SET") ||
				functionIdentifier.equals("SETUP")){
			return SET;
		}
		else if(functionIdentifier.equals("START")){
			return START;
		}
		else if(functionIdentifier.equals("STOP")){
			return STOP;
		}
		else if(functionIdentifier.equals("READ")){
			return READ;
		}
		else if(functionIdentifier.equals("BLINK")){
			return BLINK;
		}
		else if(functionIdentifier.equals("RESULT")){
			return RESULT;
		}
		else{
			logWarn("Function identifier unknown: " + functionIdentifier);
			return -1;
		}
	}
	
	String encodeParameters(Value value) {
		if(value instanceof IntegerValue) {
			return ((IntegerValue)value).getInt() + "";
		} else if(value instanceof FloatValue) {
			return ((FloatValue)value).getFloat() + "";
		} else if(value instanceof BooleanValue) {
			return ((BooleanValue)value).getBoolean() ? "1" : "0";
		} else if(value instanceof RecordValue) {
			String[] names = ((RecordValue)value).getFieldNames();
			String result = "";
			
			for(String name : names) {
				Value field = ((RecordValue)value).getField(name);
				String encodedField = encodeParameters(field);
				
				if(encodedField == null)
					return null;
				
				if("".equals(result))
					result = encodedField;
				else if(!"".equals(encodedField))
					result += ", " + encodedField;
			}
			
			return result;
		} else {
			logWarn("Not encoded "+value+" of type "+value.getType().getName());
			return null;
		}
	}

	static String value(PhyModule fct, Object ...args) {
//		if (args[0] instanceof PhyModule) {
//			args[0] = ((PhyModule)args[0]).getId();
//		}
		return fct.getId()+COMMA_DELIM+StringUtil.join(args, COMMA_DELIM);
	}

	static String values(Object ...args) {
		return StringUtil.join(args, COMMA_DELIM);
	}

	static byte[] str2bytes(String message) {
		return message.getBytes(UTF8Charset);
	}

	static String bytes2str(byte[] encodedMessage) {
		return new String(encodedMessage, UTF8Charset).trim();
	}

	public TciCDProvided getCodec(RB RB, String encodingRule) {
		return this;
	}
}
