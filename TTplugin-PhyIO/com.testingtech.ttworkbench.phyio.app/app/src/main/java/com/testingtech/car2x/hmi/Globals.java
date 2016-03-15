package com.testingtech.car2x.hmi;

import android.app.Activity;

import com.testingtech.car2x.hmi.ttmanclient.Driver;
import com.testingtech.car2x.hmi.ttmanclient.XMLCreator;

public class Globals {

    public static String serverIp = "192.168.43.192"; // is overwritten in MainActivity
    public static int serverPort = 10279;
    public static TestRunnerActivity runnerActivity;
    public static Activity mainActivity;
    public static String currentTestCase;
    public static XMLCreator xmlCreator;

}
