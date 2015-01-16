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


public class DeleteContent extends Functionality

{
  ORB                             _orb;

  public DeleteContent (Frame fParentFrame,
    ORB orb)
  {
    super (fParentFrame);
    _orb                          = orb;
  }

  public void realize (XMLTree xtXMLTree)
  {
    try
    {
      TreeComponent                       tcSelectedContentTreeComponent;
      TreeComponent                       tcDirectoryTreeComponent;
      StreamingServer                     ssStreamingServer;
      TreeComponent                       tcServerTreeComponent;
      String                              sServerIOR;
      String                              sDirectory;
      String                              sFileName;
	  long                                lErrorNumber;


      if (JOptionPane.showOptionDialog (null,
        "Are you sure?",
        "DeleteSite", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null) == 1)
        return;

      tcSelectedContentTreeComponent          =
        xtXMLTree.getSelectedTreeComponent();

      tcDirectoryTreeComponent    =
        (TreeComponent) tcSelectedContentTreeComponent.getParent();

      if ((tcServerTreeComponent = tcSelectedContentTreeComponent.getEntityNode()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "Server not found",
          "DeleteContent",
          JOptionPane.ERROR_MESSAGE);

        return;
      }

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  ssStreamingServer = StreamingServerHelper.narrow(
		nc.resolve(nc.to_name(tcServerTreeComponent.getAttributeValue("Identifier") + "_Server")));

      sDirectory          = tcDirectoryTreeComponent.getAttributeValue("PathName");
      sFileName           = tcSelectedContentTreeComponent.getAttributeValue("Identifier");

      if ((lErrorNumber = ssStreamingServer.deleteContent(
        sDirectory.equals("") ? sFileName :
        sDirectory + "/" + sFileName)) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "ssStreamingServer.deleteContent failed (Error: " +
		  lErrorNumber + ")",
          "DeleteContent",
          JOptionPane.ERROR_MESSAGE);

        return;
      }

      xtXMLTree.setSelectedTreeComponent(tcDirectoryTreeComponent);
      tcDirectoryTreeComponent.remove(tcSelectedContentTreeComponent);
      xtXMLTree.getXMLTopTreeComponent().refreshSubTree();
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "DeleteContent ERROR: " + e,
        "DeleteContent",
        JOptionPane.ERROR_MESSAGE);

      return;
    }
  }
}

