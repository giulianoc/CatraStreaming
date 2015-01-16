package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;

public class DeleteServer extends Functionality

{

  Properties                    _pProperties;
  XMLTreeForConfiguration       _xtfcXMLTreeForConfiguration;
  XMLTreeForContents            _xtfcXMLTreeForContents;
  XMLTreeForMonitor             _xtfmXMLTreeForMonitor;

  public DeleteServer (Frame fParentFrame,
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
      TreeComponent                   tcServerTreeComponent;
      TreeComponent                   tcSiteTreeComponent;


      if (JOptionPane.showOptionDialog (null,
        "Are you sure?",
        "DeleteServer", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null) == 1)
        return;

	  if (ConfigurationTransaction.beginGUIConfigurationTransaction(
		_xtfcXMLTreeForConfiguration.getXML().getFile(),
		_pProperties.getProperty("login")) != 0)
	  {
	    JOptionPane.showMessageDialog(null,
	      "beginGUIConfigurationTransaction failed",
		  "DeleteServer",
		  JOptionPane.ERROR_MESSAGE);

	    return;
	  }

      tcServerTreeComponent          =
        xtXMLTree.getSelectedTreeComponent();

      tcSiteTreeComponent    = (TreeComponent) tcServerTreeComponent.getParent();

      xtXMLTree.setSelectedTreeComponent(tcSiteTreeComponent);
      tcSiteTreeComponent.remove(tcServerTreeComponent);
      tcSiteTreeComponent.refreshSubTree();

      // Update of XMLTreeForContents
      {
        TreeComponent                 tcXMLTopTreeComponent;
        TreeVisitToFindTreeComponent  tvftcTreeVisitToFindTreeComponent;
        TreeVisit                     tvTreeVisit;
        TreeComponent                 tcServerInXMLTreeContents;
        TreeComponent                 tcSiteInXMLTreeContents;



        tcXMLTopTreeComponent     = _xtfcXMLTreeForContents.getXMLTopTreeComponent ();

        // search the server in the contents tree
        tvftcTreeVisitToFindTreeComponent   = new TreeVisitToFindTreeComponent (
          "Entity", tcServerTreeComponent.getAttributeValue("Identifier"));
        tvTreeVisit             = new TreeVisit (tvftcTreeVisitToFindTreeComponent);

        if (tvTreeVisit.preOrderVisit(tcXMLTopTreeComponent) != 0)
        {
          JOptionPane.showMessageDialog (null,
            "preOrderVisit failed", "DeleteServer",
            JOptionPane.ERROR_MESSAGE);

		  if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		    _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			JOptionPane.showMessageDialog(null,
			  "rollbackGUIConfigurationTransaction failed",
			  "DeleteServer", JOptionPane.ERROR_MESSAGE);
		  }

          return;
        }

        if ((tcServerInXMLTreeContents =
          tvftcTreeVisitToFindTreeComponent.getTreeComponentFound ()) ==
          null)
        {
          JOptionPane.showMessageDialog (null,
            "Server not exists in the contents tree", "DeleteServer",
            JOptionPane.PLAIN_MESSAGE);

		  if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		    _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			JOptionPane.showMessageDialog(null,
			  "rollbackGUIConfigurationTransaction failed",
			  "DeleteServer", JOptionPane.ERROR_MESSAGE);
		  }

          return;
        }

        tcSiteInXMLTreeContents   = (TreeComponent) tcServerInXMLTreeContents.getParent();
        tcSiteInXMLTreeContents.remove(tcServerInXMLTreeContents);
        tcSiteInXMLTreeContents.refreshSubTree();
      }

      // Update of XMLTreeForMonitor
      {
        TreeComponent                 tcXMLTopTreeComponent;
        TreeVisitToFindTreeComponent  tvftcTreeVisitToFindTreeComponent;
        TreeVisit                     tvTreeVisit;
        TreeComponent                 tcServerInXMLTreeMonitor;
        TreeComponent                 tcSiteInXMLTreeMonitor;



        tcXMLTopTreeComponent     = _xtfmXMLTreeForMonitor.getXMLTopTreeComponent ();

        // search the server in the monitor tree
        tvftcTreeVisitToFindTreeComponent   = new TreeVisitToFindTreeComponent (
          "Server", tcServerTreeComponent.getAttributeValue("Identifier"));
        tvTreeVisit             = new TreeVisit (tvftcTreeVisitToFindTreeComponent);

        if (tvTreeVisit.preOrderVisit(tcXMLTopTreeComponent) != 0)
        {
          JOptionPane.showMessageDialog (null,
            "preOrderVisit failed", "DeleteServer",
            JOptionPane.ERROR_MESSAGE);

		  if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		    _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			JOptionPane.showMessageDialog(null,
			  "rollbackGUIConfigurationTransaction failed",
			  "DeleteServer", JOptionPane.ERROR_MESSAGE);
		  }

          return;
        }

        if ((tcServerInXMLTreeMonitor =
          tvftcTreeVisitToFindTreeComponent.getTreeComponentFound ()) ==
          null)
        {
          JOptionPane.showMessageDialog (null,
            "Server not exists in the monitor tree", "DeleteServer",
            JOptionPane.PLAIN_MESSAGE);

		  if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
		    _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
			JOptionPane.showMessageDialog(null,
			  "rollbackGUIConfigurationTransaction failed",
			  "DeleteServer", JOptionPane.ERROR_MESSAGE);
		  }

          return;
        }

        tcSiteInXMLTreeMonitor    = (TreeComponent) tcServerInXMLTreeMonitor.getParent();
        tcSiteInXMLTreeMonitor.remove(tcServerInXMLTreeMonitor);
        tcSiteInXMLTreeMonitor.refreshSubTree();
      }

	  if (ConfigurationTransaction.commitGUIConfigurationTransaction (
		_xtfcXMLTreeForConfiguration,
		_xtfcXMLTreeForConfiguration.getFunctionalities()) != 0) {
		JOptionPane.showMessageDialog(null,
		  "commitGUIConfigurationTransaction failed",
		  "DeleteServer", JOptionPane.ERROR_MESSAGE);

		return;
	  }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "DeleteServer ERROR: " + e,
        "DeleteServer", JOptionPane.ERROR_MESSAGE);

	  if (ConfigurationTransaction.rollbackGUIConfigurationTransaction(
	    _xtfcXMLTreeForConfiguration.getXML().getFile()) != 0) {
		JOptionPane.showMessageDialog(null,
		  "rollbackGUIConfigurationTransaction failed",
		  "DeleteServer", JOptionPane.ERROR_MESSAGE);
	  }

      return;
    }
  }
}
