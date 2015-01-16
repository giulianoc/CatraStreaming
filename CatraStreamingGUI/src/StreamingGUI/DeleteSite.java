package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;

public class DeleteSite extends Functionality

{

  Properties                    _pProperties;
  XMLTreeForConfiguration       _xtfcXMLTreeForConfiguration;
  XMLTreeForContents            _xtfcXMLTreeForContents;
  XMLTreeForMonitor             _xtfmXMLTreeForMonitor;

  public DeleteSite (Frame fParentFrame,
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
    try
    {
      TreeComponent                       tcSiteTreeComponent;
      TreeComponent                       tcSiteTreeComponentParent;
      TreeComponent                       tcXMLTopTreeComponent;
      TreeVisit                           tvTreeVisit;
      TreeVisitToFindTreeComponent        tvftcTreeVisitToFindTreeComponent;
      TreeComponent                       tcSiteToRemoveTreeComponent;


      tcSiteTreeComponent          =
        xtXMLTree.getSelectedTreeComponent();

	  if (tcSiteTreeComponent.getChildCount() != 0)
      {
		JOptionPane.showMessageDialog(null,
		  "The site is not empty",
		  "DeleteSite", JOptionPane.ERROR_MESSAGE);

		return;
	  }

      if (JOptionPane.showOptionDialog (null,
        "Are you sure?",
        "DeleteSite", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null) == 1)
        return;

	  if (ConfigurationTransaction.beginGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile(),
		_pProperties.getProperty("login")) != 0)
      {
		JOptionPane.showMessageDialog(null,
		  "beginGUIConfigurationTransaction failed",
		  "DeleteSite", JOptionPane.ERROR_MESSAGE);

		return;
	  }


      tcSiteTreeComponentParent    =
        (TreeComponent) tcSiteTreeComponent.getParent();

      xtXMLTree.setSelectedTreeComponent(tcSiteTreeComponentParent);
      tcSiteTreeComponentParent.remove(tcSiteTreeComponent);
      tcSiteTreeComponentParent.refreshSubTree();

      // XMLTreeForContents
      tcXMLTopTreeComponent     = _xtfcXMLTreeForContents.getXMLTopTreeComponent ();
      tvftcTreeVisitToFindTreeComponent   = new TreeVisitToFindTreeComponent (
        "Site", tcSiteTreeComponent.getAttributeValue("Identifier"));
      tvTreeVisit             = new TreeVisit (tvftcTreeVisitToFindTreeComponent);

      if (tvTreeVisit.preOrderVisit(tcXMLTopTreeComponent) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "preOrderVisit failed", "DeleteSite",
          JOptionPane.ERROR_MESSAGE);

  	    if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		  JOptionPane.showMessageDialog(null,
		    "rollbackGUIConfigurationTransaction failed",
		    "DeleteSite", JOptionPane.ERROR_MESSAGE);
	    }

        return;
      }

      if ((tcSiteToRemoveTreeComponent =
        tvftcTreeVisitToFindTreeComponent.getTreeComponentFound ()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "TreeComponent not found (TagName: Site, Identifier: "
          + tcSiteTreeComponent.getAttributeValue("Identifier") + ")",
          "DeleteSite", JOptionPane.ERROR_MESSAGE);

  	    if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		  JOptionPane.showMessageDialog(null,
		    "rollbackGUIConfigurationTransaction failed",
		    "DeleteSite", JOptionPane.ERROR_MESSAGE);
	    }

        return;
      }
      tcXMLTopTreeComponent.remove(tcSiteToRemoveTreeComponent);
      tcXMLTopTreeComponent.refreshSubTree();

      // XMLTreeForMonitor
      tcXMLTopTreeComponent     = _xtfmXMLTreeForMonitor.getXMLTopTreeComponent ();
      tvftcTreeVisitToFindTreeComponent   = new TreeVisitToFindTreeComponent (
        "Site", tcSiteTreeComponent.getAttributeValue("Identifier"));
      tvTreeVisit             = new TreeVisit (tvftcTreeVisitToFindTreeComponent);

      if (tvTreeVisit.preOrderVisit(tcXMLTopTreeComponent) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "preOrderVisit failed", "DeleteSite",
          JOptionPane.ERROR_MESSAGE);

  	    if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		  JOptionPane.showMessageDialog(null,
		    "rollbackGUIConfigurationTransaction failed",
		    "DeleteSite", JOptionPane.ERROR_MESSAGE);
	    }

        return;
      }

      if ((tcSiteToRemoveTreeComponent =
        tvftcTreeVisitToFindTreeComponent.getTreeComponentFound ()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "TreeComponent not found (TagName: Site, Identifier: "
          + tcSiteTreeComponent.getAttributeValue("Identifier") + ")",
          "DeleteSite", JOptionPane.ERROR_MESSAGE);

  	    if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		  _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		  JOptionPane.showMessageDialog(null,
		    "rollbackGUIConfigurationTransaction failed",
		    "DeleteSite", JOptionPane.ERROR_MESSAGE);
	    }

        return;
      }
      tcXMLTopTreeComponent.remove(tcSiteToRemoveTreeComponent);
      tcXMLTopTreeComponent.refreshSubTree();

	  if (ConfigurationTransaction.commitGUIConfigurationTransaction (
		_xtfcXMLTreeForConfiguration,
		_xtfcXMLTreeForConfiguration.getFunctionalities()) != 0)
	  {
	    JOptionPane.showMessageDialog(null,
	      "commitGUIConfigurationTransaction failed",
		  "DeleteSite", JOptionPane.ERROR_MESSAGE);

	    return;
	  }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "DeleteSite ERROR: " + e,
        "DeleteSite", JOptionPane.ERROR_MESSAGE);

      if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
	    _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
	    JOptionPane.showMessageDialog(null,
	      "rollbackGUIConfigurationTransaction failed",
	      "DeleteSite", JOptionPane.ERROR_MESSAGE);
      }

      return;
    }
  }
}
