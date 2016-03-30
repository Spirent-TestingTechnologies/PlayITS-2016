package com.testingtech.ttworkbench.phyio.server.ui.Utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TestModule {
	
	private String TESTCASE= "testcase";
	private String MODULE= "module";
	
	private final String ANNOT_REGEX ="^\\s*/*\\*/*\\s*@(\\w+)\\s*(.*)";

	private List<Testcase> testcases;
	private Map<String,List<String>> moduleAnnotations ;
	
	private File file;
	
	public TestModule(String path){
		this.file = new File(path);
		if(file==null){
			System.out.println("File not found");
			return;
		}
		moduleAnnotations = new HashMap<String,List<String>>();
		parseFile();
	}
	
	public void parseFile(){
		Pattern annotPattern = Pattern.compile(ANNOT_REGEX);
		testcases = new ArrayList<Testcase>();
		FileReader input;
		
		try {
			String myLine = null;
			input = new FileReader(file.getAbsolutePath());
			BufferedReader bufRead = new BufferedReader(input);

			while ( (myLine = bufRead.readLine()) != null)
			{    
		    	
				myLine=myLine.trim();

			    if(myLine.startsWith("/**")){
			    	String tcID= null;
			    	LinkedHashMap<String, String> parameter = new LinkedHashMap<String, String>();
			    	Map<String, List<String>> annotations = new HashMap<String, List<String>>();			    	

		    	
			    	//start parsing
			    	if(myLine.length()>3){
			    		myLine = myLine.substring(2);
			    	}else{
			    		 myLine=bufRead.readLine();
			    		 if(myLine!=null){
			    			 myLine=myLine.trim();
			    		 }
			    	}
			    	
			    	// parse all annotations
			    	while(myLine!=null && myLine.startsWith("*")){
			    		Matcher matcher = annotPattern.matcher(myLine);
				    	if(matcher.find()){					
				    		String annotName= matcher.group(1);
				    		String annotValue= matcher.group(2);
			    			System.out.println(annotName+":"+annotValue);
	
				    		List<String> oldValues = annotations.get(annotName);
				    		if(oldValues==null){
				    			List<String> annotValueList = new ArrayList<String>();
				    			annotValueList.add(annotValue);
				    			annotations.put(annotName, annotValueList);
				    		}else{
				    			annotations.get(annotName).add(annotValue);
				    		}
				    	}
				    	myLine=bufRead.readLine();
		    			myLine=myLine.trim();
				    }

		    	
		    	
			    	// parse testcase declaration 
			    	if(myLine!=null && myLine.startsWith(TESTCASE)){
				    	Scanner s2 = new Scanner(myLine);
				    	s2.useDelimiter("\\(|\\)|\\s");
				    	
				    	if(s2.hasNext()){				    		
				    		
				    		s2.next();			// testcase 
				    		if(s2.hasNext()){
				    			tcID = s2.next();				    			
				    			
				    			// get parameters
				    			while(s2.hasNext()){
				    				String varType = s2.next();
				    				if(varType.isEmpty()){break;}		//empty brackets
				    				if(s2.hasNext()){
				    					String varName =s2.next();			    				
				    					parameter.put(varType, varName);
				    					
				    				}
				    			}
				    		}
				    	}
				    	s2.close();
			    	
			    	
				    }
			    	// parse testcase declaration 
			    	if(myLine!=null && myLine.startsWith(MODULE)){
			    		moduleAnnotations = annotations;
			    		
			    	}
			    	if(tcID!=null){
			    		testcases.add(new Testcase(tcID, parameter,annotations));
			    	}
			    }
			}
			bufRead.close(); 
		
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	
	public List<Testcase> getTestcases(){
		return new ArrayList<Testcase>(testcases);
	}
	
	public String getName(){
		return file.getName();
	}

	public Map<String, List<String>> getModuleAnnotations() {
		return moduleAnnotations;
	}

	



}
