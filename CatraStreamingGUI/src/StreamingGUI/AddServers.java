package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;
import StreamingIDL.*;
import java.util.Hashtable;
import org.omg.CORBA.*;
import org.omg.CORBA.StringHolder;
import org.omg.CosNaming.*;



public class AddServers extends Functionality

{

  int                           _iMaxServersNumberToAdd;
  Properties                    _pProperties;
  XMLTreeForConfiguration       _xtfcXMLTreeForConfiguration;
  XMLTreeForContents            _xtfcXMLTreeForContents;
  XMLTreeForMonitor             _xtfmXMLTreeForMonitor;
  ORB                           _orb;
  Vector                        _vLastSavedDateForConfigurationItems;

  public AddServers (Frame fParentFrame,
    Properties pProperties,
	XMLTreeForConfiguration xtfcXMLTreeForConfiguration,
    XMLTreeForContents xtfcXMLTreeForContents,
    XMLTreeForMonitor xtfmXMLTreeForMonitor,
	ORB orb, Vector vLastSavedDateForConfigurationItems)
  {
    super (fParentFrame);
	_pProperties                  = pProperties;
	_iMaxServersNumberToAdd       = 5;
	_xtfcXMLTreeForConfiguration  = xtfcXMLTreeForConfiguration;
    _xtfcXMLTreeForContents       = xtfcXMLTreeForContents;
    _xtfmXMLTreeForMonitor        = xtfmXMLTreeForMonitor;
    _orb                          = orb;
    _vLastSavedDateForConfigurationItems      = vLastSavedDateForConfigurationItems;
  }

