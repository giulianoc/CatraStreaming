
package StreamingGUI;

import javax.swing.tree.DefaultMutableTreeNode;
import java.util.Vector;
import XMLTree.*;


public class TreeVisitToGetConfigurationFile implements TreeVisitInterface

{

  String              _sCurrentXML;
  int                 _iCurrentLevel;
  Vector              _vFunctionalities;
  boolean             _bIsFunctionalitiesAdded;
  String              _sNewLine = "\r\n";


  public TreeVisitToGetConfigurationFile (Vector vFunctionalities)
  {
    _iCurrentLevel              = 0;
    _sCurrentXML                =
	  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + _sNewLine;
    _vFunctionalities           = vFunctionalities;
    _bIsFunctionalitiesAdded    = false;
  }

  public int beginNodeVisit (TreeComponent tcCurrentTreeComponent)
  {
    int                       iCurrentLevelIndex;
    String []                 sAttributesNames;
    int                       iAttributesNumber;
    int                       iAttributeIndex;


    if (!(tcCurrentTreeComponent.getAttributeValue("TagName").
		equals ("Configurations") ||
		tcCurrentTreeComponent.getAttributeValue("TagName").
		equals ("Site") ||
		tcCurrentTreeComponent.getAttributeValue("TagName").
		equals ("Entity")))
	    return 0;

    for (iCurrentLevelIndex = 0; iCurrentLevelIndex < _iCurrentLevel;
      iCurrentLevelIndex++)
    _sCurrentXML           += "\t";

    _sCurrentXML           += "<" +
      tcCurrentTreeComponent.getAttributeValue("TagName");

    sAttributesNames        = tcCurrentTreeComponent.getAttributesNames();

    iAttributesNumber       = sAttributesNames.length;
    for (iAttributeIndex = 0; iAttributeIndex < iAttributesNumber;
      iAttributeIndex++)
    {
      if (sAttributesNames[iAttributeIndex].equals("TagName"))
        continue;

      _sCurrentXML          += (" " + sAttributesNames [iAttributeIndex] +
        "=\"" + tcCurrentTreeComponent.getAttributeValue(
        sAttributesNames [iAttributeIndex]) + "\"");
    }

    _sCurrentXML            += (">" + _sNewLine);

    if (!_bIsFunctionalitiesAdded)
    {
      int           iFunctionalityIndex;
      String        sCurrentFunctionalityIdentifier;
      Functionality fCurrentFunctionality;

      for (iFunctionalityIndex = 0;
        iFunctionalityIndex < _vFunctionalities.size() - 1;
        iFunctionalityIndex++)
      {
        fCurrentFunctionality               = (Functionality) (_vFunctionalities.
          elementAt(iFunctionalityIndex));

        _sCurrentXML           += "\t";

        _sCurrentXML           += "<" +
          fCurrentFunctionality.getAttributeValue("TagName");

        sAttributesNames        = fCurrentFunctionality.getAttributesNames();

        iAttributesNumber       = sAttributesNames.length;
        for (iAttributeIndex = 0; iAttributeIndex < iAttributesNumber;
          iAttributeIndex++)
        {
          if (sAttributesNames[iAttributeIndex].equals("TagName"))
            continue;

          _sCurrentXML          += (" " + sAttributesNames [iAttributeIndex] +
            "=\"" + fCurrentFunctionality.getAttributeValue(
            sAttributesNames [iAttributeIndex]) + "\"");
        }

        _sCurrentXML            += (">" + _sNewLine);

        _sCurrentXML           += "\t";
        _sCurrentXML            += ("</" +
          fCurrentFunctionality.getAttributeValue("TagName") + ">" + _sNewLine);
      }

      _bIsFunctionalitiesAdded        = true;
    }

    _iCurrentLevel++;


    return 0;
  }

  public int nodeVisit (TreeComponent tcCurrentTreeComponent)
  {
    return 0;
  }

  public int endNodeVisit (TreeComponent tcCurrentTreeComponent)

  {

    int                         iCurrentLevelIndex;


    if (!(tcCurrentTreeComponent.getAttributeValue("TagName").
		equals ("Configurations") ||
		tcCurrentTreeComponent.getAttributeValue("TagName").
		equals ("Site") ||
		tcCurrentTreeComponent.getAttributeValue("TagName").
		equals ("Entity")))
	    return 0;

    _iCurrentLevel--;

    for (iCurrentLevelIndex = 0; iCurrentLevelIndex < _iCurrentLevel;
      iCurrentLevelIndex++)
    _sCurrentXML           += "\t";

    _sCurrentXML            += ("</" +
      tcCurrentTreeComponent.getAttributeValue("TagName") + ">" + _sNewLine);


    return 0;
  }

  public String getConfigurationFile ()

  {

    return _sCurrentXML;
  }
}
