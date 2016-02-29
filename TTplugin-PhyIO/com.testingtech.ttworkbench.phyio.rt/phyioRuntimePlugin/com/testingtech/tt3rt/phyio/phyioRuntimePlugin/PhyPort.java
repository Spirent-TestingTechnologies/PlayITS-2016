package com.testingtech.tt3rt.phyio.phyioRuntimePlugin;

public class PhyPort {
	private final int id;
	private PhyPortKind kind;
	
	public PhyPort(int id, PhyPortKind kind) {
		this.id = id;
		this.kind = kind;
	}

	public int getId() {
		return id;
	}
	
	public PhyPortKind getKind() {
		return kind;
	}

	@Override
	public String toString() {
		return "PhyPort [id=" + id + ", kind=" + kind + "]";
	}
}