  public void realize (XMLTree xtXMLTree)
  {
    TreeComponent                   tcSelectedSiteTreeComponent;
	ModelForValuesList              lmListModel;
	ChoiceFromValuesDialog          cvdValuesDialog           = null;
    TreeComponent                   tcNewResourceType;
    Vector                          vAttributesTypesList      = new Vector ();
    Vector                          vMandatoryList            = new Vector ();
    int                             iServerIndex;
    Vector                          vServersNames            = new Vector ();
	String                          sStringName;
	int                             iMaxServersToBeRetrieved;
	int                             iSelectedValuesNumber;


    try
    {
      tcSelectedSiteTreeComponent   = xtXMLTree.getSelectedTreeComponent();

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  BindingListHolder blhBindingListHolder = new BindingListHolder ();
	  BindingIteratorHolder bihBindingIteratorHolder = new BindingIteratorHolder ();

	  iMaxServersToBeRetrieved    = 500;
	  nc.list(iMaxServersToBeRetrieved, blhBindingListHolder, bihBindingIteratorHolder);

	  lmListModel  = new ModelForValuesList ();

	  for (iServerIndex = 0; iServerIndex < blhBindingListHolder.value.length;
		   iServerIndex++)
	  {
		sStringName = new String (((blhBindingListHolder.value [iServerIndex]).binding_name[0]).id);

        lmListModel. addElement (
		  sStringName.substring(0, sStringName.length() - new String ("_Server").length()));
	  }

      cvdValuesDialog = new ChoiceFromValuesDialog (_fParentFrame,
		"Choice the Streaming Server Name to add",
        lmListModel, "MULTIPLE", null, true);
      cvdValuesDialog.setVisible(true);

        iSelectedValuesNumber			= cvdValuesDialog. getSelectedValuesNumber ();

        if (iSelectedValuesNumber != -1)
        {
		  int						iSelectedValueIndex;
		  int                       iSelectedValuePosition;

		  vServersNames.clear();
		  for (iSelectedValueIndex = 0; iSelectedValueIndex < iSelectedValuesNumber;
			   iSelectedValueIndex++)
		  {

			String aaaa = cvdValuesDialog.getSelectedValue(iSelectedValueIndex);
			vServersNames.add(
					 cvdValuesDialog.getSelectedValue(iSelectedValueIndex));
          }

		  if (addServers(xtXMLTree, tcSelectedSiteTreeComponent, vServersNames,
						 true) != 0) {
			JOptionPane.showMessageDialog(null,
										  "addServers failed", "AddServers",
										  JOptionPane.ERROR_MESSAGE);

			return;
		  }
	    }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ERROR: " + e,
        "AddServers", JOptionPane.ERROR_MESSAGE);

      return;
    }
    finally
    {
      if (cvdValuesDialog != null)
        cvdValuesDialog.dispose();
    }
  }

  public int addServers (XMLTree xtXMLTree,
    TreeComponent tcSelectedSiteTreeComponent, Vector vServersNames,
	boolean bUpdateConfigurationFile)
  {
	int                             iServerIndex;
    ConfigurationItemsWithInfoListHolder    cilConfigurationItemsWithInfoList;
	TreeComponent                   tcServerTreeComponent;
	TreeComponent                   tcConfigurationSectionTreeComponent;
    Choice                          tcConfigurationItemTreeComponent;
	StreamingServer                 ssStreamingServer;
    // String                          sIORServer;
    // StringsListHolder               lsNICs;

    try
    {
	  if (bUpdateConfigurationFile &&
	    ConfigurationTransaction.beginGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile(),
		_pProperties.getProperty("login")) != 0) {
		JOptionPane.showMessageDialog(null,
		  "beginGUIConfigurationTransaction failed",
		  "AddServers", JOptionPane.ERROR_MESSAGE);

		return 1;
	  }

	  for (iServerIndex = 0; iServerIndex < vServersNames.size();
		   iServerIndex++) {

		// Verify that the server doesn't exist
		{
		  TreeVisit tvTreeVisit;
		  TreeVisitToFindTreeComponent tvftcTreeVisitToFindTreeComponent;

		  tvftcTreeVisitToFindTreeComponent = new TreeVisitToFindTreeComponent(
			"Entity", (String) vServersNames.elementAt(iServerIndex));
		  tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

		  if (tvTreeVisit.preOrderVisit(
			_xtfcXMLTreeForConfiguration.getXMLTopTreeComponent()) != 0) {
			JOptionPane.showMessageDialog(null,
										 "preOrderVisit failed", "AddServers",
										 JOptionPane.PLAIN_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0)
		    {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 2;
		  }

		  if (tvftcTreeVisitToFindTreeComponent.getTreeComponentFound() !=
			  null) {
			JOptionPane.showMessageDialog(null,
										 "The " +
										 (String) vServersNames.
										 elementAt(iServerIndex) +
										 " server already exist", "AddServers",
										 JOptionPane.ERROR_MESSAGE);

			continue;
		  }
		}

		/*    IOR file
			{
		  BufferedReader          brIORServer;
		  // URL				uIORUrl		= new URL (
		  //  "file://" +
		  //  (String) vServersNames.elementAt(iServerIndex) +
		  //  "/home/giuliano/GUI/StreamingServer.ref");
		  URL				uIORUrl		= new URL (
		 "file:\\D:\\GUI\\StreamingServer.ref");
		  System.out.println ("IORFileName: " + uIORUrl. getFile ());
		   brIORServer     = new BufferedReader (new FileReader (uIORUrl. getFile ()));
		  sIORServer      = brIORServer. readLine ();
		  System. out. println ("Server IOR: " + sIORServer);
		  System. out. println ("string_to_object for StreamingGUIServer");
		  org.omg.CORBA.Object    obj = _orb.string_to_object (sIORServer);
		  System. out. println ("narrow for StreamingGUIServer");
		  scStreamingServersManager       = StreamingServersManagerHelper.narrow (
		 obj);
			}
		 */

		// load the configuration of the server and
		// add the new Server node to the tree
        {
		  int         iConfigurationItemIndex;
		  int         iEnumerativeIndex;
		  long        lErrorNumber;

		  // name service
		  NamingContextExt nc =
			NamingContextExtHelper.narrow(_orb.resolve_initial_references(
			"NameService"));
		  ssStreamingServer = StreamingServerHelper.narrow(
			nc.resolve(nc.to_name( ((String) vServersNames.elementAt(
			iServerIndex)) + "_Server")));

		  cilConfigurationItemsWithInfoList = new ConfigurationItemsWithInfoListHolder();

		  System.out.println(
			"called CORBA: ssStreamingServer.getConfiguration");

		  if ((lErrorNumber = ssStreamingServer.getConfiguration(
			cilConfigurationItemsWithInfoList)) != 0) {
			JOptionPane.showMessageDialog(null,
			  "ssStreamingServer.getConfiguration failed (Error: " +
			  lErrorNumber + ")",
			  "AddServers", JOptionPane.ERROR_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 3;
		  }

		  System.out.println("called CORBA: ssStreamingServer.getHostName");

		  StringHolder shHostName = new StringHolder();

		  if ((lErrorNumber = ssStreamingServer.getHostName(shHostName)) != 0) {
			JOptionPane.showMessageDialog(null,
			  "ssStreamingServer.getHostName failed (Error: " +
			  lErrorNumber + ")",
			  "AddServers", JOptionPane.ERROR_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 4;
		  }

		  // Add the new Server node to the tree
		  {
			tcServerTreeComponent =
			  xtXMLTree.buildTreeComponent();
			tcServerTreeComponent.setAttribute("TagName", "Entity");
			tcServerTreeComponent.setAttribute("Identifier",
			  (String) vServersNames.elementAt(iServerIndex));
			tcServerTreeComponent.setAttribute("Label",
			  (String) vServersNames.elementAt(iServerIndex) +
			  " (HostName: " + shHostName.value + ")");
			tcServerTreeComponent.setAttribute("HostName", shHostName.value);
			tcServerTreeComponent.setAttribute("Icon", "Server.gif");
			tcServerTreeComponent.setAttribute("ToolTip",
			  (String) vServersNames.elementAt(iServerIndex) +
			  " Streaming Server");
			tcServerTreeComponent.setAttribute("DisabledFunctionalitiesList",
			  "AddSite,DeleteSite,AddServers,ChangeInEverySiteServer,ChangeInEveryServer");
			tcServerTreeComponent.setAttribute(
			  "DisabledFunctionalitiesListSeparator", ",");

			tcSelectedSiteTreeComponent.add(tcServerTreeComponent);
		  }

		  // lsNICs = new StringsListHolder();

		  // System.out.println("called CORBA: scStreamingServersManager.getNICs");

		  // if (scStreamingServersManager.getNICs(lsNICs) != 0) {
		  //  JOptionPane.showMessageDialog(null,
		  //    "scStreamingServersManager.getNICs failed",
		  //	"AddServers", JOptionPane.ERROR_MESSAGE);

		  //  return 4;
		  //}

		  for (iConfigurationItemIndex = 0; iConfigurationItemIndex <
			 cilConfigurationItemsWithInfoList.value.length; iConfigurationItemIndex++)
		  {
			// Verify if the section name already exist
			{
			  TreeVisit tvTreeVisit;
			  TreeVisitToFindTreeComponent tvftcTreeVisitToFindTreeComponent;

			  tvftcTreeVisitToFindTreeComponent = new
				TreeVisitToFindTreeComponent(
				"ConfigurationSection",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pSectionName);
			  tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

			  if (tvTreeVisit.preOrderVisit(tcServerTreeComponent) != 0) {
				JOptionPane.showMessageDialog(null,
											 "preOrderVisit failed",
											 "AddServers",
											 JOptionPane.PLAIN_MESSAGE);

				if (bUpdateConfigurationFile &&
				  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
				  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
				  JOptionPane.showMessageDialog(null,
					"rollbackGUIConfigurationTransaction failed",
					"AddServers", JOptionPane.ERROR_MESSAGE);
				}

				return 5;
			  }

			  tcConfigurationSectionTreeComponent =
				tvftcTreeVisitToFindTreeComponent.getTreeComponentFound();
			}

			if (tcConfigurationSectionTreeComponent == null) {
			  tcConfigurationSectionTreeComponent =
				xtXMLTree.buildTreeComponent();
			  tcConfigurationSectionTreeComponent.setAttribute(
				"TagName", "ConfigurationSection");
			  tcConfigurationSectionTreeComponent.setAttribute(
				"Identifier",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pSectionName);
			  tcConfigurationSectionTreeComponent.setAttribute(
				"Label",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pSectionName);
			  tcConfigurationSectionTreeComponent.setAttribute(
				"Icon", "ConfigurationSection.gif");
			  tcConfigurationSectionTreeComponent.setAttribute(
				"ToolTip",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pSectionName + " Configuration Section");
			  tcConfigurationSectionTreeComponent.setAttribute(
				"DisabledFunctionalitiesList",
				"AddSite,DeleteSite,AddServers,DeleteServer,ActivateServer,DeactivateServer,ShutdownServer,ChangeInEverySiteServer,ChangeInEveryServer");
			  tcConfigurationSectionTreeComponent.setAttribute(
				"DisabledFunctionalitiesListSeparator", ",");
			  tcServerTreeComponent.add(tcConfigurationSectionTreeComponent);
			}

			if (cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				itItemType == StreamingIDL.ItemType.ENUMERATIVE)
			{
			  tcConfigurationItemTreeComponent =
				xtXMLTree.buildChoiceFromValues();
			  tcConfigurationItemTreeComponent.setAttribute(
				"Identifier", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemName);
			  tcConfigurationItemTreeComponent.setAttribute(
				"Label", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemName);
			  tcConfigurationItemTreeComponent.setAttribute(
				"Icon", "ConfigurationItem.gif");
			  tcConfigurationItemTreeComponent.setAttribute(
				"ToolTip", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemComment);
			  tcConfigurationItemTreeComponent.setAttribute(
				"ValueIcon", "ConfigurationItemValue.gif");
			  tcConfigurationItemTreeComponent.setAttribute(
				"Mandatory", "yes");
			  tcConfigurationItemTreeComponent.setAttribute(
				"ValuesSeparator", ",");
			  for (iEnumerativeIndex = 0; iEnumerativeIndex <
				   cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].pValues.length;
				   iEnumerativeIndex++)
			  {
				tcConfigurationItemTreeComponent.setAttribute(
				  "Value" + (iEnumerativeIndex + 1),
				  cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				  pValues[iEnumerativeIndex]);
			  }
			  tcConfigurationItemTreeComponent.setAttribute(
				"SelectionMode", cilConfigurationItemsWithInfoList.value[
				iConfigurationItemIndex].pSelectionMode);
			  tcConfigurationItemTreeComponent.setAttribute(
				"DisabledFunctionalitiesList",
				"AddSite,DeleteSite,AddServers,DeleteServer,ActivateServer,DeactivateServer,ShutdownServer");
			  tcConfigurationItemTreeComponent.setAttribute(
				"DisabledFunctionalitiesListSeparator", ",");

			  if (tcConfigurationItemTreeComponent.addUserInput(
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemValue, "ConfigurationItemValue.gif",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemValue, null) == null)
			  {
				JOptionPane.showMessageDialog(null, "addUserInput failed",
				  "AddServers", JOptionPane.ERROR_MESSAGE);

				if (bUpdateConfigurationFile &&
				  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
				  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			      JOptionPane.showMessageDialog(null,
				    "rollbackGUIConfigurationTransaction failed",
				    "AddServers", JOptionPane.ERROR_MESSAGE);
			    }

				return 6;
			  }
			  tcConfigurationSectionTreeComponent.add(
				tcConfigurationItemTreeComponent);
		    }
			else if (cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				itItemType == StreamingIDL.ItemType.NUMBER)
			{
			  tcConfigurationItemTreeComponent =
				xtXMLTree.buildChoiceFromNumberInput();
			  tcConfigurationItemTreeComponent.setAttribute(
				"Identifier", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemName);
			  tcConfigurationItemTreeComponent.setAttribute(
				"Label", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemName);
			  tcConfigurationItemTreeComponent.setAttribute(
				"Icon", "ConfigurationItem.gif");
			  tcConfigurationItemTreeComponent.setAttribute(
				"ToolTip", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemComment);
			  tcConfigurationItemTreeComponent.setAttribute(
				"ValueIcon", "ConfigurationItemValue.gif");
			  tcConfigurationItemTreeComponent.setAttribute(
				"Mandatory", "yes");
			  tcConfigurationItemTreeComponent.setAttribute(
				"IsInteger", "yes");
			  tcConfigurationItemTreeComponent.setAttribute(
				"DisabledFunctionalitiesList",
				"AddSite,DeleteSite,AddServers,DeleteServer,ActivateServer,DeactivateServer,ShutdownServer");
			  tcConfigurationItemTreeComponent.setAttribute(
				"DisabledFunctionalitiesListSeparator", ",");
			  if (tcConfigurationItemTreeComponent.addUserInput(
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemValue, "ConfigurationItemValue.gif",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemValue, null) == null)
	          {
				JOptionPane.showMessageDialog(null, "addUserInput failed",
				  "AddServers", JOptionPane.ERROR_MESSAGE);

			    if (bUpdateConfigurationFile &&
				  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
				  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			      JOptionPane.showMessageDialog(null,
			    	"rollbackGUIConfigurationTransaction failed",
				    "AddServers", JOptionPane.ERROR_MESSAGE);
			    }

				return 7;
			  }
			  tcConfigurationSectionTreeComponent.add(
				tcConfigurationItemTreeComponent);
			}
			else if (cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				itItemType == StreamingIDL.ItemType.TEXT)
			{
			  tcConfigurationItemTreeComponent =
				xtXMLTree.buildChoiceFromTextInput();
			  tcConfigurationItemTreeComponent.setAttribute(
				"Identifier",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemName);
			  tcConfigurationItemTreeComponent.setAttribute(
				"Label",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemName);
			  tcConfigurationItemTreeComponent.setAttribute(
				"Icon", "ConfigurationItem.gif");
			  tcConfigurationItemTreeComponent.setAttribute(
				"ToolTip", cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemComment);
			  tcConfigurationItemTreeComponent.setAttribute(
				"ValueIcon", "ConfigurationItemValue.gif");
			  tcConfigurationItemTreeComponent.setAttribute(
				"Mandatory", "yes");
	          /*
			  tcConfigurationItemTreeComponent.setAttribute(
			    "IsInteger", "yes");
			  tcConfigurationItemTreeComponent.setAttribute(
			    "MinValue", "0");
			  tcConfigurationItemTreeComponent.setAttribute(
			    "MaxValue", "2000");
			  */
			  tcConfigurationItemTreeComponent.setAttribute(
				"DisabledFunctionalitiesList",
				"AddSite,DeleteSite,AddServers,DeleteServer,ActivateServer,DeactivateServer,ShutdownServer");
			  tcConfigurationItemTreeComponent.setAttribute(
				"DisabledFunctionalitiesListSeparator", ",");
			  if (tcConfigurationItemTreeComponent.addUserInput(
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemValue, "ConfigurationItemValue.gif",
				cilConfigurationItemsWithInfoList.value[iConfigurationItemIndex].
				ciConfigurationItem.pItemValue, null) == null)
	          {
				JOptionPane.showMessageDialog(null, "addUserInput failed",
				  "AddServer", JOptionPane.ERROR_MESSAGE);

			    if (bUpdateConfigurationFile &&
				  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
				  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			      JOptionPane.showMessageDialog(null,
				    "rollbackGUIConfigurationTransaction failed",
				    "AddServers", JOptionPane.ERROR_MESSAGE);
			    }

				return 8;
			  }
			  tcConfigurationSectionTreeComponent.add(
				tcConfigurationItemTreeComponent);
			}
			else
			{
				JOptionPane.showMessageDialog(null,
				  "Configuration item type wrong",
				  "AddServer", JOptionPane.ERROR_MESSAGE);

			    if (bUpdateConfigurationFile &&
				  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
				  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			      JOptionPane.showMessageDialog(null,
				    "rollbackGUIConfigurationTransaction failed",
				    "AddServers", JOptionPane.ERROR_MESSAGE);
			    }

				return 9;
			}
	    }

		tcSelectedSiteTreeComponent.refreshSubTree();

		_vLastSavedDateForConfigurationItems.setElementAt(new Date(), 0);

		// Update of XMLTreeForContents
		{
		  TreeComponent tcXMLTopTreeComponent;
		  TreeVisitToFindTreeComponent tvftcTreeVisitToFindTreeComponent;
		  TreeVisit tvTreeVisit;
		  TreeComponent tcSiteInXMLTreeContents;
		  TreeComponent tcContentTreeComponent;

		  tcXMLTopTreeComponent = _xtfcXMLTreeForContents.
			getXMLTopTreeComponent();

		  // search the site in the contents tree
		  tvftcTreeVisitToFindTreeComponent = new TreeVisitToFindTreeComponent(
			"Site", tcSelectedSiteTreeComponent.getAttributeValue("Identifier"));
		  tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

		  if (tvTreeVisit.preOrderVisit(tcXMLTopTreeComponent) != 0) {
			JOptionPane.showMessageDialog(null,
										 "preOrderVisit failed", "AddServers",
										 JOptionPane.ERROR_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 10;
		  }

		  if ( (tcSiteInXMLTreeContents =
				tvftcTreeVisitToFindTreeComponent.getTreeComponentFound()) ==
			  null) {
			JOptionPane.showMessageDialog(null,
										 "Site not exists in the contents tree",
										 "AddServers",
										 JOptionPane.PLAIN_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 11;
		  }

		  tcServerTreeComponent = xtXMLTree.buildTreeComponent();
		  tcServerTreeComponent.setAttribute("TagName", "Entity");
		  tcServerTreeComponent.setAttribute("Identifier",
		    (String) vServersNames.elementAt(iServerIndex));
		  tcServerTreeComponent.setAttribute("Label",
		    (String) vServersNames.elementAt(iServerIndex) +
			" (HostName: " + shHostName.value + ")");
		  tcServerTreeComponent.setAttribute("HostName", shHostName.value);
		  tcServerTreeComponent.setAttribute("Icon", "Server.gif");
		  tcServerTreeComponent.setAttribute("ToolTip",
		    (String) vServersNames.elementAt(iServerIndex) +
			" Streaming Server");
		  tcServerTreeComponent.setAttribute("DisabledFunctionalitiesList",
											 "AddDirectory,DeleteDirectory,AddContent,DeleteContent,DuplicateContent,SaveContentLocally,DumpContent,GetTracksInfo,AddHintTrack,AddHintTracks,DeleteTrack,PropagationRules");
		  tcServerTreeComponent.setAttribute(
			"DisabledFunctionalitiesListSeparator", ",");

		  tcContentTreeComponent = xtXMLTree.buildTreeComponent();
		  tcContentTreeComponent.setAttribute("TagName", "ContentRoot");
		  tcContentTreeComponent.setAttribute("Identifier", "ContentRoot");
		  tcContentTreeComponent.setAttribute("Label", "ContentRoot");
		  tcContentTreeComponent.setAttribute("Icon", "ContentRoot.gif");
		  tcContentTreeComponent.setAttribute("ToolTip", "Root Directory");
		  tcContentTreeComponent.setAttribute("PathName", "");
		  tcContentTreeComponent.setAttribute("DisabledFunctionalitiesList",
											  "DeleteDirectory,DeleteContent,DuplicateContent,SaveContentLocally,DumpContent,GetTracksInfo,AddHintTrack,AddHintTracks,DeleteTrack,PropagationRules");
		  tcContentTreeComponent.setAttribute(
			"DisabledFunctionalitiesListSeparator", ",");

		  if (fillTreeWithContents("", tcContentTreeComponent,
		    xtXMLTree, ssStreamingServer) != 0)
		  {
			JOptionPane.showMessageDialog(null,
			  "fillTreeWithContents failed",
			  "AddServers", JOptionPane.ERROR_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 12;
		  }

		  tcServerTreeComponent.add(tcContentTreeComponent);
		  tcSiteInXMLTreeContents.add(tcServerTreeComponent);
		  tcXMLTopTreeComponent.refreshSubTree();
		}

		// Update of XMLTreeForMonitor
		{
		  TreeComponent tcXMLTopTreeComponent;
		  TreeVisitToFindTreeComponent tvftcTreeVisitToFindTreeComponent;
		  TreeVisit tvTreeVisit;
		  TreeComponent tcSiteInXMLTreeMonitor;
		  TreeComponent tcMonitorTreeComponent;

		  tcXMLTopTreeComponent = _xtfmXMLTreeForMonitor.getXMLTopTreeComponent();

		  // search the site in the contents tree
		  tvftcTreeVisitToFindTreeComponent = new TreeVisitToFindTreeComponent(
			"Site", tcSelectedSiteTreeComponent.getAttributeValue("Identifier"));
		  tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

		  if (tvTreeVisit.preOrderVisit(tcXMLTopTreeComponent) != 0)
		  {
			JOptionPane.showMessageDialog(null,
			  "preOrderVisit failed", "AddServers",
			  JOptionPane.ERROR_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 13;
		  }

		  if ( (tcSiteInXMLTreeMonitor =
				tvftcTreeVisitToFindTreeComponent.getTreeComponentFound()) ==
			  null) {
			JOptionPane.showMessageDialog(null,
			  "Site not exists in the contents tree",
			  "AddServers", JOptionPane.PLAIN_MESSAGE);

			if (bUpdateConfigurationFile &&
			  ConfigurationTransaction.rollbackGUIConfigurationTransaction(
			  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			  JOptionPane.showMessageDialog(null,
				"rollbackGUIConfigurationTransaction failed",
				"AddServers", JOptionPane.ERROR_MESSAGE);
			}

			return 14;
		  }

		  tcServerTreeComponent = xtXMLTree.buildTreeComponent();
		  tcServerTreeComponent.setAttribute("TagName", "Server");
		  tcServerTreeComponent.setAttribute("Identifier",
		    (String) vServersNames.elementAt(iServerIndex));
		  tcServerTreeComponent.setAttribute("Label",
		    (String) vServersNames.elementAt(iServerIndex) +
			" (HostName: " + shHostName.value +
			")");
		  tcServerTreeComponent.setAttribute("HostName", shHostName.value);
		  tcServerTreeComponent.setAttribute("Icon", "Server.gif");
		  tcServerTreeComponent.setAttribute("ToolTip",
		    (String) vServersNames.elementAt(iServerIndex) +
			" Streaming Server");
		  tcServerTreeComponent.setAttribute("DisabledFunctionalitiesList",
											 "");
		  tcServerTreeComponent.setAttribute(
			"DisabledFunctionalitiesListSeparator", ",");

		  tcMonitorTreeComponent = xtXMLTree.buildTreeComponent();
		  tcMonitorTreeComponent.setAttribute("TagName", "Monitor");
		  tcMonitorTreeComponent.setAttribute("Identifier",
		    "ServerConnectedUsers");
		  tcMonitorTreeComponent.setAttribute("Label", "Server Connected Users");
		  tcMonitorTreeComponent.setAttribute("Icon", "ServerConnectedUsers.gif");
		  tcMonitorTreeComponent.setAttribute("ToolTip", "Monitor of Server Connected Users");
		  tcMonitorTreeComponent.setAttribute("DisabledFunctionalitiesList",
											  "");
		  tcMonitorTreeComponent.setAttribute(
			"DisabledFunctionalitiesListSeparator", ",");
		  tcServerTreeComponent.add(tcMonitorTreeComponent);

		  tcSiteInXMLTreeMonitor.add(tcServerTreeComponent);
		  tcXMLTopTreeComponent.refreshSubTree();
		}
	  }
    }

	if (bUpdateConfigurationFile &&
	  ConfigurationTransaction.commitGUIConfigurationTransaction (
	  _xtfcXMLTreeForConfiguration,
	  _xtfcXMLTreeForConfiguration.getFunctionalities()) != 0)
	{
	  JOptionPane.showMessageDialog(null,
	    "commitGUIConfigurationTransaction failed",
		"AddServers", JOptionPane.ERROR_MESSAGE);

	  return 15;
	}
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ERROR: " + e,
        "AddServers", JOptionPane.ERROR_MESSAGE);

	  if (bUpdateConfigurationFile &&
	    ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		  JOptionPane.showMessageDialog(null,
			"rollbackGUIConfigurationTransaction failed",
			"AddServers", JOptionPane.ERROR_MESSAGE);
		}

      return 16;
    }

	return 0;
  }

  public int fillTreeWithContents (String sDirectory,
    TreeComponent tcTreeComponent, XMLTree xtXMLTree,
    StreamingServer ssStreamingServer)
  {
    StringsListHolder   lsFileNames = new StringsListHolder ();
    StringsListHolder   lsDirectories = new StringsListHolder ();
    TreeComponent       tcLocalTreeComponent;
    int                 iIndex;
	long                lErrorNumber;


    System. out. println ("called CORBA: ssStreamingServer.getContents");

    try
    {
      if ((lErrorNumber = ssStreamingServer.getContents(sDirectory,
        lsFileNames, lsDirectories)) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "ssStreamingServer.getContents failed (Error: " +
		  lErrorNumber + ")",
          "AddServers", JOptionPane.ERROR_MESSAGE);

        return 1;
      }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ERROR: " + e,
        "AddServers", JOptionPane.ERROR_MESSAGE);

      return 2;
    }

    for (iIndex = 0; iIndex < lsDirectories.value.length; iIndex++)
    {
      tcLocalTreeComponent        = xtXMLTree.buildTreeComponent();
      tcLocalTreeComponent.setAttribute("TagName", "Directory");
      tcLocalTreeComponent.setAttribute("Identifier", lsDirectories.value[iIndex]);
      tcLocalTreeComponent.setAttribute("Label", lsDirectories.value[iIndex]);
      tcLocalTreeComponent.setAttribute("Icon", "Directory.gif");
      tcLocalTreeComponent.setAttribute("ToolTip",
	    lsDirectories.value[iIndex] + " directory");
      tcLocalTreeComponent.setAttribute("PathName",
        sDirectory.equals("") ? lsDirectories.value[iIndex] :
        sDirectory + "/" + lsDirectories.value[iIndex]);
      tcLocalTreeComponent.setAttribute("DisabledFunctionalitiesList",
        "DeleteContent,DuplicateContent,SaveContentLocally,DumpContent,GetTracksInfo,AddHintTrack,AddHintTracks,DeleteTrack,PropagationRules");
      tcLocalTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator", ",");

      tcTreeComponent.add (tcLocalTreeComponent);

      if (fillTreeWithContents (
        sDirectory.equals("") ? lsDirectories.value[iIndex] :
        sDirectory + "/" + lsDirectories.value[iIndex],
        tcLocalTreeComponent, xtXMLTree, ssStreamingServer) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "fillTreeWithContents failed",
          "AddServers", JOptionPane.ERROR_MESSAGE);

        return 3;
      }
    }

    for (iIndex = 0; iIndex < lsFileNames.value.length; iIndex++)
    {
      tcLocalTreeComponent        = xtXMLTree.buildTreeComponent();
      tcLocalTreeComponent.setAttribute("TagName", "Content");
      tcLocalTreeComponent.setAttribute("Identifier", lsFileNames.value[iIndex]);
      tcLocalTreeComponent.setAttribute("Label", lsFileNames.value[iIndex]);
      tcLocalTreeComponent.setAttribute("Icon", "Content.gif");
      tcLocalTreeComponent.setAttribute("ToolTip",
	    lsFileNames.value[iIndex] + " clip");
      tcLocalTreeComponent.setAttribute("PathName",
        sDirectory.equals("") ? lsFileNames.value[iIndex] :
        sDirectory + "/" + lsFileNames.value[iIndex]);
      tcLocalTreeComponent.setAttribute("DisabledFunctionalitiesList",
        "AddDirectory,DeleteDirectory,AddContent,PropagationRules");
      tcLocalTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator", ",");

      tcTreeComponent.add (tcLocalTreeComponent);
    }

    return 0;
  }
}

