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


public class AddDirectory extends Functionality

{

  ORB                             _orb;


  public AddDirectory (Frame fParentFrame,
    ORB orb)
  {
    super (fParentFrame);
    _orb                          = orb;
  }

  public void realize (XMLTree xtXMLTree)
  {
    TreeComponent                   tcSelectedDirectoryTreeComponent;
    String                          sSelectedText;

    ChoiceFromTextInputDialog       tidTextInputDialog      = null;


    try
    {
      tcSelectedDirectoryTreeComponent      =
        xtXMLTree.getSelectedTreeComponent();

      tidTextInputDialog = new ChoiceFromTextInputDialog (_fParentFrame,
        "Add directory", "", true);
      tidTextInputDialog.setVisible(true);

      sSelectedText     = tidTextInputDialog. getSelectedText ();

      if (sSelectedText != null)
      {
        TreeComponent                       tcDirectoryTreeComponent;
        TreeVisit                           tvTreeVisit;
        TreeVisitToFindTreeComponent        tvftcTreeVisitToFindTreeComponent;
        StreamingServer                     ssStreamingServer;
        TreeComponent                       tcServerTreeComponent;
        String                              sServerIOR;
        String                              sSelectedDirectory;
		long                                lErrorNumber;

        // verify if the site already exists
        tvftcTreeVisitToFindTreeComponent   = new TreeVisitToFindTreeComponent (
          "Directory", sSelectedText);
        tvTreeVisit             = new TreeVisit (tvftcTreeVisitToFindTreeComponent);

        if (tvTreeVisit.preOrderVisit(tcSelectedDirectoryTreeComponent) != 0)
        {
          JOptionPane.showMessageDialog (null,
            "preOrderVisit failed", "AddDirectory",
            JOptionPane.ERROR_MESSAGE);

          return;
        }

        if (tvftcTreeVisitToFindTreeComponent.getTreeComponentFound () !=
          null)
        {
          JOptionPane.showMessageDialog (null,
            "Directory already exists", "AddDirectory",
            JOptionPane.PLAIN_MESSAGE);

          return;
        }

        if ((tcServerTreeComponent = tcSelectedDirectoryTreeComponent.getEntityNode()) ==
          null)
        {
          JOptionPane.showMessageDialog (null,
            "Server not found",
            "AddDirectory", JOptionPane.ERROR_MESSAGE);

          return;
        }

		// name service
		NamingContextExt nc =
		  NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		  "NameService"));
		ssStreamingServer = StreamingServerHelper.narrow(
		  nc.resolve(nc.to_name(tcServerTreeComponent.getAttributeValue(
		  "Identifier") + "_Server")));

        sSelectedDirectory          =
          tcSelectedDirectoryTreeComponent.getAttributeValue("PathName");

        if ((lErrorNumber = ssStreamingServer.addDirectory(
          sSelectedDirectory.equals("") ? sSelectedText :
          sSelectedDirectory + "/" + sSelectedText)) != 0)
        {
          JOptionPane.showMessageDialog (null,
            "ssStreamingServer.addDirectory failed (Error: " +
			lErrorNumber + ")",
            "AddDirectory", JOptionPane.ERROR_MESSAGE);

          return;
        }

        tcDirectoryTreeComponent       = xtXMLTree.buildTreeComponent();
        tcDirectoryTreeComponent.setAttribute("TagName", "Directory");
        tcDirectoryTreeComponent.setAttribute("Identifier", sSelectedText);
        tcDirectoryTreeComponent.setAttribute("Label", sSelectedText);
        tcDirectoryTreeComponent.setAttribute("Icon", "Directory.gif");
        tcDirectoryTreeComponent.setAttribute("PathName",
          sSelectedDirectory.equals("") ? sSelectedText :
          sSelectedDirectory + "/" + sSelectedText);
        tcDirectoryTreeComponent.setAttribute("DisabledFunctionalitiesList",
          "DeleteContent,DuplicateContent,SaveContentLocally,DumpContent,GetTracksInfo,AddHintTrack,AddHintTracks,DeleteTrack,PropagationRules");
        tcDirectoryTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator", ",");

        tcSelectedDirectoryTreeComponent.insert(tcDirectoryTreeComponent, 0);
        xtXMLTree.getXMLTopTreeComponent().refreshSubTree ();
      }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "AddDirectory ERROR: " + e,
        "AddDirectory", JOptionPane.ERROR_MESSAGE);

      return;
    }
    finally
    {
      if (tidTextInputDialog != null)
        tidTextInputDialog.dispose();
    }

    return;
  }
}
