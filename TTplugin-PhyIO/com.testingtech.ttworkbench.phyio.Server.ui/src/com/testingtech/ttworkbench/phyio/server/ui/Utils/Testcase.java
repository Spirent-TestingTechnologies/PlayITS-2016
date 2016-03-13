package com.testingtech.ttworkbench.phyio.server.ui.Utils;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class Testcase {
	
	private String testcaseID;
	private Map<String, List<String>> parameters;
	private Map<String, List<String>> annotations;
	
	
	public Testcase(String testcaseID, Map<String, List<String>> parameters, Map<String, List<String>> annotations){
		this.testcaseID=testcaseID;
		this.parameters=parameters;
		this.annotations=annotations;

	}
	
	
	public String getTestcaseID(){
		return testcaseID;
	}
	
	public List<String> getAnnotationValues(String annotKey){
		List<String> values =annotations.get(annotKey);
		if(values==null){
			return new ArrayList<String>();
		}else{
			return values;
		}
	}

}
