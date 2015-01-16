package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;
import StreamingIDL.*;
import org.omg.CORBA.ORB;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;

public class ActivateServerConfiguration extends Functionality
{

  ORB                             _orb;
  Vector                          _vLastSavedDateForConfigurationItems;

  public ActivateServerConfiguration (Frame fParentFrame,
    ORB orb, Vector vLastSavedDateForConfigurationItems)
  {
    super (fParentFrame);
    _orb                                    = orb;
    _vLastSavedDateForConfigurationItems    = vLastSavedDateForConfigurationItems;
  }

  public void realize (XMLTree xtXMLTree)
  {
    try
    {
      TreeVisit                           tvTreeVisit;
      TreeVisitToFindConfigurationChanges     tvtccTreeVisitToFindConfigurationChanges;
      Vector                              vConfigurationItems = new Vector ();
      StreamingServer                     ssStreamingServer;
      TreeComponent                       tcSelectedConfigurationTreeComponent;
      TreeComponent                       tcServerNode;
      ConfigurationItem                   lsciConfigurationItem [];
      int                                 iConfigurationItemIndex;
      String                              sServerName;
	  long                                lErrorNumber;


      tcSelectedConfigurationTreeComponent      =
        xtXMLTree.getSelectedTreeComponent();

      if ((tcServerNode = tcSelectedConfigurationTreeComponent.getEntityNode()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "Server not found",
          "ActivateServerConfiguration",
          JOptionPane.ERROR_MESSAGE);

        return;
      }

      tvtccTreeVisitToFindConfigurationChanges      =
        new TreeVisitToFindConfigurationChanges (vConfigurationItems,
        _vLastSavedDateForConfigurationItems);
      tvTreeVisit       = new TreeVisit (tvtccTreeVisitToFindConfigurationChanges);

      if (tvTreeVisit.inOrderVisit(tcServerNode) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "inOrderVisit failed", "ActivateServerConfiguration",
          JOptionPane.ERROR_MESSAGE);

        // ricaricare la configurazione del server in quanto sono state perse le
        // modifiche

        return;
      }

      if (vConfigurationItems.size() == 0)
      {
        JOptionPane.showMessageDialog (null,
          "You didn't change any parameters", "ActivateServerConfiguration",
          JOptionPane.ERROR_MESSAGE);

        return;
      }

      lsciConfigurationItem         = new ConfigurationItem [
        vConfigurationItems.size()];
      for (iConfigurationItemIndex = 0;
        iConfigurationItemIndex < vConfigurationItems.size(); iConfigurationItemIndex++)
        lsciConfigurationItem [iConfigurationItemIndex]       =
          (ConfigurationItem) vConfigurationItems.elementAt(iConfigurationItemIndex);

	  sServerName = tcServerNode.getAttributeValue("Identifier");

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  ssStreamingServer = StreamingServerHelper.narrow(
		nc.resolve(nc.to_name(sServerName + "_Server")));

      if ((lErrorNumber = ssStreamingServer.setChangedConfiguration (
		lsciConfigurationItem)) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "ssStreamingServer.setConfiguration failed (Error: " +
		  lErrorNumber + ")",
          "ActivateServerConfiguration",
          JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ActivateServerConfiguration ERROR: " + e,
        "ActivateServerConfiguration",
        JOptionPane.ERROR_MESSAGE);

      return;
    }
  }
}
