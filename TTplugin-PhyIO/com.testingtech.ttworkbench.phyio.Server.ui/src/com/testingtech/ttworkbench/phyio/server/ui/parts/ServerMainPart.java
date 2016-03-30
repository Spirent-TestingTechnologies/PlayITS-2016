package com.testingtech.ttworkbench.phyio.server.ui.parts;
 

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;

import javax.annotation.PostConstruct;
import javax.inject.Inject;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.testingtech.ttworkbench.phyio.server.ui.Utils.ServiceProvider;
import com.testingtech.tworkbench.ttman.server.api.IExecutionServer;

public class ServerMainPart {
	
	private Text textServerPath;
	private Text textWorkspacePath;
	private Label lblConnectionStatus;
	private int PORT=22782;
	
	private String PROJECTS_REQ = "getProjectsFromWorkspace";
	private String MODULES_REQ = "getModulesFromFolder";
	private String TESTCASES_REQ = "getTestcasesFromModule";
	private String ANNOT_VALUES_TESTCASE_REQ = "getAnnotationValuesForTestcase";
	private String ANNOT_VALUES_MODUL_REQ = "getAnnotationValuesForModul";
	private String WORKSPACE_REQ = "getWorkspacePath";

	private Process ttmanProcess;

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
				
		    	String serverPath = textServerPath.getText();
		    	String workspacePath = textWorkspacePath.getText();
						    	
		    	System.out.println("start server ...");
							
		    	try {
		    		//starts the server if your operating system is windows, change parameters when using linux
					ttmanProcess= (new ProcessBuilder( "cmd","/c","start", "cmd.exe","/k",serverPath,"--data",workspacePath)).start();
				} catch (IOException e1) {
					e1.printStackTrace();
				}
						    	
						    	
		    	if(ttmanProcess==null){
		    		System.out.println("serverProcess = null");
		    	} else{
		    		System.out.println("Process started");
		    	}
						    	
		    	//thread for server socket, necessary for the service provider
		    	Thread thread = new Thread(new Runnable() {
					@Override
					public void run() {
							
						try {
			    
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
				    		    		String modulAndAnnotation = br.readLine();	 //client sends modulename and annotation
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
				});
					
				thread.start();
			    	
			    
			}
		});
		
		btnStart.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 1, 1));
		btnStart.setText("Start");
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		
		lblConnectionStatus = new Label(parent, SWT.NONE);
		lblConnectionStatus.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 3, 1));
	}
	
}