package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyIOConstants.*;

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
				IntegerValue timestampV = (IntegerValue)((RecordValue)value).getField("timestamp");
				timestampV.setInt(timestamp);
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
		String typeName = value.getType().getName();
		String outStr = null;
		
		if ("SwitchLED".equals(typeName)) {
			int on = ((BooleanValue)value).getBoolean() ? 1 : 0;
			outStr = value(PhyModule.LED01, SET, on);
		
		} else if ("BlinkLEDStart".equals(typeName)) {
			outStr = value(PhyModule.LED01, BLINK);
			
		} else if ("BlinkLEDStop".equals(typeName)) {
			outStr = value(PhyModule.LED01, STOP);
		
		} else if ("RelaySwith".equals(typeName)) {
			int on = ((BooleanValue)value).getBoolean() ? 1 : 0;
			outStr = value(PhyModule.Relay01, SET, on);
		
		} else if ("Velocity".equals(typeName)) {
			int velocity = ((IntegerValue)value).getInt();
			outStr = value(PhyModule.Motor01, SET, velocity);
		
		} else if ("ReadDistance".equals(typeName)) {
			outStr = value(PhyModule.PingEcho01, READ);
		
		} else if ("ReadButtonState".equals(typeName)) {
			outStr = value(PhyModule.PushButton01, READ);
			
		} else if ("ReadButtonStateStart".equals(typeName)
				|| "ReadDoorStart".equals(typeName)) {
			outStr = value(PhyModule.PushButton01, START);
			
		} else if ("ReadButtonStateStop".equals(typeName)
				|| "ReadDoorStop".equals(typeName)) {
			outStr = value(PhyModule.PushButton01, STOP);

		} else if ("ReadRGB".equals(typeName)) {
			outStr = value(PhyModule.ColorView01, READ);
			
		} else if ("ReadBrightness".equals(typeName)) {
			outStr = value(PhyModule.LightSensor01, READ);
			
		} else if ("RFID".equals(typeName)) {
			RecordValue rValue = (RecordValue)value;
			int id1 = ((IntegerValue)rValue.getField("id1")).getInt();
			int id2 = ((IntegerValue)rValue.getField("id2")).getInt();
			int id3 = ((IntegerValue)rValue.getField("id3")).getInt();
			int id4 = ((IntegerValue)rValue.getField("id4")).getInt();
			outStr = value(PhyModule.RFIDSensor01, SET, id1, id2, id3, id4);
			
		// map parameters
		} else if (
				value.getType().getTypeClass() == TciTypeClass.INTEGER ||
				value.getType().getTypeClass() == TciTypeClass.FLOAT) {
			outStr = value(PhyModule.LED01, value);
		}

		if (outStr != null) {
			return TriMessageImpl.valueOf(str2bytes(outStr));
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
