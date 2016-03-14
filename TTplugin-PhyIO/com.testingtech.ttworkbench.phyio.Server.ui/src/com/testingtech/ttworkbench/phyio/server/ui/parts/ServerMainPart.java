package com.testingtech.ttworkbench.phyio.server.ui.parts;
 

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
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

import com.testingtech.ttworkbench.phyio.server.ui.Utils.ServiceProvider;
import com.testingtech.ttworkbench.phyio.server.ui.Utils.TestModule;
import com.testingtech.ttworkbench.phyio.server.ui.Utils.Testcase;
import com.testingtech.tworkbench.ttman.server.api.IExecutionServer;

public class ServerMainPart {
	
	private Text textServerPath;
	private Text textWorkspacePath;
	private Label lblConnectionStatus;
	private int PORT=22782;
	
	private String GROUPANNOT= "group";
	private String TITEL_ANNOT= "shortdesc";
	private String STATE_ANNOT= "state";
	
	private String MODULES_REQ = "getModulesFromFolder";
	private String TESTCASES_REQ = "getTestcasesFromModule";
	private String ANNOT_VALUES_REQ = "getAnnotationValuesForTestcase";



	private String DEFAULT_GROUPNAME= "tests";
	private String GROUP_FLAG= "<G>";
	private String TEST_FLAG= "<TC>";
	private String TITLE_FLAG= "<TI>";
	private String STATE_FLAG= "<ST>";
	
	private String END_GROUP_FLAG= "</G>";
	private String END_TEST_FLAG= "</TC>";
	






	
	private Process ttmanProcess;

	
	IExecutionServer client;
	
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
		
		lblConnectionStatus = new Label(parent, SWT.NONE);
		lblConnectionStatus.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 3, 1));
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		
		Button btnStart = new Button(parent, SWT.NONE);
		btnStart.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				String serverPath =null;
				String workspacePath =null;
				
			    try {		    	
				
			    	
			    	serverPath = textServerPath.getText();
			    	workspacePath = textWorkspacePath.getText();
			    	System.out.println("start server ...");
			    			    
				
			    	
			    	Process ttmanProcess= (new ProcessBuilder( "cmd","/c","start", "cmd.exe","/k",serverPath,"--data",workspacePath)).start();
					
					

			    	if(ttmanProcess==null){
			    		System.out.println("serverProcess = null");
			    	}else{
			    		System.out.println("Process started");

			    	}
	    
			    	while(true){
			    			ServiceProvider provider = new ServiceProvider();
			    			ServerSocket servsock = new ServerSocket(PORT);
			    		    while (true) {
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
				    		    	  	}else if(request.endsWith(TESTCASES_REQ)){
				    		    	  		String modName = br.readLine();				// client sends modulename
				    		    	  		if(modName!=null){
				    		    	  			provider.sendTestcases(bWriter,modName);
				    		    	  		}
				    		    	  	}else if(request.endsWith(ANNOT_VALUES_REQ)){
				    		    	  		String modName = br.readLine();				// client sends modulename testcasename and annotation
				    		    	  		if(modName!=null){
				    		    	  			provider.sendAnnotationValuesForTestcase(bWriter,modName);
				    		    	  		}
				    		    	  	}
				    		    	  	
				    		    	  
//				    		    	  	TestModule tm= new TestModule(workspacePath+"\\"+projectPath);
//				    		    	  	sendTestInfo(tm,sock);
				    		      }
				    		      sock.close();
		
			    		    }
			    	}
			    		  
			    				    
			    } catch(Exception ex) {
			    	ex.printStackTrace();
			    } 
			    
			}
		});
		
		btnStart.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 2, 1));
		btnStart.setText("Start");
		
		Button btnStop = new Button(parent, SWT.NONE);
		btnStop.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if(ttmanProcess!=null){
					ttmanProcess.destroy();;
				}
			}
		});
		btnStop.setText("Stop");
		//TODO Your code here
	}
	
}