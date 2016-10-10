/**
 * HMI service application in charge of controlling test cases
 * via TTman and pushing event information (such as test case
 * progress) to the HMI client.
 * <p/>
 * Current variant does not communicate with the TTman server. Instead, it responds to the HMI
 * using pre-defined messages.
 */
package com.testingtech.car2x.hmi.ttmanclient;

import com.testingtech.car2x.hmi.Utils.Globals;
import com.testingtech.car2x.hmi.Utils.Logger;
import com.testingtech.car2x.hmi.Utils.PropertyReader;
import com.testingtech.tworkbench.ttman.server.api.Credentials;
import com.testingtech.tworkbench.ttman.server.api.ExecutionServerFactory;
import com.testingtech.tworkbench.ttman.server.api.Parameter;
import com.testingtech.tworkbench.ttman.server.impl.client.RemoteExecutionClient;

import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Collections;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

public class Driver {

    public static final String CAR2_X_MODULE_PARAMETERS_TCP_REMOTE_HOST = "Car2X_ModuleParameters.TCP_REMOTE_HOST";
    private Publisher publisher = new Publisher();
    private NotificationHandler notificationHandler = new NotificationHandler(publisher);
    private TestCaseRunner testCaseRunner;
    private RemoteExecutionClient client;
    private ExecutorService executorService = Executors.newSingleThreadExecutor();

    private static Driver DRIVER;

    public static Driver getInstance() {
        if (DRIVER == null) {
            DRIVER = new Driver();
        }
        return DRIVER;
    }

    private Driver() {
    }

    /**
     * Create and initialize  connection to TTmanServe.
     * @return true if connection is initialized otherwise false
     */
    public boolean connect() {
        boolean isConnected = false;
        try {
            connectToServer();
            isConnected = true;
        } catch (Exception e) {
            e.printStackTrace(Logger.writer);
            Logger.writer.flush();
        }
        return isConnected;
    }

    private void connectToServer() throws InterruptedException, ExecutionException, TimeoutException {
        Future<RemoteExecutionClient> serverConnectionFuture = executorService.submit(new Callable<RemoteExecutionClient>() {
            @Override
            public RemoteExecutionClient call() throws Exception {
                return initServerConnection();
            }
        });
        client = serverConnectionFuture.get(30, TimeUnit.SECONDS);
    }

    public void startTestCase() {
        Logger.writeLog("DRIVER: Starting");
        if (testCaseRunner != null) {
            testCaseRunner.setCurrentTestCase(Globals.currentTestCase);
            publisher.setCurrentTestCase(Globals.currentTestCase);
            new Thread(testCaseRunner).start();
            Logger.writeLog("DRIVER: Test case started");
        } else {
            Logger.writeLog("Cannot start test case: testCaseRunner is null");
        }
    }

    public void stopExecution() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                Logger.writeLog("DRIVER: Stopping test case execution");
                testCaseRunner.stopExecution();
            }
        }).start();
    }

    /**
     * Initialize TTman server connection using the file based configuration parameters.
     */
    private RemoteExecutionClient initServerConnection() throws IOException {
        InetAddress clientIp = getOwnIp();
        if (clientIp != null) {
            Logger.writeLog("DRIVER: Host address " + clientIp.getHostAddress());
        }
        RemoteExecutionClient client = (RemoteExecutionClient) new ExecutionServerFactory().createClient();
        String user = PropertyReader.readProperty("ttman.server.user.name");
        String password = PropertyReader.readProperty("ttman.server.user.password");
        Credentials credentials = new Credentials(user, password);
        Logger.writeLog("DRIVER: connecting to TTman server at " + Globals.serverIp + ":" + Globals.serverPort);
        client.connect(Globals.serverIp, Globals.serverPort, credentials, notificationHandler);
        return client;
    }

    /**
     * Loads ttcn3 execution file and creates @tesCaseRunner
     * @param relExecPath
     * @throws IOException
     */
    public void initTestSuite(String relExecPath) throws IOException {

        // if no rel. path of executable clf file is written in annotations,then use same name like the testmodule
        if(relExecPath.isEmpty()){
            relExecPath= Globals.currTestModule+".clf";
        }
        Logger.writeLog("DRIVER: Loading test suite " + relExecPath);
        client.loadTestSuiteFromFile(Globals.currentTestProject, relExecPath);

        byte[] encodedParam = ("<Values:charstring type=\"charstring\" xmlns:Values=\"Values.xsd\"><Values:value>" + Globals.serverIp + "</Values:value></Values:charstring>").getBytes("UTF-8");
        Set<Parameter> paramCollection = new HashSet<Parameter>();
        paramCollection.add(new Parameter(CAR2_X_MODULE_PARAMETERS_TCP_REMOTE_HOST, "", encodedParam));
        paramCollection.add(new Parameter("NOTIFY_UI","","true".getBytes()));
        client.setModuleParameters(Collections.singleton(new Parameter(CAR2_X_MODULE_PARAMETERS_TCP_REMOTE_HOST, "", encodedParam)));
        testCaseRunner = new TestCaseRunner(client);
    }

    private InetAddress getOwnIp() {
        try {
            Enumeration<NetworkInterface> enumNetwork = NetworkInterface.getNetworkInterfaces();
            while (enumNetwork.hasMoreElements()) {
                NetworkInterface netInterface = enumNetwork.nextElement();
                if (netInterface.getName().equalsIgnoreCase("eth0") ||
                        netInterface.getName().equalsIgnoreCase("wlan0")) {
                    Enumeration<InetAddress> enumIpAddr = netInterface.getInetAddresses();
                    while (enumIpAddr.hasMoreElements()) {
                        InetAddress inetAddress = enumIpAddr.nextElement();
                        if (!inetAddress.getHostAddress().contains(":")) {
                            return inetAddress;
                        }
                    }
                }
            }
        } catch (SocketException se) {
            se.printStackTrace(Logger.writer);
            Logger.writer.flush();
        }
        return null;
    }

    public boolean isConnected() {
        return client != null && client.isConnected();
    }

    public void closeServerConnection() {

        new Thread(new Runnable() {
            @Override
            public void run() {
                Logger.writeLog("DRIVER: Disconnecting from server");
                try {
                    if(Globals.informationSocket!=null){
                        Globals.informationSocket.close();
                        if (client != null) {
                            client.disconnect();
                        }
                    }
                } catch (IOException ioex) {
                    Logger.writeLog("DRIVER: An error occurred while disconnecting from server: " + ioex.getMessage());
                }
            }
        }).start();
    }
}