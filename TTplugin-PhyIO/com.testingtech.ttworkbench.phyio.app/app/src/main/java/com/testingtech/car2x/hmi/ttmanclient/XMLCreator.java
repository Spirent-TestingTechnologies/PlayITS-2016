package com.testingtech.car2x.hmi.ttmanclient;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.widget.Toast;

import com.testingtech.car2x.hmi.Globals;
import com.testingtech.car2x.hmi.PropertyReader;
import com.testingtech.car2x.hmi.TestSelectorActivity;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
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
public class XMLCreator extends AsyncTask<Void, Void, Boolean> {

    private String SEPERATOR="<SEP>";

    private BufferedWriter requestWriter=null;
    private BufferedReader responseReader=null;
    private String projectName ;
    private Activity loadProjActivity;
    private ProgressDialog progress;

    @Override
    protected void onPreExecute() {
        progress = createProgressDialog();
        progress.show();
        super.onPreExecute();
    }

    private ProgressDialog createProgressDialog() {
        ProgressDialog progress = new ProgressDialog(loadProjActivity);
        progress.setMessage("Load project: " + projectName);
        progress.setCanceledOnTouchOutside(false);
        progress.setCancelable(true);
        progress.setIndeterminate(true);
        progress.setOnCancelListener(
                new DialogInterface.OnCancelListener() {
                    @Override
                    public void onCancel(DialogInterface dialog) {
                        cancel(true);
                    }
                });
        return progress;
    }

    @Override
    protected void onPostExecute(Boolean isConnected) {
        if (progress != null) {
            progress.dismiss();
        }
        if (!isConnected) {
            showConnectionError();
        } else {
            Toast.makeText(loadProjActivity, "Project successfully loaded", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(loadProjActivity, TestSelectorActivity.class);
            loadProjActivity.startActivity(intent);
        }
    }

    public XMLCreator(String projectName, Activity loadProjActivity){
        this.loadProjActivity= loadProjActivity;
        this.projectName=projectName;
        requestWriter=Globals.informationWriter;
        responseReader=Globals.informationReader;

    }

    private void showConnectionError() {
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(loadProjActivity);
        alertDialog.setTitle("Connection Error");
        alertDialog.setMessage("Cannot load Project: " + projectName);
        alertDialog.setNeutralButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        alertDialog.show();
    }

    @Override
    protected Boolean doInBackground(Void... params) {
        String folderName = PropertyReader.readProperty("ttw.testcase.folder");

        try {

            requestWriter.write("getModulesFromFolder\n");
            requestWriter.write(projectName+"\\"+folderName+"\n");
            requestWriter.flush();


            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

            // root elements
            Document doc = docBuilder.newDocument();
            Element rootElement = doc.createElement("testCases");
            doc.appendChild(rootElement);

            String rawModuleNames = responseReader.readLine();
            if(rawModuleNames==null){
                return true;
            }
            String[] moduleNames = rawModuleNames.split(SEPERATOR);

            for(String testModuleName : moduleNames){
                // group elements
                Element group = doc.createElement("group");
                // set attribute to group element
                Attr attr = doc.createAttribute("name");
                attr.setValue(testModuleName.replace(".ttcn3",""));
                group.setAttributeNode(attr);

                // get testcases

                requestWriter.write("getTestcasesFromModule\n");
                requestWriter.write(testModuleName + "\n");
                requestWriter.flush();
                String rawTestNames = responseReader.readLine();
                if(rawTestNames==null||rawTestNames.isEmpty()){
                    continue;
                }
                rootElement.appendChild(group);

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
                    requestWriter.write("getAnnotationValuesForTestcase\n");
                    requestWriter.write(testModuleName + SEPERATOR + testName+ SEPERATOR+"shortDesc\n");
                    requestWriter.flush();

                    String rawTitleValues = responseReader.readLine();
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


                    requestWriter.write("getAnnotationValuesForTestcase\n");
                    requestWriter.write(testModuleName + SEPERATOR + testName+ SEPERATOR+"stage\n");
                    requestWriter.flush();

                    String rawStates = responseReader.readLine();
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

        } catch (UnknownHostException e) {
            return false;
        } catch (IOException e) {
            return false;

        } catch (ParserConfigurationException e) {
            return false;

        } catch (TransformerException e) {
            return false;


        }
        return true;

    }

    public String getProjectPath(){
        try {
            requestWriter.write("getWorkspacePath\n");

            requestWriter.flush();
            return responseReader.readLine();

        } catch (IOException e) {
            e.printStackTrace();
        }
        return"";
    }
}
