package com.testingtech.ttworkbench.phyio.server.ui.Utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

public class ServiceProvider {
	
	private Map<String,TestModule> moduleMap ;
	
	String SEPERATOR= "<SEP>";
	
	public ServiceProvider(){
		moduleMap= new HashMap<String,TestModule>();
	}
	

	public void sendModuleNames(BufferedWriter bWriter,String path){
		List<TestModule> testModules = new ArrayList<TestModule>();
  		File folder = new File(path);
  		
		OutputStream os;
		BufferedWriter br;
		try {
;	

	  		if(folder!=null && folder.isDirectory()){  		
	  			testModules = getModules(folder);
	  		}
	  		for(TestModule testModule: testModules){
	  			bWriter.write(testModule.getName()+SEPERATOR);
	  		}
	  		bWriter.newLine();
	  		bWriter.flush();
			
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}
		
		
  		
	}
	
	private List<TestModule> getModules(File parent){
		List<TestModule> foundModules = new ArrayList<TestModule>();
		for(File child : parent.listFiles()){
			if(child.isDirectory()){
				foundModules.addAll(getModules(child));
				continue;
			}
			String extension ="";
			int i = child.getName().lastIndexOf('.');
			if (i > 0) {
			    extension = child.getName().substring(i+1);
			}
			if(extension.equals("ttcn3")){
				TestModule newModule = new TestModule(child.getAbsolutePath());
				foundModules.add(newModule);
				moduleMap.put(newModule.getName(),newModule);
				
			}
		}
		return foundModules;
	}


	public void sendTestcases(BufferedWriter bWriter, String modName) {

		try{
			if(modName==null||modName.isEmpty()){
				bWriter.write("\n");
				bWriter.flush();
			}
			TestModule testModule = moduleMap.get(modName);
			for(Testcase testcase: testModule.getTestcases()){
				bWriter.write(testcase.getTestcaseID()+SEPERATOR);
			}
			bWriter.write("\n");
			bWriter.flush();
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}			
	}
	
	public void sendAnnotationValuesForTestcase(BufferedWriter bWriter,String data) {
		String[] reqValues = data.split(SEPERATOR);
		try{
			if(reqValues.length==3){
				TestModule testModule = moduleMap.get(reqValues[0]);
				if(testModule!=null){
					// find testcase
					for(Testcase testcase: testModule.getTestcases()){					
						if(testcase.getTestcaseID().equals(reqValues[1])){
							
							//write values to socket
							List<String> values = testcase.getAnnotationValues(reqValues[2]);
							for(String value:values){
								bWriter.write(value+SEPERATOR);
							}
						}
					}
				}
			}		
			bWriter.write("\n");
			bWriter.flush();
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}			
	}
	
	
}
