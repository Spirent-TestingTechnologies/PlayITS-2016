package com.testingtech.ttworkbench.phyio.server.ui.parts;
 

import java.io.IOException;

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


	private Process ttmanProcess;

	private ServiceProvider serviceProvider;
	
	IExecutionServer client;
	

	@Inject
	public ServerMainPart() {
	}
	
	@PostConstruct
	public void postConstruct(Composite parent) {
		parent.setLayout(new GridLayout(3, false));
		
		Label lblTtmanServer = new Label(parent, SWT.NONE);
		lblTtmanServer.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false, 2, 1));
		lblTtmanServer.setText("TTman Server");
		new Label(parent, SWT.NONE);
		
		Label lblServerPath = new Label(parent, SWT.NONE);
		lblServerPath.setText("TTmanServer-Pfad:");
		
		textServerPath = new Text(parent, SWT.BORDER);
		textServerPath.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		
		Label lblWorkspacePath = new Label(parent, SWT.NONE);
		lblWorkspacePath.setText("Workspace-Pfad:");
		
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
						 
		    	serviceProvider = new ServiceProvider(workspacePath,PORT);
		    	//thread for server socket, necessary for the service provider
		    	Thread thread = new Thread(serviceProvider);
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
				if(serviceProvider.isRunning()){
					serviceProvider.stop();
				}
			}
		});
		btnStop.setText("Stop");
		
		lblConnectionStatus = new Label(parent, SWT.NONE);
		lblConnectionStatus.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 3, 1));

	}
	
}
