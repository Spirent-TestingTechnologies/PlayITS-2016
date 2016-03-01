package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

import static com.testingtech.tt3rt.phyio.phyioRuntimePlugin.PhyModule.*;

public enum PhyPortKind {
	// TODO refine push button
	PhyPushButton(PushButton01), 
	// fixed
	PhyConfig(GeneralConfig01), PhyLED(LED01), PhyEcho(PingEcho01), PhyMotor(Motor01), PhyColorSensor(ColorView01), PhyRelay(Relay01), PhyLightSensor(LightSensor01);

	private PhyModule[] supportedFunctions;

	private PhyPortKind(PhyModule...supportedFunctions) {
		this.supportedFunctions = supportedFunctions;
	}

	public PhyModule[] getSupportedFunctions() {
		return supportedFunctions;
	}

	public boolean isSupportingFunction(int functionId) {
		for (PhyModule func : supportedFunctions) {
			if (func.getId() == functionId) {
				return true;
			}
		}
		return false;
	}
}