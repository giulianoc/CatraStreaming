package StreamingGUI;

import XMLTree.*;

import com.ibm.xml.generator.DOMGenerator;
import com.ibm.xml.base.treesupport.XMLTreeCellRenderer;
import com.ibm.xml.base.treesupport.event.NodeSelectionEvent;
import com.ibm.xml.tokenizer.XMLTokenizer;
import org.w3c.dom.Element;
import com.ibm.xml.tokenizer.event.*;
import javax.swing.JOptionPane;
import java.net.URL;
import javax.swing.tree.*;
import java.util.Vector;
import java.util.Date;
import java.util.Enumeration;
import java.util.Properties;
import java.util.Hashtable;
import org.omg.CORBA.ORB;

import StreamingIDL.*;

public class XMLTreeForConfiguration extends XMLTree {

  private XMLTreeForContents    _xtfcXMLTreeForContents;
  private XMLTreeForMonitor     _xtfmXMLTreeForMonitor;
  private ORB                   _orb;
  private Vector                _vLastSavedDateForConfigurationItems = new Vector ();

  private XMLTreeNodeData       _xtndSiteOrServer;
  private boolean               _bSiteTag;
  private boolean               _bServerTag;
  private boolean               _bAddedSite;


  public XMLTreeForConfiguration ()
  {
    super ();
    System.out.println("XMLTreeForConfiguration");
	_xtndSiteOrServer             = null;
	_bSiteTag                     = false;
	_bServerTag                   = false;
	_bAddedSite                   = false;
  }

  public int init (DBInterface dbiDBInterface, URL uXML, Properties pProperties,
    ORB orb, XMLTreeForContents xtfcXMLTreeForContents,
    XMLTreeForMonitor xtfmXMLTreeForMonitor)
  {
    _orb                          = orb;
    _xtfcXMLTreeForContents       = xtfcXMLTreeForContents;
    _xtfmXMLTreeForMonitor        = xtfmXMLTreeForMonitor;
    _vLastSavedDateForConfigurationItems.addElement(new Date ());

    return super.init (dbiDBInterface, uXML, pProperties);
  }

  public void setLastSavedDateForConfigurationItems (Date dNewDate)
  {
    _vLastSavedDateForConfigurationItems.setElementAt(dNewDate, 0);
  }

