package StreamingGUI;

import XMLTree.*;
import StreamingIDL.*;

import com.ibm.xml.generator.DOMGenerator;
import com.ibm.xml.base.treesupport.XMLTreeCellRenderer;
import com.ibm.xml.base.treesupport.event.NodeSelectionEvent;
import com.ibm.xml.tokenizer.XMLTokenizer;
import org.w3c.dom.Element;
import com.ibm.xml.tokenizer.event.*;

import javax.swing.*;
import java.util.*;
import java.net.URL;
import org.omg.CORBA.ORB;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;


public class XMLTreeForMonitor extends XMLTree
{

  JLabel                        _jlMonitorTableTitle;
  JTable                        _jtMonitor;
  JLabel                        _jlConnectedUsersValue;
  ORB                           _orb;
  MonitoringRefreshThread       _mrtMonitoringRefreshThread    = null;

  public XMLTreeForMonitor (JLabel jlMonitorTableTitle, JTable jtMonitor,
    JLabel jlConnectedUsersValue)
  {
    super ();
	_jlMonitorTableTitle        = jlMonitorTableTitle;
    _jtMonitor                  = jtMonitor;
	_jlConnectedUsersValue      = jlConnectedUsersValue;
    System.out.println("XMLTreeForMonitor");
  }

  public int init (DBInterface dbiDBInterface, URL uXML, Properties pProperties,
    ORB orb)
  {
    _orb                          = orb;
	_mrtMonitoringRefreshThread   = new MonitoringRefreshThread ();

	if (_mrtMonitoringRefreshThread.init (this) != 0)
	{
		  JOptionPane.showMessageDialog(null,
			"_mrtMonitoringRefreshThread.init failed",
			"XMLTreeForMonitor", JOptionPane.ERROR_MESSAGE);

		return 1;
	}

	_mrtMonitoringRefreshThread.start ();

    return super.init (dbiDBInterface, uXML, pProperties);
  }

