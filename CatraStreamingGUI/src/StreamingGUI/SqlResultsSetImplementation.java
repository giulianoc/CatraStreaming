
package StreamingGUI;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Vector;
import XMLTree.SqlResultsSetInterface;


public class SqlResultsSetImplementation implements SqlResultsSetInterface

{

  int                 _iColumnsNumber;
  int                 _iRowsNumber;
  Vector              _vColumnsInfo             = new Vector ();
  Vector              _vValuesInfo              = new Vector ();


  public SqlResultsSetImplementation ()
  {
  }

  public int init (ResultSet rsResultsSet)
  {
    // super ();
    int                                       iColumnIndex;
    String                                    sValue;
    SqlResultsSetInterface.SqlValueInfo       sviValueInfo;


    /*
    _iRowsNumber            = 0;
    try
    {
      _iColumnsNumber       = rsResultsSet.getMetaData().getColumnCount();

      for (iColumnIndex = 0; iColumnIndex < _iColumnsNumber; iColumnIndex++)
      {
        _vColumnsInfo.addElement (new SqlResultsSetInterface.SqlColumnInfo (
          rsResultsSet.getMetaData().getColumnName(iColumnIndex + 1),
          rsResultsSet.getMetaData().getColumnType(iColumnIndex + 1),
          rsResultsSet.getMetaData().getColumnDisplaySize(iColumnIndex + 1)
          ));
        _vValuesInfo.addElement (new Vector ());
      }

      while (rsResultsSet.next ())
      {
        for (iColumnIndex = 0; iColumnIndex < _iColumnsNumber; iColumnIndex++)
        {
          sValue        = rsResultsSet.getString (iColumnIndex + 1);

          if (sValue == null)
            ((Vector) _vValuesInfo.elementAt (iColumnIndex)).addElement (
              new SqlResultsSetInterface.SqlValueInfo (sValue, true, 0));
          else
            ((Vector) _vValuesInfo.elementAt (iColumnIndex)).addElement (
              new SqlResultsSetInterface.SqlValueInfo (sValue, false, sValue.length ()));
        }

        _iRowsNumber++;
        // System.out.println (str);
      }
    }
    catch(SQLException e)
    {
      System. out. println ("SQLException..........." + e);
    }
    */

    return 0;
  }

  public int getRowsNumber ()
  {
    return _iRowsNumber;
  }

  public int getColumnsNumber ()
  {
    return _iColumnsNumber;
  }

  public SqlResultsSetInterface.SqlColumnInfo getColumnInfo (int iColumnIndex)
  {
    return ((SqlResultsSetInterface.SqlColumnInfo) _vColumnsInfo.elementAt (iColumnIndex));
  }

  public SqlResultsSetInterface.SqlValueInfo getSqlValueInfo (
    int iRowIndex, int iColumnIndex)
  {
    return ((SqlResultsSetInterface.SqlValueInfo)((Vector) _vValuesInfo.elementAt (iColumnIndex)).
      elementAt (iRowIndex));
  }

}
