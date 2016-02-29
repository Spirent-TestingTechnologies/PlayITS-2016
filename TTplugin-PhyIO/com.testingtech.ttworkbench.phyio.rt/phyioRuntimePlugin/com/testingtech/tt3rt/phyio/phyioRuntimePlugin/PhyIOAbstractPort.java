package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import org.etsi.ttcn.tri.TriAddress;
import org.etsi.ttcn.tri.TriAddressList;
import org.etsi.ttcn.tri.TriComponentId;
import org.etsi.ttcn.tri.TriException;
import org.etsi.ttcn.tri.TriMessage;
import org.etsi.ttcn.tri.TriParameter;
import org.etsi.ttcn.tri.TriParameterList;
import org.etsi.ttcn.tri.TriPortId;
import org.etsi.ttcn.tri.TriPortIdList;
import org.etsi.ttcn.tri.TriSignatureId;
import org.etsi.ttcn.tri.TriStatus;
import org.etsi.ttcn.tri.TriTestCaseId;

import com.testingtech.ttcn.tri.TriStatusImpl;
import com.testingtech.ttworkbench.tt3rt.rs232.port.RS232PortPlugin;

public abstract class PhyIOAbstractPort extends RS232PortPlugin {

	private static final long serialVersionUID = 1L;

	public PhyIOAbstractPort() {
		super();
	}

	protected abstract void clean();

	public TriStatus triSAReset() {
		clean();
		return super.triSAReset();
	}

	public TriStatus triExecuteTestcase(TriTestCaseId testCaseId, TriPortIdList tsiPorts) {
		clean();
		return super.triExecuteTestcase(testCaseId, tsiPorts);
	}

	public TriStatus triEndTestCase() {
		clean();
		return super.triEndTestCase();
	}

	public TriStatus triSendBC(TriComponentId componentId, TriPortId tsiPortId, TriMessage sendMessage) {
		return new TriStatusImpl("triSendBC not supported, please use send");
	}

	public TriStatus triSendMC(TriComponentId componentId, TriPortId tsiPortId, TriAddressList addresses, TriMessage sendMessage) {
		return new TriStatusImpl("triSendMC not supported, please use send");
	}

	public TriStatus triCall(TriComponentId componentId, TriPortId tsiPortId, TriAddress sutAddress, TriSignatureId signatureId,
			TriParameterList parameterList) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triCallBC(TriComponentId componentId, TriPortId tsiPortId, TriSignatureId signatureId, TriParameterList parameterList) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triCallMC(TriComponentId componentId, TriPortId tsiPortId, TriAddressList sutAddresses, TriSignatureId signatureId, TriParameterList parameterList) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triRaise(TriComponentId componentId, TriPortId tsiPortId, TriAddress sutAddress, TriSignatureId signatureId, TriException exception) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triRaiseBC(TriComponentId componentId, TriPortId tsiPortId, TriSignatureId signatureId, TriException exc) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triRaiseMC(TriComponentId componentId, TriPortId tsiPortId, TriAddressList sutAddresses, TriSignatureId signatureId, TriException exc) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triReply(TriComponentId componentId, TriPortId tsiPortId, TriAddress sutAddress, TriSignatureId signatureId, TriParameterList parameterList, TriParameter returnValue) {
		return new TriStatusImpl("this plugin requires message based communication");
	}

	public TriStatus triReplyBC(TriComponentId componentId, TriPortId tsiPortId, TriSignatureId signatureId,
			TriParameterList parameterList, TriParameter returnValue) {
		return new TriStatusImpl("triReplyBC not supported, please use triReply");
	}

	public TriStatus triReplyMC(TriComponentId componentId, TriPortId tsiPortId, TriAddressList sutAddresses, TriSignatureId signatureId, TriParameterList parameterList, TriParameter returnValue) {
		return new TriStatusImpl("triReplyMC not supported, please use triReply");
	}

	public void triEnqueueCall(TriPortId tsiPortId, TriAddress SUTaddress, TriComponentId componentId, TriSignatureId signatureId, TriParameterList parameterList) {
		throw new RuntimeException("this plugin requires message based communication");
	}

	public void triEnqueueException(TriPortId tsiPortId, TriAddress sutAddress, TriComponentId componentId, TriSignatureId signatureId, TriException exception) {
		throw new RuntimeException("this plugin requires message based communication");
	}

	public void triEnqueueReply(TriPortId tsiPortId, TriAddress address, TriComponentId componentId, TriSignatureId signatureId, TriParameterList parameterList, TriParameter returnValue) {
		throw new RuntimeException("this plugin requires supports message based communication, client only");
	}

}