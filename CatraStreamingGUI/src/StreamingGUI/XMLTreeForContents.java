package StreamingGUI;

import XMLTree.*;

import com.ibm.xml.generator.DOMGenerator;
import com.ibm.xml.base.treesupport.XMLTreeCellRenderer;
import com.ibm.xml.base.treesupport.event.NodeSelectionEvent;
import com.ibm.xml.tokenizer.XMLTokenizer;
import org.w3c.dom.Element;
import com.ibm.xml.tokenizer.event.*;
import javax.swing.JOptionPane;

import javax.swing.*;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Properties;
import org.omg.CORBA.ORB;
import java.util.Hashtable;
import java.net.URL;

public class XMLTreeForContents extends XMLTree {

  ORB                           _orb;
  JTextField                    _jtfURLForPlayer;
  XMLTreeForConfiguration       _xtfcXMLTreeForConfiguration;

  public XMLTreeForContents (JTextField jtfURLForPlayer)
  {
    super ();
    _jtfURLForPlayer              = jtfURLForPlayer;
    System.out.println("XMLTreeForContents");
  }

  public int init (DBInterface dbiDBInterface, URL uXML, Properties pProperties,
    ORB orb, XMLTreeForConfiguration xtfcXMLTreeForConfiguration)
  {
    _orb                          = orb;
    _xtfcXMLTreeForConfiguration  = xtfcXMLTreeForConfiguration;

    return super.init (dbiDBInterface, uXML, pProperties);
  }

