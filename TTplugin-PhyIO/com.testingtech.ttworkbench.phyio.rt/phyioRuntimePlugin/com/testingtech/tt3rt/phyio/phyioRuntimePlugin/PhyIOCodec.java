package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyIOConstants.*;

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
import com.testingtech.util.StringUtil;

import de.tu_berlin.cs.uebb.muttcn.runtime.RB;

public class PhyIOCodec extends AbstractCodecPlugin implements CodecProvider {

	private static final String COMMA_DELIM = ",";
	private static final Charset UTF8Charset = Charset.forName("UTF-8");

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
		int idx = 0;
		int moduleIdMessage = Integer.parseInt(elements[idx++].trim());
		int functionIdMessage = Integer.parseInt(elements[idx++].trim());
		
		if(moduleIdMessage != moduleIdHypothesis || functionIdMessage != RESULT)
			return null;
		
		return decodeParameters(value, elements, idx);
	}
	
	Value decodeParameters(Value value, String[] elements, Integer idx)
	{
		if(value instanceof IntegerValue){
			int v = Integer.parseInt(elements[idx++].trim());
			((IntegerValue)value).setInt(v);
		} else if(value instanceof FloatValue){
			float v = Float.parseFloat(elements[idx++].trim());
			((FloatValue)value).setFloat(v);
		} else if(value instanceof BooleanValue){
			int v = Integer.parseInt(elements[idx++].trim());
			((BooleanValue)value).setBoolean(v != 0);
		} else if(value instanceof RecordValue){
			String[] names = ((RecordValue)value).getFieldNames();
			
			for(String name : names) {
				Value field = ((RecordValue)value).getField(name);
				decodeParameters(field, elements, idx);
			}
		} else {
			logWarn("Could not decode: type " + value.getType().getName() + " not supported.");
			return null;
		}
		
		return value;
	}

	@Override
	public TriMessage encode(Value value) {
		String moduleFunction = translateVariant(value.getType().getTypeEncodingVariant());	
		String parameters = encodeParameters(value);
		
		if (moduleFunction != null && parameters != null) {
			String out = moduleFunction;
			if(!parameters.equals(""))
				out += ", " + parameters;
			return TriMessageImpl.valueOf(str2bytes(out));
		} else {
			return null;
		}
	}
	
	String translateVariant(String variant) {
		String[] elements = variant.split(COMMA_DELIM);
		
		int module = getModuleID(elements[0].trim());
		int function = getFunctionID(elements[1].trim());
		
		if(module == -1 || function == -1)
			return null;
		
		return module + ", " + function;
	}
	
	int getModuleID(String moduleIdentifier) {
		switch(moduleIdentifier){
		case "CONFIG":
			return PhyModule.GeneralConfig01.getId();
		case "COLOR":
			return PhyModule.ColorView01.getId();
		case "ECHO":
			return PhyModule.PingEcho01.getId();
		case "DISTANCE":
			return PhyModule.PingEcho02.getId();
		case "LED":
			return PhyModule.LED01.getId();
		case "RELAY":
			return PhyModule.Relay01.getId();
		case "MOTOR":
			return PhyModule.Motor01.getId();
		case "BUTTON":
			return PhyModule.PushButton01.getId();
		case "DOOR":
			return PhyModule.Door01.getId();
		case "LIGHTSENSOR":
			return PhyModule.LightSensor01.getId();
		case "RFID":
			return PhyModule.RFIDSensor01.getId();
		default:
			logWarn("Module identifier unknown: " + moduleIdentifier);
			return -1;
		}
	}
	
	int getFunctionID(String functionIdentifier) {
		switch(functionIdentifier){
		case "SETUP":
		case "SET":
			return SET;
		case "START":
			return START;
		case "STOP":
			return STOP;
		case "READ":
			return READ;
		case "BLINK":
			return BLINK;
		case "RESULT":
			return RESULT;
		default:
			logWarn("Function identifier unknown: " + functionIdentifier);
			return -1;
		}
	}
	
	String encodeParameters(Value value) {
		if(value instanceof IntegerValue){
			return ((IntegerValue)value).getInt() + "";
		} else if(value instanceof FloatValue){
			return ((FloatValue)value).getFloat() + "";
		} else if(value instanceof BooleanValue){
			return ((BooleanValue)value).getBoolean() ? "1" : "0";
		} else if(value instanceof RecordValue){
			String[] names = ((RecordValue)value).getFieldNames();
			String result = "";
			
			for(String name : names) {
				Value field = ((RecordValue)value).getField(name);
				String encodedField = encodeParameters(field);
				
				if(result.equals(""))
					result = encodedField;
				else if(!encodedField.equals(""))
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
