package com.testingtech.ttworkbench.phyio.server.ui.Utils;



public class Main {
	
	private static int PORT=22782;


	public static void main(String[] args) {
		if(args.length<1){
			System.out.println("Please enter the workspace path");
			return;
		}
		
    	String workspacePath = args[0];
    	System.out.println("start ServiceProvider at: "+workspacePath);

    	ServiceProvider serviceProvider = new ServiceProvider(workspacePath,PORT);

	    	
		Thread thread = new Thread(serviceProvider );
		thread.start();
		System.out.println("Server started...");
	
    	
	}

}
