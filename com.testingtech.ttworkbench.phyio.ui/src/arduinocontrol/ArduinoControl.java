package arduinocontrol;

import java.io.IOException;
import java.util.List;

import com.testingtech.tworkbench.ttman.server.api.ExecuteCampaignJob;
import com.testingtech.tworkbench.ttman.server.api.ICampaignExecutionHandler;
import com.testingtech.tworkbench.ttman.server.api.ITERequest;
import com.testingtech.tworkbench.ttman.server.api.ITEResponse;
import com.testingtech.tworkbench.ttman.server.api.Job;
import com.testingtech.tworkbench.ttman.server.api.Parameter;
import com.testingtech.tworkbench.ttman.server.api.TestCampaign;
import com.testingtech.tworkbench.ttman.server.api.TestCase;
import com.testingtech.tworkbench.ttman.server.api.TestCaseStatus;

public class ArduinoControl implements ICampaignExecutionHandler {

	@Override
	public ITEResponse teRequest(ITERequest arg0) throws IOException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void controlFinished(Job arg0, String arg1, List<TestCaseStatus> arg2)
			throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void controlStarted(Job arg0, String arg1) throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void serverShutdown() throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void testCaseFinished(Job arg0, TestCase arg1, TestCaseStatus arg2,
			List<Parameter> arg3) throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void testCaseStarted(Job arg0, TestCase arg1, List<Parameter> arg2)
			throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void campaignFinished(ExecuteCampaignJob arg0, TestCampaign arg1)
			throws IOException {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void campaignStarted(ExecuteCampaignJob arg0, TestCampaign arg1)
			throws IOException {
		// TODO Auto-generated method stub
		
	}

}
