package com.testingtech.car2x.hmi;

import android.app.Activity;

import com.testingtech.car2x.hmi.ttmanclient.Driver;
import com.testingtech.car2x.hmi.ttmanclient.XMLCreator;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class Globals {

    public static String serverIp = "192.168.43.192"; // is overwritten in MainActivity
    public static int serverPort = 10279;
    public static TestRunnerActivity runnerActivity;
    public static Activity mainActivity;
    public static String currentTestCase;
    public static XMLCreator xmlCreator;
    public static String currTestModule;
    public static String currentTestProject;

    public static Socket informationSocket;
    public static BufferedWriter informationWriter;
    public static BufferedReader informationReader;


}
