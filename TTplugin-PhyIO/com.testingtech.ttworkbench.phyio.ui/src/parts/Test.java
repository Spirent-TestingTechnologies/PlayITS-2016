package parts;
 

import java.io.IOException;
import java.net.ConnectException;
import java.text.MessageFormat;

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

import arduinocontrol.ArduinoControl;

import com.testingtech.tworkbench.ttman.server.api.AuthenticationException;
import com.testingtech.tworkbench.ttman.server.api.Credentials;
import com.testingtech.tworkbench.ttman.server.api.ExecutionServerFactory;
import com.testingtech.tworkbench.ttman.server.api.IExecutionServer;

public class Test {
	
	private Text textHost;
	private Text textPort;
	private Label lblConnectionStatus;
	
	IExecutionServer client;
	
	@Inject
	public Test() {
		//TODO Your code here
	}
	
	@PostConstruct
	public void postConstruct(Composite parent) {
		parent.setLayout(new GridLayout(3, false));
		
		Label lblTtmanServer = new Label(parent, SWT.NONE);
		lblTtmanServer.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, false, false, 2, 1));
		lblTtmanServer.setText("TTman Server");
		new Label(parent, SWT.NONE);
		
		Label lblHost = new Label(parent, SWT.NONE);
		lblHost.setText("Host:");
		
		textHost = new Text(parent, SWT.BORDER);
		textHost.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		
		Label lblPort = new Label(parent, SWT.NONE);
		lblPort.setText("Port:");
		
		textPort = new Text(parent, SWT.BORDER);
		textPort.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		
		lblConnectionStatus = new Label(parent, SWT.NONE);
		lblConnectionStatus.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 3, 1));
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		new Label(parent, SWT.NONE);
		
		Button btnConnect = new Button(parent, SWT.NONE);
		btnConnect.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				String host = null;
				Integer port = null;
				
			    try {
			      host = textHost.getText();
				  port = Integer.parseInt(textPort.getText());
				  Credentials credentials = new Credentials("", "");
			      // bind the client callback handler to the given address and port
			      client = new ExecutionServerFactory().createClient();

			      // connect to the server and define a callback handler
			      System.out.println(MessageFormat.format("Client connects to the Execution Server running at {0}:{1}", host, port));
			      client.connect(host, port, credentials, new ArduinoControl());
			      
			    } catch(Exception ex) {
			    	lblConnectionStatus.setText(ex.getMessage());
			    	return;
			    } 
			    
			    lblConnectionStatus.setText("Successfully connected to TTman Server " + host + ":" + port);
			}
		});
		btnConnect.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, false, false, 2, 1));
		btnConnect.setText("Connect");
		
		Button btnDisconnect = new Button(parent, SWT.NONE);
		btnDisconnect.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				if(client != null) {
					try {
						client.disconnect();
					} catch (IOException e1) {
						lblConnectionStatus.setText(e1.getMessage());
						return;
					}
				}
				
				lblConnectionStatus.setText("Disconnected");
			}
		});
		btnDisconnect.setText("Disconnect");
		//TODO Your code here
	}
	
	
	
	
}