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

public class ActivateServer extends Functionality

{

  Properties                    _pProperties;
  ORB                           _orb;

  public ActivateServer (Frame fParentFrame,
    Properties pProperties, ORB orb)
  {
    super (fParentFrame);
    _orb                         = orb;
	_pProperties                 = pProperties;
  }

  public void realize (XMLTree xtXMLTree)
  {
    try
    {
      TreeComponent                   tcServerTreeComponent;
      TreeComponent                   tcSiteTreeComponent;
	  StreamingServer                 ssStreamingServer;
	  long                            lErrorNumber;


      if (JOptionPane.showOptionDialog (null,
        "Are you sure?",
        "ActivateServer", JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE, null, null, null) == 1)
        return;

      tcServerTreeComponent          =
        xtXMLTree.getSelectedTreeComponent();

	  // name service
	  NamingContextExt nc =
		NamingContextExtHelper.narrow(_orb.resolve_initial_references(
		"NameService"));
	  ssStreamingServer = StreamingServerHelper.narrow(
		nc.resolve(nc.to_name(tcServerTreeComponent.getAttributeValue("Identifier") + "_Server")));

      if ((lErrorNumber = ssStreamingServer.activate ()) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "ssStreamingServer.activate failed (Error: " +
		  lErrorNumber + ")",
          "ActivateServer", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ActivateServer ERROR: " + e,
        "ActivateServer", JOptionPane.ERROR_MESSAGE);

      return;
    }
  }
}
