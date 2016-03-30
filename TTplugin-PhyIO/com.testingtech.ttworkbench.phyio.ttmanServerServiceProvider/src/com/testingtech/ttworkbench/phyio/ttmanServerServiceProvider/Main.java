package com.testingtech.ttworkbench.phyio.ttmanServerServiceProvider;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class Main {

	private static int PORT=22782;
	
	private static String PROJECTS_REQ = "getProjectsFromWorkspace";
	private static String MODULES_REQ = "getModulesFromFolder";
	private static String TESTCASES_REQ = "getTestcasesFromModule";
	private static String ANNOT_VALUES_TESTCASE_REQ = "getAnnotationValuesForTestcase";
	private static String ANNOT_VALUES_MODUL_REQ = "getAnnotationValuesForModul";
	private static String WORKSPACE_REQ = "getWorkspacePath";
	
	public static void main(String[] args) {
		
		try {
			
	    	String workspacePath = "C:\\Users\\lassan\\Projekte\\Arduino\\git\\PlayITS-2016\\TTplugin-PhyIO\\com.testingtech.ttworkbench.phyio\\projects";

	    	while(true){
    			ServiceProvider provider = new ServiceProvider();
    			ServerSocket servsock = new ServerSocket(PORT);
    		      
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
    		    	  		provider.sendModuleNames(bWriter,workspacePath+"\\"+relPath);
    		    	  	}
    		    	} else if(request.endsWith(TESTCASES_REQ)){
    		    		String modName = br.readLine();				// client sends modulename
    		    	  	if(modName!=null){
    		    	  		provider.sendTestcases(bWriter,modName);
    		    	  	}
    		    	} else if(request.endsWith(ANNOT_VALUES_TESTCASE_REQ)){
		    	  		String modName = br.readLine();				// client sends modulename testcasename and annotation
		    	  		if(modName!=null) {
		    	  			provider.sendAnnotationValuesForTestcase(bWriter,modName);
		    	  		}
    		    	} else if(request.endsWith(ANNOT_VALUES_MODUL_REQ)) {
    		    		String modulAndAnnotation = br.readLine();	
    		    		if(modulAndAnnotation!=null) {
    		    			provider.sendAnnotationValuesForModul(bWriter, modulAndAnnotation);
    		    		}
    		    	} else if(request.endsWith(WORKSPACE_REQ)) {
    		    	  	provider.sendWorkspacePath(bWriter, workspacePath);
    		    	} else if(request.endsWith(PROJECTS_REQ)) {
    		    	  	provider.sendProjectNames(bWriter, workspacePath);
    		    	 }
    		    	  	
    		      }
    		      sock.close();
    		      servsock.close();
	    	}
	    	
		} catch(Exception ex) {
			ex.printStackTrace();
		} 
	}
	
}
