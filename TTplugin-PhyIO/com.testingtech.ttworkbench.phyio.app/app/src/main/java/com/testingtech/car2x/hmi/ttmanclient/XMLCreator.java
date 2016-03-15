package com.testingtech.car2x.hmi.ttmanclient;
import com.testingtech.car2x.hmi.Globals;
import com.testingtech.car2x.hmi.PropertyReader;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
/**
 * Created by dammd on 12.03.2016.
 */
public class XMLCreator {

    private static String SEPERATOR="<SEP>";


    public static void createXML() {
        OutputStream os = null;
        Socket socket = null;

        String projectName = PropertyReader.readProperty("ttw.testcase.project");
        String folderName = PropertyReader.readProperty("ttw.testcase.folder");

        String moduleName = PropertyReader.readProperty("ttw.testcase.module");
        int port = Integer.valueOf(PropertyReader.readProperty("ttman.server.Information.port"));

        try {
            socket = new Socket(Globals.serverIp,port);
            os = socket.getOutputStream();
            BufferedWriter br = new BufferedWriter(new OutputStreamWriter(os));


            BufferedReader in= new BufferedReader(new InputStreamReader(socket.getInputStream()));

            br.write("getModulesFromFolder\n");
            br.write(projectName+"\\"+folderName+"\n");
            br.flush();


            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

            // root elements
            Document doc = docBuilder.newDocument();
            Element rootElement = doc.createElement("testCases");
            doc.appendChild(rootElement);

            String rawModuleNames = in.readLine();
            if(rawModuleNames==null){
                return;
            }
            String[] moduleNames = rawModuleNames.split(SEPERATOR);

            for(String testModuleName : moduleNames){
                // group elements
                Element group = doc.createElement("group");
                // set attribute to group element
                Attr attr = doc.createAttribute("name");
                attr.setValue(testModuleName);
                group.setAttributeNode(attr);
                rootElement.appendChild(group);

                // get testcases

                br.write("getTestcasesFromModule\n");
                br.write(testModuleName + "\n");
                br.flush();
                String rawTestNames = in.readLine();
                if(rawTestNames==null||rawTestNames.isEmpty()){
                    continue;
                }
                String[] testNames = rawTestNames.split(SEPERATOR);
                for(String testName : testNames){
                    // testcase elements
                    Element testcase = doc.createElement("testCase");
                    // set attribute to group element
                    Attr tcAttr = doc.createAttribute("id");
                    tcAttr.setValue(testName);
                    testcase.setAttributeNode(tcAttr);
                    group.appendChild(testcase);

                    // get testcase annotations
                    br.write("getAnnotationValuesForTestcase\n");
                    br.write(testModuleName + SEPERATOR + testName+ SEPERATOR+"shortDesc\n");
                    br.flush();

                    String rawTitleValues = in.readLine();
                    String titleValue = null;
                    if(rawTitleValues==null|| rawTitleValues.isEmpty()){
                        titleValue=testName;

                    }else{
                        // in case there are multiple annotations with @shortDesc
                        String[] titleValues = rawTestNames.split(SEPERATOR);
                        titleValue = titleValues[0];
                    }

                    // titel element
                    Element titel = doc.createElement("title");

                    //add textfield
                    titel.appendChild(doc.createTextNode(titleValue));
                    testcase.appendChild(titel);


                    br.write("getAnnotationValuesForTestcase\n");
                    br.write(testModuleName + SEPERATOR + testName+ SEPERATOR+"state\n");
                    br.flush();

                    String rawStates = in.readLine();
                    if(rawStates==null|| rawStates.isEmpty()) {
                        continue;
                    }

                    String[]states = rawStates.split(SEPERATOR );

                    for(String state:states){
                        //split value in state number and text
                        String[] stageValues = state.split(":");
                        if(stageValues.length<2)continue;
                        // create state element
                        Element stageElem = doc.createElement("stageLabel");

                        //create attributes
                        Attr stageID = doc.createAttribute("stageId");
                        stageID.setValue(stageValues[0]);
                        stageElem.setAttributeNode(stageID);

                        stageElem.appendChild(doc.createTextNode(stageValues[1]));
                        testcase.appendChild(stageElem);
                    }

                }


            }
            File path = Globals.mainActivity.getExternalFilesDir(null);



            File sourceFile = new File(path, "source.xml");
            if(!sourceFile.exists()) {
                sourceFile.createNewFile();
            }
            TransformerFactory transformerFactory = TransformerFactory.newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            StreamResult result = new StreamResult(sourceFile);
            transformer.transform(source, result);

            socket.close();




        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (ParserConfigurationException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (TransformerException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }



    }
}
