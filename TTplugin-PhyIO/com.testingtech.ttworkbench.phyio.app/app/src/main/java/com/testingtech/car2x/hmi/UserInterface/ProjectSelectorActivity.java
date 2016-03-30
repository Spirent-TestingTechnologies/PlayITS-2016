package com.testingtech.car2x.hmi.UserInterface;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import com.testingtech.car2x.hmi.Utils.Globals;
import com.testingtech.car2x.hmi.R;
import com.testingtech.car2x.hmi.AsyncTasks.ProjectLoader;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class ProjectSelectorActivity extends AppCompatActivity {

    private static List<String> projectNames ;
    public static final String PROJECT_NAME= "project_Name";
    private static final String PROJECT_REQ ="getProjectsFromWorkspace";

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
        new ProjectLoader(projectName,this).execute();
    }

    /**
     * Sends a request for all available projects in the server workspace.
     * @return true if list of projects is received
     */
    public static boolean loadProjects(){
        try {
            if(Globals.informationWriter==null || Globals.informationReader==null){
                return false;
            }
            // send request
            Globals.informationWriter.write(PROJECT_REQ+"\n");
            Globals.informationWriter.flush();

            // receive response and parse it
            String rawModuleNames = Globals.informationReader.readLine();
            if(rawModuleNames==null||rawModuleNames.isEmpty()){
                return false;
            }
            projectNames = new ArrayList<String>(Arrays.asList(rawModuleNames.split(Globals.SEPERATOR)));
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;

    }
}
