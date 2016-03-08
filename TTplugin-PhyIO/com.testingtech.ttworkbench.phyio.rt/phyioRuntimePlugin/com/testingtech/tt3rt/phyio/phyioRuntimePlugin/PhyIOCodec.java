package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyIOConstants.*;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.ColorView01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.GeneralConfig01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.LED01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.LightSensor01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.Motor01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.PingEcho01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.PingEcho02;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.PushButton01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.RFIDSensor01;
import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.Relay01;

import java.nio.charset.Charset;

import org.etsi.ttcn.tci.BooleanValue;
import org.etsi.ttcn.tci.FloatValue;
import org.etsi.ttcn.tci.IntegerValue;
import org.etsi.ttcn.tci.RecordValue;
import org.etsi.ttcn.tci.TciCDProvided;
import org.etsi.ttcn.tci.TciTypeClass;
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
		Value value = decodingHypothesis.newInstance();
		
		String typeName = decodingHypothesis.getName();
		String str = bytes2str(message.getEncodedMessage());
		if (str.startsWith("#")) {
			return null;
		}
		
		String[] elements = str.split(COMMA_DELIM);
		int idx = 0;
		int moduleId = Integer.parseInt(elements[idx++].trim());
		int functionId = Integer.parseInt(elements[idx++].trim());
		PhyModule module = PhyModule.valueFromId(moduleId);
		switch (functionId) {
		case RESULT:
			if (module == PhyModule.PingEcho01 && "Distance".equals(typeName)) {
				int distance = Integer.parseInt(elements[idx++].trim());
				((FloatValue)value).setDouble(distance*1.0/100.0);
				return value;
			} else if (module == PhyModule.PingEcho02 && "DistanceSensorState".equals(typeName)) {
				int distance = Integer.parseInt(elements[idx++].trim());
				IntegerValue distanceV = (IntegerValue)((RecordValue)value).getField("distance");
				distanceV.setInt(distance);
				
				int timestamp = Integer.parseInt(elements[idx++].trim());
				IntegerValue timestampV = (IntegerValue)((RecordValue)value).getField("time");
				timestampV.setInt(timestamp);
				return value;
			} else if (module == PhyModule.ColorView01 && "RGB".equals(typeName)) {
				// format 10, 101, 81009, 114, 68, 188, 109, 120, 80
				//   CV01,R1,<timestamp:unit16>,<r:unit16>, <g:unit16>, <b:unit16>, <c:unit16>, <colorTemp:unit16>, <lux:unit16>
				int timestamp = Integer.parseInt(elements[idx++].trim());
				int r = Integer.parseInt(elements[idx++].trim());
				int g = Integer.parseInt(elements[idx++].trim());
				int b = Integer.parseInt(elements[idx++].trim());
				int c = Integer.parseInt(elements[idx++].trim());
				int colorTemp = Integer.parseInt(elements[idx++].trim());
				int lux = Integer.parseInt(elements[idx++].trim());
				int r256 = Integer.parseInt(elements[idx++].trim());
				int g256 = Integer.parseInt(elements[idx++].trim());
				int b256 = Integer.parseInt(elements[idx++].trim());
				RecordValue rv = (RecordValue) value;
				IntegerValue redField = (IntegerValue) rv.getField("red");
				IntegerValue greenField = (IntegerValue) rv.getField("green");
				IntegerValue blueField = (IntegerValue) rv.getField("blue");
				redField.setInt(r256);
				greenField.setInt(g256);
				blueField.setInt(b256);
				rv.setField("red", redField);
				rv.setField("green", greenField);
				rv.setField("blue", blueField);
				return value;
			} else if (module == PhyModule.LightSensor01 && "State".equals(typeName)) {
				int brightness = Integer.parseInt(elements[idx++].trim());
				IntegerValue brightnessV = (IntegerValue)((RecordValue)value).getField("brightness");
				brightnessV.setInt(brightness);
				
				int timestamp = Integer.parseInt(elements[idx++].trim());
				IntegerValue timestampV = (IntegerValue)((RecordValue)value).getField("time");
				timestampV.setInt(timestamp);
			} else if (module == PhyModule.LightSensor01 && "Frequency".equals(typeName)) {
				int frequency = Integer.parseInt(elements[idx++].trim());
				((IntegerValue)value).setInt(frequency);
			} else if (module == PhyModule.PushButton01 && "PushButtonState".equals(typeName)) {
				int state = Integer.parseInt(elements[idx++].trim());
				((BooleanValue)value).setBoolean(state != 0);
			} else if (module == PhyModule.Door01 && "DoorState".equals(typeName)) {
				int state = Integer.parseInt(elements[idx++].trim());
				((BooleanValue)value).setBoolean(state != 0);
			} else if (module == PhyModule.RFIDSensor01 && "RFID".equals(typeName)) {
				int id1 = Integer.parseInt(elements[idx++].trim());
				int id2 = Integer.parseInt(elements[idx++].trim());
				int id3 = Integer.parseInt(elements[idx++].trim());
				int id4 = Integer.parseInt(elements[idx++].trim());
				
				RecordValue rValue = ((RecordValue)value);
				((IntegerValue)rValue.getField("id1")).setInt(id1);
				((IntegerValue)rValue.getField("id2")).setInt(id2);
				((IntegerValue)rValue.getField("id3")).setInt(id3);
				((IntegerValue)rValue.getField("id4")).setInt(id4);
			}
			break;

		default:
			break;
		}
		if (decodingHypothesis.getTypeClass() == TciTypeClass.INTEGER) {
			((IntegerValue)value).setInt(Integer.parseInt(new String(message.getEncodedMessage())));
			return value;
		}
		logWarn("Could not decode: "+str);
		return null;
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
