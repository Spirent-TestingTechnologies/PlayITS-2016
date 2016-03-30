package com.testingtech.car2x.hmi.Utils;

import android.app.Activity;

import com.testingtech.car2x.hmi.UserInterface.TestRunnerActivity;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.net.Socket;

public class Globals {

    public static String serverIp = "192.168.43.192"; // is overwritten in MainActivity
    public static int serverPort = 10279;
    public final static String SEPERATOR = "<SEP>";
    public final static String SOURCE_XML_ID = "source.xml";

    public static TestRunnerActivity runnerActivity;
    public static Activity mainActivity;
    public static String currentTestCase;
    public static String currTestModule;
    public static String currentTestProject;

    public static Socket informationSocket;
    public static BufferedWriter informationWriter;
    public static BufferedReader informationReader;


}