  protected void handleEnabledFunctionalities ()
  {
    super.handleEnabledFunctionalities();

    try
    {
	  if (getSelectedTreeComponent() != null)
	  {
		if (getSelectedTreeComponent().getAttributeValue("TagName").equals(
		  "Content")) {
		  TreeComponent tcSelectedContentTreeComponent;
		  TreeComponent tcContentDirectoryTreeComponent;
		  String sHostName;
		  String sRTSPRequestsPort;
		  String sDirectory;
		  String sFileName;
		  TreeComponent tcServerTreeComponentInContentsTree;
		  Choice cRTSPRequestsPort;

		  tcSelectedContentTreeComponent = getSelectedTreeComponent();

		  tcContentDirectoryTreeComponent =
			(TreeComponent) tcSelectedContentTreeComponent.getParent();

		  if ( (tcServerTreeComponentInContentsTree =
				tcSelectedContentTreeComponent.getEntityNode()) == null) {
			JOptionPane.showMessageDialog(null,
										 "Server not found",
										 "XMLTreeForContents",
										 JOptionPane.ERROR_MESSAGE);

			return;
		  }

		  if ( (sHostName = tcServerTreeComponentInContentsTree.
				getAttributeValue(
			"HostName")) ==
			  null) {
			JOptionPane.showMessageDialog(null,
										 "HostName attribute not found",
										 "XMLTreeForContents",
										 JOptionPane.ERROR_MESSAGE);

			return;
		  }

		  sDirectory = tcContentDirectoryTreeComponent.getAttributeValue(
			"PathName");
		  sFileName = tcSelectedContentTreeComponent.getAttributeValue(
			"Identifier");

		  // Search of the RTSP port for the specified server searching before the
		  // server node in the configuration tree
		  {
			TreeVisit tvTreeVisit;
			TreeVisitToFindTreeComponent tvftcTreeVisitToFindTreeComponent;
			TreeComponent tcServerTreeComponentInConfigurationsTree;

			tvftcTreeVisitToFindTreeComponent = new
			  TreeVisitToFindTreeComponent(
"Entity",
			  tcServerTreeComponentInContentsTree.getAttributeValue(
			  "Identifier"));
			tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

			if (tvTreeVisit.preOrderVisit(
			  _xtfcXMLTreeForConfiguration.getXMLTopTreeComponent()) != 0) {
			  JOptionPane.showMessageDialog(null,
										   "preOrderVisit failed",
										   "XMLTreeForContents",
										   JOptionPane.PLAIN_MESSAGE);

			  return;
			}

			if ( (tcServerTreeComponentInConfigurationsTree =
				  tvftcTreeVisitToFindTreeComponent.getTreeComponentFound()) == null) {
			  JOptionPane.showMessageDialog(null,
										   "The " +
										   tcServerTreeComponentInContentsTree.
										   getAttributeValue("Identifier") +
										   " server doesn't exist",
										   "XMLTreeForContents",
										   JOptionPane.ERROR_MESSAGE);

			  return;
			}

			tvftcTreeVisitToFindTreeComponent = new
			  TreeVisitToFindTreeComponent(
			  "ChoiceFromNumberInput", "RTSPRequestsPort");
			tvTreeVisit = new TreeVisit(tvftcTreeVisitToFindTreeComponent);

			if (tvTreeVisit.preOrderVisit(
			  tcServerTreeComponentInConfigurationsTree) != 0) {
			  JOptionPane.showMessageDialog(null,
										   "preOrderVisit failed",
										   "XMLTreeForContents",
										   JOptionPane.PLAIN_MESSAGE);

			  return;
			}

			if ( (cRTSPRequestsPort =
				  (Choice) tvftcTreeVisitToFindTreeComponent.
				  getTreeComponentFound()) == null) {
			  JOptionPane.showMessageDialog(null,
				"The 'RTSPRequestsPort' configuration item doesn't exist",
				"XMLTreeForContents", JOptionPane.ERROR_MESSAGE);

			  return;
			}

			sRTSPRequestsPort = ( (TreeComponent) cRTSPRequestsPort.getChildAt(
			  0)).
			  getAttributeValue("OutputParameter");
		  }

		  _jtfURLForPlayer.setText("URL for player: rtsp://" + sHostName + ":" +
								   sRTSPRequestsPort + "/" +
								   (sDirectory.equals("") ? sFileName :
									sDirectory + "/" + sFileName));
		}
	  }
	  else
	  {
		  _jtfURLForPlayer.setText("URL for player: <none>");
	  }
    }
    catch (Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "ERRORaaaaaaaaaa: " + e,
        "XMLTreeForContents",
        JOptionPane.ERROR_MESSAGE);

      return;
    }
  }

  protected void xtXMLTokenizer_elementStartTagFound(ElementTokenEvent e)
  {
    String                sToken		= e.getToken();

    System.out.println("elementStartTagFound: "+sToken);

    if (sToken.compareTo("AddDirectoryFunctionality") == 0)
    {
      if (addFunctionality (new AddDirectory (getParentFrame (),
        _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for AddDirectory",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DeleteDirectoryFunctionality") == 0)
    {
      if (addFunctionality (new DeleteDirectory (getParentFrame (),
        _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DeleteDirectory",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("AddContentFunctionality") == 0)
    {
      if (addFunctionality (new AddContent (getParentFrame (),
        _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for AddContent",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DeleteContentFunctionality") == 0)
    {
      if (addFunctionality (new DeleteContent (getParentFrame (),
        _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DeleteContent",
          "XMLTreeForContent", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DuplicateContentFunctionality") == 0)
    {
      if (addFunctionality (new DuplicateContent (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DuplicateContent",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("SaveContentLocallyFunctionality") == 0)
    {
      if (addFunctionality (new SaveContentLocally (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for SaveContentLocally",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DumpContentFunctionality") == 0)
    {
      if (addFunctionality (new DumpContent (getParentFrame (),
        _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DumpContent",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("GetTracksInfoFunctionality") == 0)
    {
      if (addFunctionality (new GetTracksInfo (getParentFrame (),
        _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for GetTracksInfo",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("AddHintTrackFunctionality") == 0)
    {
      if (addFunctionality (new AddHintTrack (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for AddHintTrack",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("AddHintTracksFunctionality") == 0)
    {
      if (addFunctionality (new AddHintTracks (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for AddHintTracks",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DeleteTrackFunctionality") == 0)
    {
      if (addFunctionality (new DeleteTrack (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DeleteTrack",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("PropagationRulesFunctionality") == 0)
    {
      if (addFunctionality (new PropagationRules (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for PropagationRules",
          "XMLTreeForContents", JOptionPane.PLAIN_MESSAGE);

        return;
      }
    }
    else
      super.xtXMLTokenizer_elementStartTagFound(e);
  }
}
