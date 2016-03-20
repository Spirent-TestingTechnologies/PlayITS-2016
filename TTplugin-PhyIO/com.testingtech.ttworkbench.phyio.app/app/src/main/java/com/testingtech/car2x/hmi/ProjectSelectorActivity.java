package com.testingtech.car2x.hmi;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.testingtech.car2x.hmi.ttmanclient.Driver;
import com.testingtech.car2x.hmi.ttmanclient.XMLCreator;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ProjectSelectorActivity extends AppCompatActivity {

    private static List<String> projectNames ;
    public static final String PROJECT_NAME= "project_Name";
    private static final String PROJECT_REQ ="getProjectsFromWorkspace";
    private static final String SEPERATOR = "<SEP>";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_project_selector);

        final ListView listView = (ListView) findViewById(R.id.projListView);

        ArrayAdapter<String> projListAdapter = new ArrayAdapter<>(
               this,R.layout.child_item,R.id.laptop,projectNames);

        listView.setAdapter(projListAdapter);
        listView.setOnItemClickListener((new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, final View view,
                                    int position, long id) {
                final String item = (String) listView.getItemAtPosition(position);
                Globals.currentTestProject= item;
                loadProject(item);
            }
        }));

    }


    private void loadProject(String projectName) {

        new XMLCreator(projectName,this).execute();
    }

    public static boolean loadProjects(){
        try {
            Globals.informationWriter.write(PROJECT_REQ+"\n");
            Globals.informationWriter.flush();

            String rawModuleNames = Globals.informationReader.readLine();
            if(rawModuleNames==null||rawModuleNames.isEmpty()){
                return false;
            }
            projectNames = new ArrayList<String>(Arrays.asList(rawModuleNames.split(SEPERATOR)));
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;

    }
}
