package com.testingtech.ttworkbench.phyio.server.ui.Utils;

import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class ServiceProvider {
	
	private String TTCN3_NATURE = "com.testingtech.ttworkbench.core.ttcn3nature";
	
	private Map<String,TestModule> moduleMap ;
	
	String SEPERATOR= "<SEP>";
	
	public ServiceProvider(){
		moduleMap= new HashMap<String,TestModule>();
	}
	
	public void sendProjectNames(BufferedWriter bWriter, String workspacePath) {
		
		File workspace = new File(workspacePath);
		if(workspace.exists() && workspace.isDirectory()) {
			for(File project : workspace.listFiles()) {
				if(project.isDirectory()) {
					for(File file : project.listFiles()) {
						if(file.getName().equals(".project")) {
							
							try {
								DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
								DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
								
								Document doc = dBuilder.parse(file);
								
								NodeList nodeList = doc.getElementsByTagName("natures");
								
								for(int i = 0; i < nodeList.getLength(); i++) {
									Node node = nodeList.item(i);
									if(node.getNodeType() == Node.ELEMENT_NODE) {
										Element element = (Element) node;
										
										String strNature = element.getElementsByTagName("nature").item(0).getTextContent();
										if(strNature.equals(TTCN3_NATURE)) {
											bWriter.write(project.getName() + SEPERATOR);
										}
										
									}
								}
								
							} catch (ParserConfigurationException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (SAXException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							
						}
					}
			
				}
			}
		}
		
		try {
			bWriter.newLine();
			bWriter.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
	}
	
	public void sendModuleNames(BufferedWriter bWriter,String path){
		List<TestModule> testModules = new ArrayList<TestModule>();
  		File folder = new File(path);
  		
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
			if(testModule!=null){
				for(Testcase testcase:testModule.getTestcases()){
					bWriter.write(testcase.getTestcaseID()+SEPERATOR);
				}
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
	
	public void sendAnnotationValuesForModul(BufferedWriter bWriter,String data) {
		String[] reqValues = data.split(SEPERATOR);
		try{
			if(reqValues.length==2){
				TestModule testModule = moduleMap.get(reqValues[0]);
				if(testModule!=null){
					
					String annotName = reqValues[1];
					List<String> values = testModule.getModuleAnnotations().get(annotName);
					
					if(values != null) {
						for(String value:values){
							bWriter.write(value+SEPERATOR);
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
	
	public void sendWorkspacePath(BufferedWriter bWriter, String workspacePath) {
		try {
			bWriter.write(workspacePath);
			bWriter.write("\n");
			bWriter.flush();
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}
		
	}
	
	
}
