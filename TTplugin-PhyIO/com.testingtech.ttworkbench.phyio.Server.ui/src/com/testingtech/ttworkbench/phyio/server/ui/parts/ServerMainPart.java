package com.testingtech.ttworkbench.phyio.server.ui.parts;
 

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.inject.Inject;
import javax.annotation.PostConstruct;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;

import com.testingtech.ttworkbench.phyio.server.ui.Utils.ServerOutputStream;
import com.testingtech.ttworkbench.phyio.server.ui.Utils.ServiceProvider;
import com.testingtech.ttworkbench.phyio.server.ui.Utils.TestModule;
import com.testingtech.ttworkbench.phyio.server.ui.Utils.Testcase;
import com.testingtech.tworkbench.ttman.server.api.ExecutionServerFactory;
import com.testingtech.tworkbench.ttman.server.api.IExecutionHandler;
import com.testingtech.tworkbench.ttman.server.api.IExecutionServer;
import com.testingtech.tworkbench.ttman.server.api.ITERequest;
import com.testingtech.tworkbench.ttman.server.api.ITEResponse;
import com.testingtech.tworkbench.ttman.server.api.Job;
import com.testingtech.tworkbench.ttman.server.api.Parameter;
import com.testingtech.tworkbench.ttman.server.api.TestCase;
import com.testingtech.tworkbench.ttman.server.api.TestCaseStatus;

public class ServerMainPart {
	
	private Text textServerPath;
	private Text textWorkspacePath;
	private Label lblConnectionStatus;
	private int PORT=22782;
	
	private String GROUPANNOT= "group";
	private String TITEL_ANNOT= "shortdesc";
	private String STATE_ANNOT= "state";
	
	private String PROJECTS_REQ = "getProjectsFromWorkspace";
	private String MODULES_REQ = "getModulesFromFolder";
	private String TESTCASES_REQ = "getTestcasesFromModule";
	private String ANNOT_VALUES_TESTCASE_REQ = "getAnnotationValuesForTestcase";
	private String ANNOT_VALUES_MODUL_REQ = "getAnnotationValuesForModul";
	private String WORKSPACE_REQ = "getWorkspacePath";



	private String DEFAULT_GROUPNAME= "tests";
	private String GROUP_FLAG= "<G>";
	private String TEST_FLAG= "<TC>";
	private String TITLE_FLAG= "<TI>";
	private String STATE_FLAG= "<ST>";
	
	private String END_GROUP_FLAG= "</G>";
	private String END_TEST_FLAG= "</TC>";

	private Process ttmanProcess;
	private boolean serverIsRunning;

	
	IExecutionServer client;
	private Text text;
	
	@Inject
	public ServerMainPart() {
		//TODO Your code here
	}
	
	@PostConstruct
	public void postConstruct(Composite parent) {
		parent.setLayout(new GridLayout(3, false));
		
		Label lblTtmanServer = new Label(parent, SWT.NONE);
		lblTtmanServer.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false, 2, 1));
		lblTtmanServer.setText("TTman Server");
		new Label(parent, SWT.NONE);
		
		Label lblServerPath = new Label(parent, SWT.NONE);
		lblServerPath.setText("Pfad:");
		
		textServerPath = new Text(parent, SWT.BORDER);
		textServerPath.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		
		Label lblWorkspacePath = new Label(parent, SWT.NONE);
		lblWorkspacePath.setText("Workspace:");
		
		textWorkspacePath = new Text(parent, SWT.BORDER);
		textWorkspacePath.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		
		Button btnStart = new Button(parent, SWT.NONE);
		btnStart.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				
				
//			    	IExecutionServer server = new ExecutionServerFactory().createLocalServer(null);
			    	
			    	//Process ttmanProcess= (new ProcessBuilder( "cmd","/c","start", "cmd.exe","/k",serverPath,"--data",workspacePath)).start();
					Thread thread = new Thread(new Runnable() {
						@Override
						public void run() {
							
							try {
								
//						    	serverPath = textServerPath.getText();
//						    	workspacePath = textWorkspacePath.getText();
						    	
						    	String serverPath = "C:\\Program Files\\TTworkbenchProfessional\\TTmanServer.bat";
						    	//workspacePath = "C:\\Users\\lassan\\Projekte\\Arduino\\realization\\workspace";
						    	String workspacePath = "C:\\Users\\lassan\\Projekte\\Arduino\\git\\PlayITS-2016\\TTplugin-PhyIO\\com.testingtech.ttworkbench.phyio\\projects";
//						    	workspacePath = "C:\\Users\\lassan\\Projekte\\Arduino\\git_2015";
//						    	workspacePath = "C:\\Users\\lassan\\Desktop\\workspace";
						    	
						    	System.out.println("start server ...");
							
						    	ttmanProcess= (new ProcessBuilder( "cmd","/c","start", "cmd.exe","/k",serverPath,"--data",workspacePath)).start();
//						    	ttmanProcess = Runtime.getRuntime().exec(serverPath + " --data " + workspacePath);
						    	
						    	if(ttmanProcess==null){
						    		System.out.println("serverProcess = null");
						    	} else{
						    		System.out.println("Process started");
						    		serverIsRunning = true;
						    	}
			    
						    	while(serverIsRunning){
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
					    		    	  	
					    		    	  
				//				    		    	  	TestModule tm= new TestModule(workspacePath+"\\"+projectPath);
				//				    		    	  	sendTestInfo(tm,sock);
					    		      }
					    		      sock.close();
						    	}
						    	
						    	ttmanProcess.destroy();
					    		  
					    				    
							} catch(Exception ex) {
								ex.printStackTrace();
							} 
						}
					});
					
					thread.start();
			    	
			    
			}
		});
		
		btnStart.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1));
		btnStart.setText("Start");
		new Label(parent, SWT.NONE);
		
		Button btnStop = new Button(parent, SWT.NONE);
		btnStop.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if(ttmanProcess!=null){
					serverIsRunning = false;
				}
			}
		});
		btnStop.setText("Stop");
		
		lblConnectionStatus = new Label(parent, SWT.NONE);
		lblConnectionStatus.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 3, 1));
		
		text = new Text(parent, SWT.BORDER | SWT.READ_ONLY | SWT.WRAP | SWT.V_SCROLL);
		text.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 3, 1));
		//TODO Your code here
	}
	
}