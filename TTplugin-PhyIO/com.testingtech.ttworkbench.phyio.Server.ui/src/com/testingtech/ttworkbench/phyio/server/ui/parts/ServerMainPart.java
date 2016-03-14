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
			    			ServerSocket servsock = new ServerSocket(PORT);
			    		    while (true) {
				    		      Socket sock = servsock.accept();
				    		      InputStream is = sock.getInputStream();
				    		      BufferedReader br = new BufferedReader(new InputStreamReader(is));
				    		      String projectPath = br.readLine();
				    		      if(projectPath!=null){
				    		    	  	TestModule tm= new TestModule(workspacePath+"\\"+projectPath);
				    		    	  	sendTestInfo(tm,sock);
				    		      }
				    		      sock.close();
		
			    		    }
			    	}
			    		  
			    				    
			    } catch(Exception ex) {
			    	System.out.println(ex.getMessage());
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
	
	
	public void sendTestInfo(TestModule tm, Socket sock){

		
			List<Testcase> testcases = tm.getTestcases();
			Map<String,List<Testcase>> groups = sortByGroups(testcases);
			sendGroups(sock,groups);
			
			
	
	}
	
	private Map<String,List<Testcase>> sortByGroups(List<Testcase> testcases){
		Map<String,List<Testcase>> groups = new HashMap<String,List<Testcase>>();
		for(Testcase tc : testcases){
			// get all group annotations 
			List<String> groupNames = tc.getAnnotationValues(GROUPANNOT);
			
			// add test to all groups or create new group
			for(String groupName:groupNames){
				List<Testcase> group = groups.get(groupName);
				if(group==null){
					List<Testcase> newGroupList = new ArrayList<Testcase>();
					newGroupList.add(tc);
					groups.put(groupName, newGroupList);
				}else{
					group.add(tc);
				}
			}
			if(groupNames.isEmpty()){
				List<Testcase> group = groups.get(DEFAULT_GROUPNAME);
				if(group==null){
					List<Testcase> newGroupList = new ArrayList<Testcase>();
					newGroupList.add(tc);
					groups.put(DEFAULT_GROUPNAME, newGroupList);
				}else{
					group.add(tc);
				}
			}
		}
		return groups;
	}
	
	
	private void sendGroups(Socket sock,Map<String,List<Testcase>> groups){
	     OutputStream os;
		try {
			os = sock.getOutputStream();
			BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));	
			for(String groupName: groups.keySet()){
				br.write(GROUP_FLAG+groupName+"\n");
				List<Testcase> testcases = groups.get(groupName);
				for(Testcase tc:testcases){
					br.write(TEST_FLAG+tc.getTestcaseID()+"\n");
					List<String> title = tc.getAnnotationValues(TITEL_ANNOT);		
					if(title.isEmpty()){	
						br.write(TITLE_FLAG+tc.getTestcaseID()+"\n");
					}else{
						br.write(TITLE_FLAG+title.get(0)+"\n");
					}
					List<String> states = tc.getAnnotationValues(STATE_ANNOT);		
					for(String state:states){
						br.write(STATE_FLAG+state+"\n");
					}
				}	
				br.write(END_TEST_FLAG+"\n");
			}
			br.write(END_GROUP_FLAG+"\n");
			br.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	
}