  protected void xtXMLTokenizer_elementStartTagFound(ElementTokenEvent e)
  {
    String                sToken		= e.getToken();

    System.out.println("elementStartTagFound: "+sToken);


    if (sToken.compareTo(new String ("Site")) == 0)
    {
      _xtndSiteOrServer      = new XMLTreeNodeData ();
	  _bSiteTag              = true;
	  _bServerTag            = false;
	  _bAddedSite            = false;
    }
    else if (sToken.compareTo(new String ("Entity")) == 0)
    {
	  _xtndSiteOrServer      = null;
      _xtndSiteOrServer      = new XMLTreeNodeData ();
	  _bServerTag            = true;
    }
    else if (sToken.compareTo("AddSiteFunctionality") == 0)
    {
      if (addFunctionality (new AddSite (getParentFrame (),
        _pProperties, this, _xtfcXMLTreeForContents, _xtfmXMLTreeForMonitor),
		sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for AddSite",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DeleteSiteFunctionality") == 0)
    {
      if (addFunctionality (new DeleteSite (getParentFrame (),
        _pProperties, this, _xtfcXMLTreeForContents,
		_xtfmXMLTreeForMonitor), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DeleteSite",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("AddServersFunctionality") == 0)
    {
      if (addFunctionality (new AddServers (getParentFrame (), _pProperties,
	    this, _xtfcXMLTreeForContents, _xtfmXMLTreeForMonitor,
		_orb, _vLastSavedDateForConfigurationItems), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for AddServers",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DeleteServerFunctionality") == 0)
    {
      if (addFunctionality (new DeleteServer (getParentFrame (), _pProperties,
	    this, _xtfcXMLTreeForContents, _xtfmXMLTreeForMonitor), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DeleteServer",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("ActivateServerFunctionality") == 0)
    {
      if (addFunctionality (new ActivateServer (getParentFrame (),
	    _pProperties, _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for ActivateServer",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("DeactivateServerFunctionality") == 0)
    {
      if (addFunctionality (new DeactivateServer (getParentFrame (),
	    _pProperties, _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for DeactivateServer",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("ShutdownServerFunctionality") == 0)
    {
      if (addFunctionality (new ShutdownServer (getParentFrame (),
	    _pProperties, _orb), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for ShutdownServer",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("ActivateServerConfigurationFunctionality") == 0)
    {
      if (addFunctionality (new ActivateServerConfiguration (getParentFrame (),
        _orb, _vLastSavedDateForConfigurationItems), sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for ActivateServerConfiguration",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("ChangeInEverySiteServerFunctionality") == 0)
    {
      if (addFunctionality (new ChangeInEverySiteServer (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for ChangeInEverySiteServer",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else if (sToken.compareTo("ChangeInEveryServerFunctionality") == 0)
    {
      if (addFunctionality (new ChangeInEveryServer (getParentFrame ()),
        sToken) != 0)
      {
        JOptionPane.showMessageDialog (null,
          "addFunctionality failed for ChangeInEveryServer",
          "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);

        return;
      }
    }
    else
      super.xtXMLTokenizer_elementStartTagFound(e);
  }

  protected void xtXMLTokenizer_elementEndTagFound(ElementTokenEvent e)
  {
    String												sToken		= e.getToken();

    // System.out.println ("End token: " + sToken);

    try
    {
	  if (sToken.compareTo(new String("Site")) == 0)
	  {
		_xtndSiteOrServer    = null;
		_bSiteTag            = false;
		_bAddedSite          = false;

		_tcCurrentTreeComponent               =
		  (TreeComponent) (_tcCurrentTreeComponent.getParent());
	  }
	  else if (sToken.compareTo(new String("Entity")) == 0)
	  {
		AddServers           fAddServers;
		Vector               vServersNames    = new Vector ();

		if ( (fAddServers = (AddServers) getFunctionality("AddServers")) == null)
		{
		  JOptionPane.showMessageDialog(null,
            "getFunctionality failed",
			"XMLTreeForConfiguration",
			JOptionPane.ERROR_MESSAGE);

		  return;
		}

		vServersNames.add(_xtndSiteOrServer.getAttributeValue("Identifier"));

		if (fAddServers.addServers(this, _tcCurrentTreeComponent,
		  vServersNames, false) != 0)
		{
		  JOptionPane.showMessageDialog(null,
			"addServers failed",
			"XMLTreeForConfiguration",
			JOptionPane.ERROR_MESSAGE);

		  return;
		}

		_xtndSiteOrServer    = null;
		_bServerTag          = false;
	  }
	  else {
		super.xtXMLTokenizer_elementEndTagFound (e);
	  }
    }
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
  }

  protected void xtXMLTokenizer_attributeNameFound(AttributeTokenEvent e)
  {

    String												sToken		= e.getToken();


    try
    {
	  if (_bSiteTag == true && _bServerTag == false)
		_xtndSiteOrServer.prepareForSetAttribute(sToken);
	  else if (_bSiteTag == true && _bServerTag == true)
		_xtndSiteOrServer.prepareForSetAttribute(sToken);
	  else if (_bSiteTag == false && _bServerTag == true)
      {
        JOptionPane.showMessageDialog (null,
          "XML wrong",
          "XMLTreeForConfiguration",
          JOptionPane.ERROR_MESSAGE);

        return;
      }
	  else
		super.xtXMLTokenizer_attributeNameFound(e);
    }
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
  }

  protected void xtXMLTokenizer_attributeValueFound(AttributeTokenEvent e)
  {

    String								sToken		= e.getToken();


    try
    {
	  if (_bSiteTag == true && _bServerTag == false)
	  {
		AddSite              fAddSite;
		String               sSiteIdentifier;
		TreeComponent        tcConfigSiteTreeComponent;

        _xtndSiteOrServer.setAttribute(sToken);

		if (_bAddedSite == false &&
		  (sSiteIdentifier = _xtndSiteOrServer.getAttributeValue(
		  "Identifier")) != null)
		{
		  if ( (fAddSite = (AddSite) getFunctionality("AddSite")) == null)
		  {
			JOptionPane.showMessageDialog(null,
              "getFunctionality failed",
			  "XMLTreeForConfiguration",
			  JOptionPane.ERROR_MESSAGE);

			return;
		  }

		  if ((tcConfigSiteTreeComponent = fAddSite.addSite(
		    this, _tcCurrentTreeComponent,
		    _xtndSiteOrServer.getAttributeValue("Identifier"),
			false)) == null)
		  {
			JOptionPane.showMessageDialog(null,
			  "addSite failed",
			  "XMLTreeForConfiguration",
			  JOptionPane.ERROR_MESSAGE);

			return;
		  }

		  _bAddedSite = true;

		  _tcCurrentTreeComponent      = tcConfigSiteTreeComponent;
		}
	  }
	  else if (_bSiteTag == true && _bServerTag == true)
        _xtndSiteOrServer.setAttribute(sToken);
	  else if (_bSiteTag == false && _bServerTag == true)
      {
        JOptionPane.showMessageDialog (null,
          "XML wrong",
          "XMLTreeForConfiguration",
          JOptionPane.ERROR_MESSAGE);

        return;
      }
      else
		super.xtXMLTokenizer_attributeValueFound(e);
    }
    catch (Exception ex)
    {
      ex.printStackTrace();
    }
  }
}
