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



public class AddContent extends Functionality

{

  ORB                             _orb;

  public AddContent (Frame fParentFrame,
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
      TreeComponent                       tcServerTreeComponent;
      String                              sSelectedDirectory;
      String                              sServerIOR;
	  StreamingServer                     ssStreamingServer;
      JFileChooser jFileChooser1 = new JFileChooser();
      // FileFilter ffFileFilter = new FileFilter();
      File                                fFiles [];
      int                                 iFileIndex;
      FileInputStream                     fisFileInputStream;
      byte                                bContent [];
      TreeComponent                       tcContentTreeComponent;
	  long                                lErrorNumber;


      tcSelectedDirectoryTreeComponent      =
        xtXMLTree.getSelectedTreeComponent();

      sSelectedDirectory          =
        tcSelectedDirectoryTreeComponent.getAttributeValue("PathName");

      if ((tcServerTreeComponent = tcSelectedDirectoryTreeComponent.getEntityNode()) ==
        null)
      {
        JOptionPane.showMessageDialog (null,
          "Server not found",
          "AddContent", JOptionPane.ERROR_MESSAGE);

        return;
      }

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  ssStreamingServer = StreamingServerHelper.narrow(
		nc.resolve(nc.to_name(tcServerTreeComponent.getAttributeValue("Identifier") + "_Server")));

      // jFileChooser1.setFileFilter(ffFileFilter);
      jFileChooser1.setMultiSelectionEnabled(true);
      jFileChooser1.showOpenDialog(_fParentFrame);
      fFiles = jFileChooser1.getSelectedFiles();
      for (iFileIndex = 0; iFileIndex < fFiles.length; iFileIndex++)
      {
        bContent                = new byte [(int)(fFiles[iFileIndex]).length()];
        fisFileInputStream      = new FileInputStream ((fFiles[iFileIndex]));

        if (fisFileInputStream.read(bContent, 0, (int) (fFiles[iFileIndex]).length()) !=
          (int) (fFiles[iFileIndex]).length())
        {
          JOptionPane.showMessageDialog (null,
            "Error reading the " + (fFiles[iFileIndex]).getAbsolutePath() + " file",
            "AddContent", JOptionPane.PLAIN_MESSAGE);
        }

        if ((lErrorNumber = ssStreamingServer.addContent (
          sSelectedDirectory.equals("") ? (fFiles[iFileIndex]).getName() :
          sSelectedDirectory + "/" + (fFiles[iFileIndex]).getName(), bContent)) != 0)
        {
          JOptionPane.showMessageDialog (null,
            "ssStreamingServer.addContent failed (Error: " +
			lErrorNumber + ")",
            "AddContent", JOptionPane.ERROR_MESSAGE);

          return;
        }

        tcContentTreeComponent       = xtXMLTree.buildTreeComponent();
        tcContentTreeComponent.setAttribute("TagName", "Content");
        tcContentTreeComponent.setAttribute("Identifier", (fFiles[iFileIndex]).getName());
        tcContentTreeComponent.setAttribute("Label", (fFiles[iFileIndex]).getName());
        tcContentTreeComponent.setAttribute("Icon", "Content.gif");
        tcContentTreeComponent.setAttribute("DisabledFunctionalitiesList",
          "AddDirectory,DeleteDirectory,AddContent,PropagationRules");
        tcContentTreeComponent.setAttribute("DisabledFunctionalitiesListSeparator", ",");

        tcSelectedDirectoryTreeComponent.add(tcContentTreeComponent);
        tcSelectedDirectoryTreeComponent.refreshSubTree ();
      }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ERROR : " + e,
        "XMLTreeForContents", JOptionPane.ERROR_MESSAGE);

      return;
    }
  }
}

