package SerialPortPlugin;

import java.nio.charset.Charset;
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
		return null; //String(message.getEncodedMessage(), "UTF-8");
	}

	@Override
	public TriMessage encode(Value value) {
		
		TriMessage message;
		
//		String typeEncoding = value.getType().getTypeEncoding(); // ""
		
		
//		String byteMessage = value.getType().getTypeEncodingVariant();
		System.out.print("encode : ");
		System.out.println("Wir kodieren was");
		System.out.print("encode : ");
		System.out.println(value);
		System.out.print("encode : ");
		System.out.println(value.getType());
		System.out.print("encode as string: ");
		System.out.println(value.toString());
		
//		return TriMessageImpl.valueOf(byteMessage.getBytes(Charset.forName("UTF-8")));
		
		if(value.getType().getName() == "SerialConfig"){
			String mes = value.toString();
			mes = mes.replaceAll("\"", "");
			mes = mes.replaceAll("BaudRate := ", "");
			mes = mes.replaceAll(" }", "");
			mes = mes.substring(14, mes.length());
			System.out.println(mes);
			message = TriMessageImpl.valueOf(mes.getBytes(Charset.forName("UTF-8")));
		}
		else{
			message = TriMessageImpl.valueOf(value.toString().getBytes(Charset.forName("UTF-8")));
		}
		return message;
	}

	public TciCDProvided getCodec(RB RB, String encodingRule) {
		return this;
	}

}
