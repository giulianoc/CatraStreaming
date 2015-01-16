package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;


public class AddSite extends Functionality

{
  Properties                    _pProperties;
  XMLTreeForConfiguration       _xtfcXMLTreeForConfiguration;
  XMLTreeForContents            _xtfcXMLTreeForContents;
  XMLTreeForMonitor             _xtfmXMLTreeForMonitor;


  public AddSite (Frame fParentFrame,
    Properties pProperties,
	XMLTreeForConfiguration xtfcXMLTreeForConfiguration,
    XMLTreeForContents xtfcXMLTreeForContents,
    XMLTreeForMonitor xtfmXMLTreeForMonitor)
  {
    super (fParentFrame);
	_pProperties                 = pProperties;
	_xtfcXMLTreeForConfiguration = xtfcXMLTreeForConfiguration;
    _xtfcXMLTreeForContents      = xtfcXMLTreeForContents;
    _xtfmXMLTreeForMonitor       = xtfmXMLTreeForMonitor;
  }

  public void realize (XMLTree xtXMLTree)
  {
    TreeComponent                   tcSelectedConfigurationsTreeComponent;
    String                          sSelectedText;

    ChoiceFromTextInputDialog       tidTextInputDialog      = null;


    try
    {
      tcSelectedConfigurationsTreeComponent      =
        xtXMLTree.getSelectedTreeComponent();

      tidTextInputDialog = new ChoiceFromTextInputDialog (_fParentFrame,
        "Add site", "", true);
      tidTextInputDialog.setVisible(true);

      sSelectedText     = tidTextInputDialog. getSelectedText ();

      if (sSelectedText != null)
      {
        if (addSite (xtXMLTree, tcSelectedConfigurationsTreeComponent,
		  sSelectedText, true) == null)
        {
          JOptionPane.showMessageDialog (null,
            "addSite failed", "AddSite",
            JOptionPane.ERROR_MESSAGE);

          return;
        }
      }
    }
    finally
    {
      if (tidTextInputDialog != null)
        tidTextInputDialog.dispose();
    }

    return;
  }

  public TreeComponent addSite(XMLTree xtXMLTree,
    TreeComponent tcSelectedConfigurationsTreeComponent,
	String sSelectedText, boolean bUpdateConfigurationFile)

  {

	TreeComponent tcConfigSiteTreeComponent;
	String sSelectedTextExcerpt;
	int iAllowedLength = 20;
	boolean bExceedAllowedLength;
	TreeVisit tvTreeVisit;
	TreeVisitToFindTreeComponent tvftcTreeVisitToFindTreeComponent;

	if (bUpdateConfigurationFile &&
		ConfigurationTransaction.beginGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile(),
		_pProperties.getProperty("login")) != 0)
	{
	  JOptionPane.showMessageDialog(null,
	    "beginGUIConfigurationTransaction failed",
		"AddSite", JOptionPane.ERROR_MESSAGE);

	  return null;
	}

	bExceedAllowedLength = (sSelectedText.length() > iAllowedLength);
	if (bExceedAllowedLength)
	  sSelectedTextExcerpt = sSelectedText.substring(0, iAllowedLength - 1) +
	  " ...";
	else
	  sSelectedTextExcerpt = sSelectedText;

	// verify if the site already exists
	tvftcTreeVisitToFindTreeComponent = new TreeVisitToFindTreeComponent(
	"Site", sSelectedText);
	tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

	if (tvTreeVisit.preOrderVisit(tcSelectedConfigurationsTreeComponent) != 0)
	{
	  if (bUpdateConfigurationFile &&
	    ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		  JOptionPane.showMessageDialog(null,
		    "rollbackGUIConfigurationTransaction failed",
		    "AddSite", JOptionPane.ERROR_MESSAGE);
	  }

	  JOptionPane.showMessageDialog(null,
	    "preOrderVisit failed", "AddSite",
		JOptionPane.ERROR_MESSAGE);

	  return null;
	}

	if (tvftcTreeVisitToFindTreeComponent.getTreeComponentFound() !=
		null)
	{
	  if (bUpdateConfigurationFile &&
	    ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		JOptionPane.showMessageDialog(null,
		  "rollbackGUIConfigurationTransaction failed",
		  "AddSite", JOptionPane.ERROR_MESSAGE);
	  }

	  JOptionPane.showMessageDialog(null,
	    "Site already exists", "AddSite",
		JOptionPane.PLAIN_MESSAGE);

	  return null;
	}

