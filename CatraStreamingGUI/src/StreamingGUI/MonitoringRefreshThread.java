package StreamingGUI;

import java.util.*;
import java.io.*;
import javax.swing.*;


public class MonitoringRefreshThread extends Thread

{

  private XMLTreeForMonitor _xtfmXMLTreeForMonitor;
  private int _iSecondsBetweenTwoRefresh;
  private boolean _bMonitoringThreadActived;

  public MonitoringRefreshThread() {
	super();
  }

  public int init(XMLTreeForMonitor xtfmXMLTreeForMonitor)
  {
	_xtfmXMLTreeForMonitor = xtfmXMLTreeForMonitor;
	_iSecondsBetweenTwoRefresh     = -1;
	_bMonitoringThreadActived      = false;

	return 0;
  }

  public void run() {
	int         iSecondsToSleep;

	while (true) {
	  try
	  {
		if (!_bMonitoringThreadActived)
	      iSecondsToSleep      = 5;
		else
   	    {
		  if (_iSecondsBetweenTwoRefresh != -1)
	        iSecondsToSleep      = _iSecondsBetweenTwoRefresh;
		  else
	        iSecondsToSleep      = 5;
        }

	    // System.out.println("Refresh from MonitoringRefreshThread. Actived: " +
	    //  _bMonitoringThreadActived + ", seconds: " + iSecondsToSleep);

		sleep (iSecondsToSleep * 1000);

		if (_bMonitoringThreadActived && _iSecondsBetweenTwoRefresh != -1)
   	    {
		  if (_xtfmXMLTreeForMonitor.tableRefresh() != 0) {
		    JOptionPane.showMessageDialog(null,
			  "_xtfmXMLTreeForMonitor.tableRefresh failed",
			  "MonitoringRefreshThread", JOptionPane.ERROR_MESSAGE);
		  }
	    }
	  }
	  catch (Exception e) {
		JOptionPane.showMessageDialog(null,
									 "ERROR: " + e,
									 "MonitoringRefreshThread",
									 JOptionPane.ERROR_MESSAGE);

		return;
	  }
	}
  }

  public int activeMonitoringThread (boolean bActiveMonitoringThread)
  {
	if (_bMonitoringThreadActived != bActiveMonitoringThread)
    {
	  _bMonitoringThreadActived    = bActiveMonitoringThread;
    }

	return 0;
  }

  public int setRefreshAutomatic (Integer iSecondsBetweenTwoRefresh)
  {
	_iSecondsBetweenTwoRefresh    = iSecondsBetweenTwoRefresh.intValue();

	return 0;
  }
}
