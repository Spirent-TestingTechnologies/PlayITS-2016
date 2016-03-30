package com.testingtech.car2x.hmi.AsyncTasks;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.widget.Toast;

import com.testingtech.car2x.hmi.Utils.Globals;
import com.testingtech.car2x.hmi.Utils.PropertyReader;
import com.testingtech.car2x.hmi.UserInterface.TestSelectorActivity;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
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
 * Loads project inforamtions of a ttcn3 project and creates XML-File in regards to this informations.
 *
 *
 */
public class ProjectLoader extends AsyncTask<Void, Void, Boolean> {

    private final String MODULE_REQ="getModulesFromFolder";
    private final String TESTCASE_REQ="getTestcasesFromModule";
    private final String TESTCASE_ANNOT_REQ="getAnnotationValuesForTestcase";


    private final String SHORTDESC_ANNOT="shortdesc";
    private final String STAGE_ANNOT="stage";


    private final String TESTCASE_ELEM = "testcase";

    private final String TESTCASE_ROOT_ELEM = "testcases";
    private final String GROUP_ELEM = "group";

    private final String TITLE_ELEM = "title";
    private final String STAGE_LABEL_ELEM = "stageLabel";

    private final String NAME_ATTR = "name";
    private final String ID_ATTR = "id";
    private final String STAGE_ID_ATTR = "stageId";



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
    protected void onPostExecute(Boolean isLoaded) {
        if (progress != null) {
            progress.dismiss();
        }
        if (!isLoaded) {
            showConnectionError();
        } else {
            Toast.makeText(loadProjActivity, "Project successfully loaded", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(loadProjActivity, TestSelectorActivity.class);
            loadProjActivity.startActivity(intent);
        }
    }

    public ProjectLoader(String projectName, Activity loadProjActivity){
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
            // create XML-File
            DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
            DocumentBuilder docBuilder = docFactory.newDocumentBuilder();

            // root elements
            Document doc = docBuilder.newDocument();
            Element rootElement = doc.createElement(TESTCASE_ROOT_ELEM);
            doc.appendChild(rootElement);

            // send a request for all module inside the chosen project
            requestWriter.write(MODULE_REQ);
            requestWriter.newLine();
            requestWriter.write(projectName+File.separator+folderName);
            requestWriter.newLine();
            requestWriter.flush();

            //parse response
            String rawModuleNames = responseReader.readLine();
            if(rawModuleNames==null){
                return false;
            }
            String[] moduleNames = rawModuleNames.split(Globals.SEPERATOR);

            for(String testModuleName : moduleNames){

                // create for all modules a new group
                Element group = doc.createElement(GROUP_ELEM);

                // set name attribute of this group element
                Attr attr = doc.createAttribute(NAME_ATTR);
                attr.setValue(testModuleName.replace(".ttcn3",""));
                group.setAttributeNode(attr);

                // send request for all testcases inside the module
                requestWriter.write(TESTCASE_REQ);
                requestWriter.newLine();
                requestWriter.write(testModuleName);
                requestWriter.newLine();
                requestWriter.flush();

                //parse response
                String rawTestNames = responseReader.readLine();
                if(rawTestNames==null||rawTestNames.isEmpty()){
                    continue;
                }
                rootElement.appendChild(group);
                String[] testNames = rawTestNames.split(Globals.SEPERATOR);

                //iterate over all testcases and request specific annotations
                for(String testName : testNames){

                    // testcase elements
                    Element testcase = doc.createElement(TESTCASE_ELEM);

                    // create attribute for testCaseId
                    Attr tcAttr = doc.createAttribute(ID_ATTR);
                    tcAttr.setValue(testName);
                    testcase.setAttributeNode(tcAttr);
                    group.appendChild(testcase);

                    // get testcase annotations
                    requestWriter.write(TESTCASE_ANNOT_REQ);
                    requestWriter.newLine();
                    requestWriter.write(testModuleName + Globals.SEPERATOR + testName+ Globals.SEPERATOR+SHORTDESC_ANNOT);
                    requestWriter.newLine();
                    requestWriter.flush();

                    // parse annotations
                    String rawTitleValues = responseReader.readLine();
                    String titleValue;

                    // if no title is defined then use testCaseId
                    if(rawTitleValues==null|| rawTitleValues.isEmpty()){
                        titleValue=testName;

                    }else{
                        // in case there are multiple annotations with @shortDesc -> take first
                        String[] titleValues = rawTitleValues.split(Globals.SEPERATOR);
                        titleValue = titleValues[0];
                    }

                    // titel element
                    Element titel = doc.createElement(TITLE_ELEM);

                    //add textfield
                    titel.appendChild(doc.createTextNode(titleValue));
                    testcase.appendChild(titel);

                    // send request for stages
                    requestWriter.write(TESTCASE_ANNOT_REQ);
                    requestWriter.newLine();
                    requestWriter.write(testModuleName + Globals.SEPERATOR + testName+ Globals.SEPERATOR+STAGE_ANNOT);
                    requestWriter.newLine();
                    requestWriter.flush();

                    // parse response
                    String rawStates = responseReader.readLine();
                    if(rawStates==null|| rawStates.isEmpty()) {
                        continue;
                    }
                    String[]states = rawStates.split(Globals.SEPERATOR );

                    for(String state:states){
                        //split value in state number and text
                        String stageNr = state.substring(0,state.indexOf(":"));
                        String stageValue= state.substring(state.indexOf(":")+1).trim();
                        // create state element
                        Element stageElem = doc.createElement(STAGE_LABEL_ELEM);

                        //create attributes
                        Attr stageID = doc.createAttribute(STAGE_ID_ATTR);
                        stageID.setValue(stageNr);
                        stageElem.setAttributeNode(stageID);

                        stageElem.appendChild(doc.createTextNode(stageValue));
                        testcase.appendChild(stageElem);
                    }

                }


            }
            // store XML-file
            File path = Globals.mainActivity.getExternalFilesDir(null);

            File sourceFile = new File(path, Globals.SOURCE_XML_ID);

            if(!sourceFile.exists()) {
                if(!sourceFile.createNewFile()){
                    return false;
                };
            }
            TransformerFactory transformerFactory = TransformerFactory.newInstance();
            Transformer transformer = transformerFactory.newTransformer();
            DOMSource source = new DOMSource(doc);
            StreamResult result = new StreamResult(sourceFile);
            transformer.transform(source, result);

        } catch (UnknownHostException e) {
            e.printStackTrace();
            return false;
        } catch (IOException e) {
            e.printStackTrace();
            return false;

        } catch (ParserConfigurationException e) {
            e.printStackTrace();
            return false;

        } catch (TransformerException e) {
            e.printStackTrace();
            return false;


        }
        return true;

    }

}