	tcConfigSiteTreeComponent = xtXMLTree.buildTreeComponent();
	tcConfigSiteTreeComponent.setAttribute("TagName", "Site");
	tcConfigSiteTreeComponent.setAttribute("Identifier", sSelectedText);
	tcConfigSiteTreeComponent.setAttribute("Label", sSelectedTextExcerpt);
	tcConfigSiteTreeComponent.setAttribute("Icon", "Site.gif");
	tcConfigSiteTreeComponent.setAttribute("ToolTip", sSelectedTextExcerpt + " Site");
	tcConfigSiteTreeComponent.setAttribute("DisabledFunctionalitiesList",
									 "AddSite,DeleteServer,ActivateServer,DeactivateServer,ShutdownServer,ActivateServerConfiguration,ChangeInEverySiteServer,ChangeInEveryServer");
	tcConfigSiteTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator",
									 ",");

	tcSelectedConfigurationsTreeComponent.add(tcConfigSiteTreeComponent);
	tcSelectedConfigurationsTreeComponent.refreshSubTree();

	// XMLTreeForContents
	{
	  TreeComponent tcSiteTreeComponent;
	  TreeComponent tcXMLTopTreeComponent;

	  tcXMLTopTreeComponent = _xtfcXMLTreeForContents.getXMLTopTreeComponent();
	  tcSiteTreeComponent = xtXMLTree.buildTreeComponent();
	  tcSiteTreeComponent.setAttribute("TagName", "Site");
	  tcSiteTreeComponent.setAttribute("Identifier", sSelectedText);
	  tcSiteTreeComponent.setAttribute("Label", sSelectedTextExcerpt);
	  tcSiteTreeComponent.setAttribute("Icon", "Site.gif");
	  tcSiteTreeComponent.setAttribute("ToolTip", sSelectedTextExcerpt + " Site");
	  tcSiteTreeComponent.setAttribute("DisabledFunctionalitiesList",
									   "AddDirectory,DeleteDirectory,AddContent,DeleteContent,DuplicateContent,SaveContentLocally,DumpContent,GetTracksInfo,AddHintTrack,AddHintTracks,DeleteTrack");
	  tcSiteTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator",
									   ",");

	  tcXMLTopTreeComponent.add(tcSiteTreeComponent);
	  tcXMLTopTreeComponent.refreshSubTree();
	}

	// XMLTreeForMonitor
	{
	  TreeComponent tcSiteTreeComponent;
	  TreeComponent tcXMLTopTreeComponent;
	  TreeComponent tcMonitorTreeComponent;

	  tcXMLTopTreeComponent = _xtfmXMLTreeForMonitor.getXMLTopTreeComponent();
	  tcSiteTreeComponent = xtXMLTree.buildTreeComponent();
	  tcSiteTreeComponent.setAttribute("TagName", "Site");
	  tcSiteTreeComponent.setAttribute("Identifier", sSelectedText);
	  tcSiteTreeComponent.setAttribute("Label", sSelectedTextExcerpt);
	  tcSiteTreeComponent.setAttribute("Icon", "Site.gif");
	  tcSiteTreeComponent.setAttribute("ToolTip", sSelectedTextExcerpt + " Site");
	  tcSiteTreeComponent.setAttribute("DisabledFunctionalitiesList",
									   "");
	  tcSiteTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator",
									   ",");

	  tcMonitorTreeComponent = xtXMLTree.buildTreeComponent();
	  tcMonitorTreeComponent.setAttribute("TagName", "Monitor");
	  tcMonitorTreeComponent.setAttribute("Identifier", "SiteConnectedUsers");
	  tcMonitorTreeComponent.setAttribute("Label", "Site Connected Users");
	  tcMonitorTreeComponent.setAttribute("Icon", "SiteConnectedUsers.gif");
	  tcMonitorTreeComponent.setAttribute("ToolTip", "Monitor of Site Connected Users");
	  tcMonitorTreeComponent.setAttribute("DisabledFunctionalitiesList",
										  "");
	  tcMonitorTreeComponent.setAttribute(
	  "DisabledFunctionalitiesListSeparator", ",");
	  tcSiteTreeComponent.add(tcMonitorTreeComponent);

	  tcXMLTopTreeComponent.add(tcSiteTreeComponent);
	  tcXMLTopTreeComponent.refreshSubTree();
	}

	if (bUpdateConfigurationFile &&
	  ConfigurationTransaction.commitGUIConfigurationTransaction (
	  _xtfcXMLTreeForConfiguration,
	  _xtfcXMLTreeForConfiguration.getFunctionalities()) != 0)
	{
	  JOptionPane.showMessageDialog(null,
	    "commitGUIConfigurationTransaction failed",
		"AddSite", JOptionPane.ERROR_MESSAGE);

	  return null;
	}

	return tcConfigSiteTreeComponent;
  }
}
