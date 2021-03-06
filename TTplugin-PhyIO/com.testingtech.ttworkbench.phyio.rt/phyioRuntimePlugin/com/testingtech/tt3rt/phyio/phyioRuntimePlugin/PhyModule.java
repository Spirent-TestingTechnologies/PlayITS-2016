package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

public enum PhyModule {
	GeneralConfig01(0),
	ColorView01(10),
	PingEcho01(20),
	PingEcho02(21),
	LED01(30),
	Relay01(40),
	Motor01(50),
	PushButton01(60),
	Door01(60),
	LightSensor01(70),
	RFIDSensor01(80),
	TheftDetection01(85);
	
	private int id;

	private PhyModule(int id) {
		this.id = id;
	}
	
	public int getId() {
		return id;
	}

	public static PhyModule valueFromId(int id) {
		for (PhyModule fct : values()) {
			if (fct.id == id) {
				return fct;
			}
		}
		return null;
	}
}