  protected void handleEnabledFunctionalities ()
  {
	super.handleEnabledFunctionalities();

		if (tableRefresh () != 0)
		{
		  JOptionPane.showMessageDialog(null,
			"scStreamingServersManager.tableRefresh failed",
			"XMLTreeForMonitor", JOptionPane.ERROR_MESSAGE);

		  return;
		}
/*
	try
    {
	TreeComponent tcSelectedTreeComponent =
	  getSelectedTreeComponent();

	if (tcSelectedTreeComponent != null) {
	  if (tcSelectedTreeComponent.getAttributeValue("Identifier").equals(
		"GlobalConnectedUsers")) {
		GlobalConnectedUsersMonitoringTableModel
		  gmtmGlobalConnectedUsersMonitoringTableModel;

		  for each element of _htStreamingServers
			String                              sServerIOR;
			StreamingGUIServer                  scStreamingServersManager;
			if ((sServerIOR = (String) _htStreamingServers.get(
		  tcSelectedTreeComponent.getAttributeValue("Identifier"))) == null)
			{
		  JOptionPane.showOptionDialog (null,
		 "_htStreamingServers.get failed",
		 "XMLTreeForMonitor", JOptionPane.OK_OPTION,
		 JOptionPane.ERROR_MESSAGE, null, null, null);
		  return;
			}
		   scStreamingServersManager             = StreamingServersManagerHelper.narrow (
		  _orb. string_to_object (sServerIOR));
			// scStreamingServersManager.     take info
			// _smtmServerMonitoringTableModel.setValue (...)

		gmtmGlobalConnectedUsersMonitoringTableModel =
		  new GlobalConnectedUsersMonitoringTableModel(1);

		_jlMonitorTableTitle.setText("Global Connected Users Monitoring");
		_jtMonitor.setModel(gmtmGlobalConnectedUsersMonitoringTableModel);
	  }
	  else if (tcSelectedTreeComponent.getAttributeValue("Identifier").
			   equals("SiteConnectedUsers")) {
		SiteConnectedUsersMonitoringTableModel
		  smtmSiteConnectedUsersMonitoringTableModel;

		  for each element of _htStreamingServers
			String                              sServerIOR;
			StreamingServersManager                 scStreamingServersManager;
			if ((sServerIOR = (String) _htStreamingServers.get(
		  tcSelectedTreeComponent.getAttributeValue("Identifier"))) == null)
			{
		  JOptionPane.showOptionDialog (null,
		 "_htStreamingServers.get failed",
		 "XMLTreeForMonitor", JOptionPane.OK_OPTION,
		 JOptionPane.ERROR_MESSAGE, null, null, null);
		  return;
			}
		   scStreamingServersManager             = StreamingServersManagerHelper.narrow (
		  _orb. string_to_object (sServerIOR));
			// scStreamingServersManager.     take info
			// _smtmServerMonitoringTableModel.setValue (...)

		smtmSiteConnectedUsersMonitoringTableModel =
		  new SiteConnectedUsersMonitoringTableModel(1);

		_jlMonitorTableTitle.setText("Site Connected Users Monitoring");
		_jtMonitor.setModel(smtmSiteConnectedUsersMonitoringTableModel);
	  }
	  else if (tcSelectedTreeComponent.getAttributeValue("TagName").equals(
		"Server"))
      {
		ServerInfoHolder             siServerInfo =
		  new ServerInfoHolder();

		StreamingServersManager scStreamingServersManager;

		// name service
		NamingContextExt nc =
		  NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		  "NameService"));
		scStreamingServersManager = StreamingServersManagerHelper.narrow(
		  nc.resolve(nc.to_name(tcSelectedTreeComponent.getAttributeValue(
		  "Identifier"))));

		System.out.println(
		  "called CORBA: scStreamingServersManager.getServerInfo");

		if ((lErrorNumber = scStreamingServersManager.getServerInfo (
		  siServerInfo)) != 0) {
		  JOptionPane.showOptionDialog(null,
			"scStreamingServersManager.getServerInfo failed (Error: " +
			lErrorNumber + ")",
			"XMLTreeForMonitor", JOptionPane.OK_OPTION,
			JOptionPane.ERROR_MESSAGE, null, null, null);

		  return;
		}

		smtmServerMonitoringTableModel = new ServerMonitoringTableModel(1);

		  smtmServerMonitoringTableModel.setValue(
			0, 0,
			siServerInfo.value.ssStatus.toString());
		  smtmServerMonitoringTableModel.setValue(
			0, 1,
			Integer.toString(siServerInfo.value.lUpTimeInMinutes));
		  smtmServerMonitoringTableModel.setValue(
			0, 2,
			siServerInfo.value.pServerVersion);
		  smtmServerMonitoringTableModel.setValue(
			0, 3,
			Integer.toString(siServerInfo.value.lCPUUsage));
		  smtmServerMonitoringTableModel.setValue(
			0, 4,
			Integer.toString(siServerInfo.value.lMemoryUsage));
		  smtmServerMonitoringTableModel.setValue(
			0, 5,
			Integer.toString(siServerInfo.value.lPlayersNumberConnected));
		  smtmServerMonitoringTableModel.setValue(
			0, 6,
			Integer.toString(siServerInfo.value.lBandWidthUsageInbps));
		  smtmServerMonitoringTableModel.setValue(
			0, 7,
			Integer.toString(siServerInfo.value.lTotalBytesServed));
		  smtmServerMonitoringTableModel.setValue(
			0, 8,
			Integer.toString(siServerInfo.value.lTotalConnectionsServed));

		ServerMonitoringTableModel
		  smtmServerMonitoringTableModel;

		smtmServerMonitoringTableModel = new ServerMonitoringTableModel(1);

		_jlMonitorTableTitle.setText("Server Monitoring");
		_jtMonitor.setModel(smtmServerMonitoringTableModel);
	  }
	  else if (tcSelectedTreeComponent.getAttributeValue("Identifier").
			   equals("ServerConnectedUsers"))
	  {
		System.out.println("ServerConnectedUsers selected");
		if (tableRefresh () != 0)
		{
		  JOptionPane.showOptionDialog(null,
			"scStreamingServersManager.tableRefresh failed",
			"XMLTreeForMonitor", JOptionPane.OK_OPTION,
			JOptionPane.ERROR_MESSAGE, null, null, null);

		  return;
		}
		}
		else
		  ;
	  }
    }
    catch (Exception e)
    {
      JOptionPane.showOptionDialog (null,
        "ERROR: " + e,
        "XMLTreeForMonitor", JOptionPane.OK_OPTION,
        JOptionPane.ERROR_MESSAGE, null, null, null);

      return;
    }
*/
  }

