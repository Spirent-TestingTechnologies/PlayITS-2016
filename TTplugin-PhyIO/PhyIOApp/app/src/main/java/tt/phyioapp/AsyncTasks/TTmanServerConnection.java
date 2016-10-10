package com.testingtech.car2x.hmi.AsyncTasks;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.widget.Toast;

import com.testingtech.car2x.hmi.Utils.Globals;
import com.testingtech.car2x.hmi.Utils.PropertyReader;
import com.testingtech.car2x.hmi.UserInterface.MainActivity;
import com.testingtech.car2x.hmi.UserInterface.ProjectSelectorActivity;
import com.testingtech.car2x.hmi.ttmanclient.Driver;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class TTmanServerConnection extends AsyncTask<Void, Void, Boolean> {

    private ProgressDialog progress;
    private MainActivity mainActivity;
    private Driver driver = Driver.getInstance();

    public TTmanServerConnection(MainActivity mainActivity) {
        this.mainActivity = mainActivity;
    }

    @Override
    protected void onPreExecute() {
        progress = createProgressDialog();
        progress.show();
        super.onPreExecute();
    }

    private ProgressDialog createProgressDialog() {
        ProgressDialog progress = new ProgressDialog(mainActivity);
        progress.setMessage("Connecting to " + Globals.serverIp);
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
    protected Boolean doInBackground(Void... params) {
        boolean connected = driver.connect();
        if(connected){
            try {
                // initialize two directional, message based connection to information server

                int port = Integer.valueOf(PropertyReader.readProperty("ttman.server.Information.port"));

                Globals.informationSocket = new Socket(Globals.serverIp,port);
                Globals.informationWriter = new BufferedWriter(new OutputStreamWriter(Globals.informationSocket.getOutputStream()));
                Globals.informationReader =  new BufferedReader(new InputStreamReader(Globals.informationSocket.getInputStream()));

                // load all projects that exist in the workspace of the server
                boolean projectsLoaded = ProjectSelectorActivity.loadProjects();
                if(!projectsLoaded){
                    return false;
                }
            } catch (IOException e) {
                return false;
            }
            return true;
        }
        return false;
    }

    @Override
    protected void onPostExecute(Boolean isConnected) {
        if (progress != null) {
            progress.dismiss();
        }
        if (!isConnected) {
            showConnectionError();
        } else {
            Toast.makeText(mainActivity, "Successfully connected to TTman server", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(mainActivity, ProjectSelectorActivity.class);
            mainActivity.startActivity(intent);
        }
    }

    private void showConnectionError() {
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(mainActivity);
        alertDialog.setTitle("Connection Error");
        alertDialog.setMessage("Cannot connect to TTman server at " + Globals.serverIp);
        alertDialog.setNeutralButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        alertDialog.show();
    }
}
