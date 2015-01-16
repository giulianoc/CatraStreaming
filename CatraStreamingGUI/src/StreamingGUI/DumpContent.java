package StreamingGUI;

import java.util.*;
import java.net.*;
import javax.swing.*;
import java.io.*;
import java.awt.Frame;
import java.util.StringTokenizer;
import XMLTree.*;
import StreamingIDL.*;
import org.omg.CORBA.*;
import org.omg.CORBA.StringHolder;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;


public class DumpContent extends Functionality

{

  ORB                             _orb;

  public DumpContent (Frame fParentFrame,
    ORB orb)
  {
    super (fParentFrame);
    _orb                          = orb;
  }

  public void realize (XMLTree xtXMLTree)
  {

    ChoiceFromTextInputDialog           tidTextInputDialog      = null;

    try
    {
      TreeComponent                       tcSelectedContentTreeComponent;
      TreeComponent                       tcContentDirectoryTreeComponent;
      String                              sDirectory;
      String                              sFileName;
      StreamingServer                     ssStreamingServer;
      TreeComponent                       tcServerTreeComponent;
      String                              sServerIOR;
      StringHolder                        sContentDump;
	  long                                lErrorNumber;


      tcSelectedContentTreeComponent          =
        xtXMLTree.getSelectedTreeComponent();

      tcContentDirectoryTreeComponent          =
        (TreeComponent) tcSelectedContentTreeComponent.getParent();

      if ((tcServerTreeComponent = tcSelectedContentTreeComponent.getEntityNode()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "Server not found",
          "DumpContent", JOptionPane.ERROR_MESSAGE);

        return;
      }

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  ssStreamingServer = StreamingServerHelper.narrow(
		nc.resolve(nc.to_name(tcServerTreeComponent.getAttributeValue("Identifier") + "_Server")));

      sDirectory          = tcContentDirectoryTreeComponent.getAttributeValue("PathName");
      sFileName           = tcSelectedContentTreeComponent.getAttributeValue("Identifier");

      sContentDump                  = new StringHolder ();

      if ((lErrorNumber = ssStreamingServer.dumpContent(
        sDirectory.equals("") ? sFileName :
        sDirectory + "/" + sFileName, sContentDump)) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "ssStreamingServer.dumpContent failed (Error: " +
		  lErrorNumber + ")",
          "DumpContent", JOptionPane.ERROR_MESSAGE);

        return;
      }

      tidTextInputDialog = new ChoiceFromTextInputDialog (_fParentFrame,
        "dump '" + sFileName + "' content", sContentDump.value, true);
      tidTextInputDialog.setVisible(true);
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "DeleteSite ERROR: " + e,
        "DeleteSite", JOptionPane.ERROR_MESSAGE);

      return;
    }
    finally
    {
      if (tidTextInputDialog != null)
        tidTextInputDialog.dispose();
    }
  }
}