  protected int tableRefresh ()
  {
	long                      lErrorNumber;
	TreeComponent             tcSelectedTreeComponent =
	  getSelectedTreeComponent();

	try
	{
	if (tcSelectedTreeComponent != null) {

	  System.out.println("tree component selected");

	  if (tcSelectedTreeComponent.getAttributeValue("Identifier").equals(
		"GlobalConnectedUsers")) {

		System.out.println("GlobalConnectedUsers selected");
	  }
	  else if (tcSelectedTreeComponent.getAttributeValue("Identifier").
			   equals("SiteConnectedUsers")) {

		System.out.println("SiteConnectedUsers selected");
	  }
	  else if (tcSelectedTreeComponent.getAttributeValue("TagName").equals(
		"Server")) {
		System.out.println("Server selected");

		ServerInfoHolder siServerInfo = new ServerInfoHolder();

		ServerMonitoringTableModel   smServerMonitoringTableModel;
		StreamingServer ssStreamingServer;

		// name service
		NamingContextExt nc =
		  NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		  "NameService"));
		ssStreamingServer = StreamingServerHelper.narrow(
		  nc.resolve(nc.to_name(tcSelectedTreeComponent.getAttributeValue(
		  "Identifier") + "_Server")));

		System.out.println(
		  "called CORBA: ssStreamingServer.getServerInfo");

		if ((lErrorNumber = ssStreamingServer.getServerInfo(
		  siServerInfo)) != 0) {
		  JOptionPane.showMessageDialog(null,
			"ssStreamingServer.getServerInfo failed (Error: " +
			lErrorNumber + ")",
			"XMLTreeForMonitor", JOptionPane.ERROR_MESSAGE);

            return 1;
		}

		smServerMonitoringTableModel =
		  new ServerMonitoringTableModel (1);

		if (siServerInfo.value.ssStatus == StreamingIDL.ServerStatus.ACTIVE)
		  smServerMonitoringTableModel.setValue(0, 0, "Active");
		else if (siServerInfo.value.ssStatus == StreamingIDL.ServerStatus.INACTIVE)
		  smServerMonitoringTableModel.setValue(0, 0, "Inactive");
		else
		  smServerMonitoringTableModel.setValue(0, 0, "Unknown");

		smServerMonitoringTableModel.setValue(0, 1,
		  siServerInfo.value.ulUpTimeInMinutes >= 60 ?
		  Integer.toString(siServerInfo.value.ulUpTimeInMinutes / 60) + "h. " +
		  Integer.toString(siServerInfo.value.ulUpTimeInMinutes % 60) + "min." :
		  Integer.toString(siServerInfo.value.ulUpTimeInMinutes) + "min.");

		smServerMonitoringTableModel.setValue(0, 2,
		  siServerInfo.value.pServerVersion);

		smServerMonitoringTableModel.setValue(0, 3,
		  Integer.toString(siServerInfo.value.ulCPUUsage));

		smServerMonitoringTableModel.setValue(0, 4,
		  Integer.toString(siServerInfo.value.ulMemoryUsage));

		smServerMonitoringTableModel.setValue(0, 5,
		  Integer.toString(siServerInfo.value.ulPlayersNumberConnected));

		smServerMonitoringTableModel.setValue(0, 6,
		  Integer.toString(siServerInfo.value.ulBandWidthUsageInbps));

		smServerMonitoringTableModel.setValue(0, 7,
		  Long.toString(siServerInfo.value.ullTotalBytesServed));

		smServerMonitoringTableModel.setValue(0, 8,
		  Long.toString(siServerInfo.value.ullTotalLostPacketsNumber));

		smServerMonitoringTableModel.setValue(0, 9,
		  Long.toString(siServerInfo.value.ullTotalSentPacketsNumber));

		_jlMonitorTableTitle.setText("Server Info Monitoring");
		_jlConnectedUsersValue.setText(
		  Integer.toString(siServerInfo.value.ulPlayersNumberConnected));
		_jtMonitor.setModel(smServerMonitoringTableModel);
	  }
	  else if (tcSelectedTreeComponent.getAttributeValue("Identifier").
			   equals("ServerConnectedUsers")) {
		System.out.println("ServerConnectedUsers selected");

		ServerConnectedUsersListHolder scuServerConnectedUsersList =
		  new ServerConnectedUsersListHolder();
		int iServerConnectedUserIndex;

		ServerConnectedUsersMonitoringTableModel
		  smtmServerConnectedUsersMonitoringTableModel;
		StreamingServer ssStreamingServer;

		// name service
		NamingContextExt nc =
		  NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		  "NameService"));
		ssStreamingServer = StreamingServerHelper.narrow(
		  nc.resolve(nc.to_name(((TreeComponent) tcSelectedTreeComponent.getParent()).getAttributeValue(
		  "Identifier") + "_Server")));

		System.out.println(
		  "called CORBA: ssStreamingServer.getServerConnectedUsers");

