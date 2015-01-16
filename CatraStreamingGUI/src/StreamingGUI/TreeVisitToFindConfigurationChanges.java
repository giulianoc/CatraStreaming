
package StreamingGUI;

import XMLTree.*;
import StreamingIDL.*;
import javax.swing.JOptionPane;
import java.util.Vector;
import java.util.Enumeration;
import java.util.StringTokenizer;
import java.text.SimpleDateFormat;
import java.util.Date;


public class TreeVisitToFindConfigurationChanges implements TreeVisitInterface

{

  Vector                            _vConfigurationItems;
  Vector                            _vLastSavedDateForConfigurationItems;
  SimpleDateFormat                  _sdfDisplayFormatter;


  public TreeVisitToFindConfigurationChanges (Vector vConfigurationItems,
    Vector vLastSavedDateForConfigurationItems)
  {
    _vConfigurationItems                  = vConfigurationItems;
    _vLastSavedDateForConfigurationItems  = vLastSavedDateForConfigurationItems;
    _sdfDisplayFormatter                  = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
  }

  public int beginNodeVisit (TreeComponent tcCurrentTreeComponent)
  {
    return 0;
  }

  public int nodeVisit (TreeComponent tcCurrentTreeComponent)

  {

    try
    {
      ConfigurationItem         ciConfigurationItem     = new ConfigurationItem ();
      TreeComponent             tcConfigurationItemValue;
      TreeComponent             tcConfigurationSectionTreeComponent;


      if (!(tcCurrentTreeComponent instanceof Choice))
        return 0;

      tcConfigurationSectionTreeComponent         =
        (TreeComponent) tcCurrentTreeComponent.getParent();

      tcConfigurationItemValue                    =
        ((TreeComponent)tcCurrentTreeComponent.getChildAt(0));

      if (tcConfigurationItemValue.getAttributeValue("Modified").compareTo(
        _sdfDisplayFormatter.format(
        (Date) _vLastSavedDateForConfigurationItems.elementAt(0))) <= 0)
        return 0;

      ciConfigurationItem.pSectionName             =
        tcConfigurationSectionTreeComponent.getAttributeValue("Identifier");
      ciConfigurationItem.pItemName             =
        tcCurrentTreeComponent.getAttributeValue("Identifier");
      ciConfigurationItem.pItemValue             =
        tcConfigurationItemValue.getAttributeValue("OutputParameter");

      _vConfigurationItems.addElement (ciConfigurationItem);
    }
    catch(Exception e)
    {
      JOptionPane.showMessageDialog (null,
        "TreeVisitToFindConfigurationChanges ERROR: " + e,
        "XMLTreeForConfiguration", JOptionPane.ERROR_MESSAGE);
    }

    return 0;
  }

  public int endNodeVisit (TreeComponent tcCurrentTreeComponent)
  {
    return 0;
  }
}
