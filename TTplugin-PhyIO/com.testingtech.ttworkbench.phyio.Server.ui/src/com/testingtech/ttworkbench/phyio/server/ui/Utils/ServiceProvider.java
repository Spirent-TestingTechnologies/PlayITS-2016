package com.testingtech.ttworkbench.phyio.server.ui.Utils;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
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

public class ServiceProvider implements Runnable{
	
	private String TTCN3_NATURE = "com.testingtech.ttworkbench.core.ttcn3nature";
	private boolean serverIsRunning ;
	private Map<String,TestModule> moduleMap ;
	private String workspacePath;
	private int port;
	
	String SEPERATOR= "<SEP>";
	
	
	private String PROJECTS_REQ = "getProjectsFromWorkspace";
	private String MODULES_REQ = "getModulesFromFolder";
	private String TESTCASES_REQ = "getTestcasesFromModule";
	private String ANNOT_VALUES_TESTCASE_REQ = "getAnnotationValuesForTestcase";
	private String ANNOT_VALUES_MODUL_REQ = "getAnnotationValuesForModul";
	private String WORKSPACE_REQ = "getWorkspacePath";
	
	public ServiceProvider(String workspacePath, int port){
		
		
		moduleMap= new HashMap<String,TestModule>();
		this.workspacePath=workspacePath;
		this.port=port;
	}
	
	private void sendProjectNames(BufferedWriter bWriter, String workspacePath) {

		//check if the project in the workspace is a TTCN3 Project. Its a TTCN3 project if the project nature is set
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
										
										Node elemNode = element.getElementsByTagName("nature").item(0);
										if(elemNode!=null){
											String strNature=elemNode.getTextContent();
											if(strNature.equals(TTCN3_NATURE)) {
												bWriter.write(project.getName() + SEPERATOR);
											}
										}
										
										
									}
								}
								
							} catch (ParserConfigurationException e) {
								e.printStackTrace();
							} catch (SAXException e) {
								e.printStackTrace();
							} catch (IOException e) {
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
			e.printStackTrace();
		}
		
		
		
	}
	
	private void sendModuleNames(BufferedWriter bWriter,String path){

		List<TestModule> testModules = new ArrayList<TestModule>();
  		File folder = new File(path);
  		
		try {

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


	private void sendTestcases(BufferedWriter bWriter, String modName) {

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
	
	private void sendAnnotationValuesForTestcase(BufferedWriter bWriter,String data) {

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
	
	private void sendAnnotationValuesForModul(BufferedWriter bWriter,String data) {

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
	
	private void sendWorkspacePath(BufferedWriter bWriter, String workspacePath) {

		try {
			bWriter.write(workspacePath);
			bWriter.write("\n");
			bWriter.flush();
		} catch (IOException e) {
			System.out.println(e.getMessage());
		}
		
	}

	@Override
	public void run() {
		serverIsRunning=true;
	 
	    	
    	while(serverIsRunning){   
    		try{
    			ServerSocket servsock = new ServerSocket(port);

    			Socket sock = servsock.accept();
    		    InputStream is = sock.getInputStream();
    		    BufferedReader br = new BufferedReader(new InputStreamReader(is));
    			OutputStream os = sock.getOutputStream();
    			  
    			BufferedWriter bWriter = new BufferedWriter(new OutputStreamWriter(os));
    		    String request ;
    		    while((request = br.readLine())!=null){
    		    	if(request.equals(MODULES_REQ)){
    		    	  	String relPath = br.readLine();				// client sends relative path
    		    	  	if(relPath!=null){
    		    	  		sendModuleNames(bWriter,workspacePath+"\\"+relPath);
    		    	  	}
    		    	} else if(request.endsWith(TESTCASES_REQ)){
    		    		String modName = br.readLine();				// client sends modulename
    		    	  	if(modName!=null){
    		    	  		sendTestcases(bWriter,modName);
    		    	  	}
    		    	} else if(request.endsWith(ANNOT_VALUES_TESTCASE_REQ)){
		    	  		String modName = br.readLine();				// client sends modulename testcasename and annotation
		    	  		if(modName!=null) {
		    	  			sendAnnotationValuesForTestcase(bWriter,modName);
		    	  		}
    		    	} else if(request.endsWith(ANNOT_VALUES_MODUL_REQ)) {
    		    		String modulAndAnnotation = br.readLine();	
    		    		if(modulAndAnnotation!=null) {
    		    			sendAnnotationValuesForModul(bWriter, modulAndAnnotation);
    		    		}
    		    	} else if(request.endsWith(WORKSPACE_REQ)) {
    		    	  	sendWorkspacePath(bWriter, workspacePath);
    		    	} else if(request.endsWith(PROJECTS_REQ)) {
    		    	  	sendProjectNames(bWriter, workspacePath);
    		    	 }
    		    	  	
 
    		      }
    		      sock.close();
    		      servsock.close();
    		      
	    		
	    	}catch(SocketException se){	    		
	    	}catch(Exception ex) {
				ex.printStackTrace();
				return;
			}
    	}
	    	    		  
    				    
		
	}

	public void stop(){
		serverIsRunning= false;
	}

	public boolean isRunning() {
		return serverIsRunning;
	}
	
	

	
	
}