		if ((lErrorNumber = ssStreamingServer.getServerConnectedUsers(
		  scuServerConnectedUsersList)) != 0) {
		  JOptionPane.showMessageDialog(null,
			"ssStreamingServer.getServerConnectedUsers failed (Error: " +
			lErrorNumber + ")",
			"XMLTreeForMonitor", JOptionPane.ERROR_MESSAGE);

            return 1;
		}

		smtmServerConnectedUsersMonitoringTableModel =
		  new ServerConnectedUsersMonitoringTableModel(
		  scuServerConnectedUsersList.value.length == 0 ? 1 :
		  scuServerConnectedUsersList.value.length);

		for (iServerConnectedUserIndex = 0; iServerConnectedUserIndex <
			 scuServerConnectedUsersList.value.length;
			 iServerConnectedUserIndex++) {
		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 0,
			scuServerConnectedUsersList.value[iServerConnectedUserIndex].
			pClientIPAddress);

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 1,
			scuServerConnectedUsersList.value[iServerConnectedUserIndex].
			pURLWithoutParameters + "?" +
			scuServerConnectedUsersList.value[iServerConnectedUserIndex].
			pURLParameters);

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 2,
			Double.toString(scuServerConnectedUsersList.value[iServerConnectedUserIndex].
			dMovieDuration));

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 3,
			scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].pVideoCodec + " - " +
			scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].pAudioCodec);

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 4,
			Integer.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ulVideoAverageBitRate) + " - " +
			Integer.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ulAudioAverageBitRate));

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 5,
			Long.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ullVideoBytesSent) + " - " +
			Long.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ullAudioBytesSent));

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 6,
			Integer.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ulVideoPacketsNumberLost) + " - " +
			Integer.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ulVideoPacketsNumberLost));

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 7,
			Long.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ullVideoRTCPBytesReceived) + " - " +
			Long.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].
			ullAudioRTCPBytesReceived));

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 8,
			Integer.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].ulVideoJitter) + " - " +
			Integer.toString(scuServerConnectedUsersList.
			value[iServerConnectedUserIndex].ulAudioJitter));

		  smtmServerConnectedUsersMonitoringTableModel.setValue(
			iServerConnectedUserIndex, 9,
			scuServerConnectedUsersList. value[iServerConnectedUserIndex].
		    ullConnectedTimeInSeconds >= 60
			?
			Long.toString(scuServerConnectedUsersList. value[
			iServerConnectedUserIndex].ullConnectedTimeInSeconds / 60) + " min. " +
			Long.toString(scuServerConnectedUsersList. value[
			iServerConnectedUserIndex].ullConnectedTimeInSeconds % 60) + " sec."
			:
			Long.toString(scuServerConnectedUsersList. value[
			iServerConnectedUserIndex].ullConnectedTimeInSeconds) + " sec.");
		}
		_jlMonitorTableTitle.setText("Server Connected Users Monitoring");
		_jlConnectedUsersValue.setText(
		  Integer.toString(scuServerConnectedUsersList.value.length));
		_jtMonitor.setModel(smtmServerConnectedUsersMonitoringTableModel);
	  }
	  else
		;
    }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ERROR: " + e,
        "XMLTreeForMonitor", JOptionPane.ERROR_MESSAGE);

      return 2;
    }

	return 0;
  }

  protected int changeRefreshAutomatic (Integer iRefreshAutomatic)
  {
    // jcbRefreshAutomatic viene istanziato ed
    // inizializzato prima di _mrtMonitoringRefreshThread
    if (_mrtMonitoringRefreshThread == null)
      ;
	else
    {
	  _mrtMonitoringRefreshThread.setRefreshAutomatic (iRefreshAutomatic);
    }

	return 0;
  }

  protected int activeMonitoringThread (boolean bActiveMonitoringThread)
  {
    // jcbRefreshAutomatic viene istanziato ed
    // inizializzato prima di _mrtMonitoringRefreshThread
    if (_mrtMonitoringRefreshThread == null)
      ;
	else
    {
      _mrtMonitoringRefreshThread.activeMonitoringThread (
	    bActiveMonitoringThread);
    }

	return 0;
  }

  protected void xtXMLTokenizer_elementStartTagFound(ElementTokenEvent e)
  {
    String                sToken		= e.getToken();

    System.out.println("elementStartTagFound: "+sToken);

    if (sToken.compareTo("RefreshFunctionality") == 0)
    {
      if (addFunctionality (new Refresh (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for Refresh",
          "XMLTreeForMonitor", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else
      super.xtXMLTokenizer_elementStartTagFound(e);
  }
}
