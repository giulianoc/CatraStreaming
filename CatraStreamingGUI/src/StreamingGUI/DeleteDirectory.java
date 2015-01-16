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


public class DeleteDirectory extends Functionality

{
  ORB                             _orb;

  public DeleteDirectory (Frame fParentFrame,
    ORB orb)
  {
    super (fParentFrame);
    _orb                          = orb;
  }

  public void realize (XMLTree xtXMLTree)
  {
    try
    {
      TreeComponent                       tcSelectedDirectoryTreeComponent;
      TreeComponent                       tcDirectoryTreeComponentParent;
      StreamingServer                     ssStreamingServer;
      TreeComponent                       tcServerTreeComponent;
      String                              sServerIOR;
	  long                                lErrorNumber;


      if (JOptionPane.showOptionDialog (null,
        "Are you sure?",
        "DeleteSite", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null) == 1)
        return;

      tcSelectedDirectoryTreeComponent          =
        xtXMLTree.getSelectedTreeComponent();

      tcDirectoryTreeComponentParent    =
        (TreeComponent) tcSelectedDirectoryTreeComponent.getParent();

      if ((tcServerTreeComponent = tcSelectedDirectoryTreeComponent.getEntityNode()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "Server not found",
          "DeleteDirectory",
          JOptionPane.ERROR_MESSAGE);

        return;
      }

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  ssStreamingServer = StreamingServerHelper.narrow(
		nc.resolve(nc.to_name(tcServerTreeComponent.getAttributeValue("Identifier") + "_Server")));

      if ((lErrorNumber = ssStreamingServer.deleteDirectory(
        tcSelectedDirectoryTreeComponent.getAttributeValue("PathName"))) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "ssStreamingServer.addDirectory failed (Error: " +
		  lErrorNumber + ")",
          "DeleteDirectory",
          JOptionPane.ERROR_MESSAGE);

        return;
      }

      xtXMLTree.setSelectedTreeComponent(tcDirectoryTreeComponentParent);
      tcDirectoryTreeComponentParent.remove(tcSelectedDirectoryTreeComponent);
      xtXMLTree.getXMLTopTreeComponent().refreshSubTree();
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "DeleteDirectory ERROR: " + e,
        "DeleteDirectory",
        JOptionPane.ERROR_MESSAGE);

      return;
    }
  }
}
