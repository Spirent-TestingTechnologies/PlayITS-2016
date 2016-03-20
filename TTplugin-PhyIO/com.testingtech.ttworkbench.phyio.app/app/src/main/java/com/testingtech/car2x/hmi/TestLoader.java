package com.testingtech.car2x.hmi;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.AsyncTask;
import android.widget.Toast;

import com.testingtech.car2x.hmi.testcases.TestCase;
import com.testingtech.car2x.hmi.testcases.XmlLoader;
import com.testingtech.car2x.hmi.ttmanclient.Driver;

import java.io.IOException;


public class TestLoader extends AsyncTask<Void, Void, Boolean> {

    public final static String TEST_ID = "id";
    public final static String TEST_TITLE = "title";
    public final static String TEST_STAGES = "stages";
    public final static String TEST_GROUP = "group";

    public static String testcaseId;


    private ProgressDialog progress;
    private Activity testSelectorActivity;

    public TestLoader(Activity testSelectorActivity, String testcaseId) {
        this.testSelectorActivity = testSelectorActivity;
        this.testcaseId = testcaseId;
    }
    @Override
    protected void onPreExecute() {
        progress = createProgressDialog();
        progress.show();
        super.onPreExecute();
    }

    private ProgressDialog createProgressDialog() {
        ProgressDialog progress = new ProgressDialog(testSelectorActivity);
        progress.setMessage("Load test file: "+ Globals.currTestModule+".clf");
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
            startTestRunnerActivity(testcaseId);
        }
    }

    @Override
    protected Boolean doInBackground(Void... params) {
        try {
            Driver.getInstance().initTestSuite();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    private void showConnectionError() {
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(testSelectorActivity);
        alertDialog.setTitle("Connection Error");
        alertDialog.setMessage("Cannot load Testfile " + Globals.currTestModule+".clf");
        alertDialog.setNeutralButton("OK", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
            }
        });
        alertDialog.show();
    }


    public void startTestRunnerActivity(String testCaseId ) {
        TestCase testCase = XmlLoader.getTestCaseById(testCaseId);
        Intent intent = new Intent(testSelectorActivity, TestRunnerActivity.class);
        intent.putExtra(TEST_ID, testCaseId);
        intent.putExtra(TEST_TITLE, testCase.getTitle());
        intent.putExtra(TEST_STAGES, testCase.getStagesAsArray());
        testSelectorActivity.startActivity(intent);
    }
}
